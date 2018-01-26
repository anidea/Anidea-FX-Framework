#include "game_rfid.h"
#include "wiring_private.h"

rfid::rfid() : Game()
{
  // Things that need to be set up when the game is first created
  digitalWrite(RS485_ENABLE, LOW);

  // Only enable the inputs/outputs that are not going to be used by this game
  INPUT_OVERRIDE_ENABLE[0] = 0;
  INPUT_OVERRIDE_ENABLE[1] = 0;
  INPUT_OVERRIDE_ENABLE[2] = 0;
  INPUT_OVERRIDE_ENABLE[3] = 1;
  INPUT_OVERRIDE_ENABLE[4] = 1;
  INPUT_OVERRIDE_ENABLE[5] = 1;

  OUTPUT_OVERRIDE_ENABLE[0] = 0;
  OUTPUT_OVERRIDE_ENABLE[1] = 0;
  OUTPUT_OVERRIDE_ENABLE[2] = 0;
  OUTPUT_OVERRIDE_ENABLE[3] = 0;
  OUTPUT_OVERRIDE_ENABLE[4] = 0;
  OUTPUT_OVERRIDE_ENABLE[5] = 0;

  RELAY_OVERRIDE_ENABLE[0] = 0;
  RELAY_OVERRIDE_ENABLE[1] = 1;

  Serial.print(F("!!!Anidea FX - "));
  Serial.print(iTotalScanLength);
  Serial.println(F(" port RFID Scanner!!!"));

  reset();
}

void rfid::loop(void)
{
  // put your main code here, to run repeatedly:
  
  // Do generic loop actions
  Game::loop();

  buttons();
  keypadCodeFlag = true;

  switch (_gameState)
  {
    case GAMESTATE_START:
      if (require_enable)
      {
        if (_enabled)
        {
          _gameState = GAMESTATE_RUN;
        }
      }
      else
      {
        _gameState = GAMESTATE_RUN;
      }
      break;

    case GAMESTATE_RUN:
      iCommFailure = 0;
      RfidSetGetTagIds(1);
    
      if (iCommFailure > 0) // Detect failure
      {
        // We had a failure
        // Red light
        digitalWrite(ERROR_LIGHT, HIGH);
      }
      else
      {
        // Red light off
        digitalWrite(ERROR_LIGHT, LOW);
      }
    
      if (iLoop++ & 0x01) // Alternate lights for fun/activity
      {
        // Blue light
        digitalWrite(ACTIVITY_LIGHT_1, HIGH);
        // Green light
        digitalWrite(ACTIVITY_LIGHT_2, LOW);
      }
      else
      {
        // Blue light
        digitalWrite(ACTIVITY_LIGHT_1, LOW);
        // Green light
        digitalWrite(ACTIVITY_LIGHT_2, HIGH);
      }
      
      if ((iCommFailure == 0) && (iEmergencyExit == 0)) //See how many are matched
      {
        iAllTagsMatched = 1;
        iTagMatchCount = 0;
        // Do the rfid arrays match?
        for (byte i = 0; i < iTotalScanLength; i++)
        {
          if (tagFails[i] < 3)
          {
            iTagMatchCount++;
          }
          else
          {
            // Mismatch
            iAllTagsMatched = 0;
          }
        }
      }
      if (iTagMatchCount >= iTotalScanLength - iTagOffset) // Check for game completed
      {
        _gameState = GAMESTATE_SOLVED;
      }
      break;

    case GAMESTATE_SOLVED:
      // Run solved and any other one time routine
      solved();
      _gameState = GAMESTATE_ENDLOOP;

      break;
 
    default:
    case GAMESTATE_ENDLOOP:
      // Wait for reset
      
      break;
  }
}

void rfid::solved(void)
{
  Serial.println(F("rfid solved"));

  //Call generic solve function
  Game::solved();

  //Do game specific solved state
  digitalWrite(MAG_LOCK, LOW);
  digitalWrite(SOUND_CUE, HIGH);
  delay(250);
  digitalWrite(SOUND_CUE, LOW);
}

void rfid::reset(void)
{
  Serial.println(F("rfid reset"));

  //Reset global game variables
  Game::reset();

  //Reset game specific variables
  iPuzzleSolved = 0;
  iEmergencyExit = 0;
  for (byte i = 0; i < iTotalScanLength; i++)
  {
    tagFails[i] = 3;
    tagStatesOld[i] = 2;
  }

  // Reset outputs - maglocks are energized all the time
  digitalWrite(SOUND_CUE, LOW);
  digitalWrite(MAG_LOCK, HIGH);
}

void rfid::RS485_SendMessage(char *pMessage, char *pResponse, uint32_t *puLength)
{
//  Uart mySerial (&sercom5, 38, 37, SERCOM_RX_PAD_3, UART_TX_PAD_2);
//  mySerial.begin(115200);
//  pinPeripheral(37, PIO_SERCOM);
//  pinPeripheral(38, PIO_SERCOM);
//  delay(100);
  
  delay(10);
  byte pos = 0;
  digitalWrite(RS485_ENABLE, HIGH);

  // Write data
  Serial.println(pMessage);
  delay(2);

  digitalWrite(RS485_ENABLE, LOW);

  delay(10);  // Wait for receive to get here.

  // Spin while waiting for time out, and not signals
  byte timeout = 0;
  while(!Serial.available() && timeout < 50)
  {
    delay(1);
    timeout++;
  }

  delay(10);
  
  while(Serial.available())
  {
    pResponse[pos] = (char)Serial.read();
    pos++; // Increment where to write next
  }

  char *pStartMsgPos, *pTermMsgPos;
  size_t uReadOffset = 0;
  uint32_t uRxCount = *puLength;

  pStartMsgPos = pResponse;

  // Eat characters just sent
  do
  {
    // Find first # in the string
    pStartMsgPos = (char*)memmem(pStartMsgPos, uRxCount, "###", 3);

    if (pStartMsgPos == 0)
    {
      *puLength = 0;
      Serial.print(F("Write "));
      Serial.print(pMessage);
      Serial.println(F(", Read NOTHING"));
      return;
    }

    // Find the last and start the message reading there.
    do
    {
      pStartMsgPos++;
      uReadOffset++;
    }while (*pStartMsgPos == '#');

    pTermMsgPos = (char*)memmem(pStartMsgPos, uRxCount - uReadOffset, "##", 2);

    if (pTermMsgPos && pStartMsgPos && (pTermMsgPos - pStartMsgPos < 5))
    {
      // retry if we don't have a minimum message
      pStartMsgPos += 1;  // Skip a head a bit
    }
    else
    {
      break;
    }
  }while(1);


  // Do we have a full message
  if (pTermMsgPos)
  {
    // We have a full message, is it for us

    // Terminate our message
    *pTermMsgPos = 0;

    strcpy(pResponse, pStartMsgPos);
    *puLength = strlen(pStartMsgPos);
  }

  Serial.print(F("Write "));
  Serial.print(pMessage);
  Serial.print(F(", Read "));
  Serial.println(pResponse);
}

byte rfid::RfidSetGetTagIds(byte iGet)
{
  byte iRet = 0;
  byte iLoadReadCount;
  char cBufOut[32], cBufIn[32];
  
  for (byte i = 0; i < iTotalScanLength; i++)
  {
    buttons();
    sprintf(cBufOut, "###255,%d,REQRFID###", i);

    uReceiveLen = sizeof(cBufIn);

    memset(cBufIn,0,32);
    RS485_SendMessage(cBufOut, cBufIn, &uReceiveLen);

    int uSrc = iTotalScanLength + 1, uDest = 0;
    char cMsg[RFID_STR_LEN_MAX + 1];
    memset(cMsg,0,RFID_STR_LEN_MAX + 1);
    sscanf(cBufIn, "%d,%d,%s", &uSrc, &uDest, cMsg);
    
    if (uReceiveLen == 0)
    {
//      Serial.println(F("No response"));
      tagFound[i] = false;
      iCommFailure = 1;
    }
    else if (uSrc != i)
    {
//      Serial.println(F("Wrong response"));
      tagFound[i] = false;
      iCommFailure = 1;
    }
    else
    {
//      Serial.println(F("Got response"));
      if (!strcmp("NONEXXXXXXXX", cMsg))
      {
        tagFound[i] = false;
      }
      else
      {
        tagFound[i] = true;
      }
    }

    if (iGet)
    {
//      Serial.println(F("Getting tag"));
      char savedTag[RFID_STR_LEN_MAX + 1];
      EEPROMReadString(EEPROM_START + RFID_STR_LEN_MAX * i, RFID_STR_LEN_MAX, savedTag);
      delay(10);
//      if (uDest != 0xFF)
//      {
//        Serial.print(F("uDest: "));
//        Serial.print(uDest);
//        Serial.print(F(" != "));
//        Serial.println(F("0xFF"));
//      }
//      if (uSrc != i)
//      {
//        Serial.print(F("uSrc: "));
//        Serial.print(uSrc);
//        Serial.print(F(" != "));
//        Serial.println(i);
//      }
//      if (strncmp(savedTag, cMsg, strlen(cMsg)) != 0)
//      {
//        Serial.print(F("cMsg: "));
//        Serial.print(cMsg);
//        Serial.print(F(" != "));
//        Serial.println(savedTag);
//      }
      if (uDest == 0xFF && uSrc == i && strncmp(savedTag, cMsg, strlen(cMsg)) == 0 && strncmp("NONEXXXXXXXX", savedTag, strlen("NONEXXXXXXXX")) != 0)
      {
//        Serial.println(F("Tag match"));
        tagFails[i] = 0;
      }
      else
      {
        // Mismatch
//        Serial.println(F("Tag mismatch"));
        if (tagFails[i] < 3)
        {
          tagFails[i]++;
        }
      }
    }
    else
    {
      // Save in EEPROM
//      Serial.println(F("Setting tag"));

      if (uDest == 0xFF && uSrc == i)
      {
        EEPROMWriteString(EEPROM_START + RFID_STR_LEN_MAX * i, cMsg); // Save entry
      }
    }
    delay(50);
  }

  return(iRet);
}

void rfid::buttons()
{
  if (digitalRead(LEARN_BUTTON) == HIGH) // Learn new pattern
  {
    if (learnPress == false)
    {
      learnPress = true;
      learn();
    }
  }
  else
  {
    learnPress = false;
  }

  if (digitalRead(RESET_BUTTON) == HIGH) // Reset
  {
    if (resetPress == false)
    {
      resetPress = true;
      Serial.println(F("Reset button pressed"));
      reset();
    }
  }
  else
  {
    resetPress = false;
  }

  if (digitalRead(EMERGENCY_BUTTON) == HIGH) // Test for Emergency exit
  {
    // Emergency exit pressed
    iEmergencyExit = 1;
    Serial.println(F("Emergency exit pressed"));
    // turn off maglocks
    digitalWrite(MAG_LOCK, LOW);
  }

  if (digitalRead(REPLAY_BUTTON) == HIGH)
  {
    buttonState = 1;
  }
  else
  {
    buttonState = 0;
  }
}

byte rfid::learn()
{
  if (iCommFailure == 0)
  {
    bool iAllTagsFound = true;
    for (byte i = 0; i < iTotalScanLength; i++)
    {
      if (tagFound[i] == false)
      {
        iAllTagsFound = false;
        break;
      }
    }

    if (iAllTagsFound || learnWithMissingTag)
    {
      // Turn on purple light
      digitalWrite(LEARN_LIGHT, HIGH);
      Serial.println(F("Learning"));
      // Save tags
      RfidSetGetTagIds(0);
      delay(500);
      digitalWrite(LEARN_LIGHT, LOW);
      return 0;
    }
    else
    {
      // turn on error light
      digitalWrite(ERROR_LIGHT, HIGH);
      Serial.println(F("A tag was not found, not learning"));
      delay(500);
      digitalWrite(ERROR_LIGHT, LOW);
      return 1;
    }
  }
  else
  {
    Serial.println(F("Cannot learn due to communication error"));
    delay(500);
    return 2;
  }
}

void rfid::EEPROMReadString(byte pos, byte len, char* data)
{
  byte i;
  for (i = 0; i < len; i++)
  {
    data[i] = EEPROM.read(pos + i);
  }
  data[i] = 0;
}

void rfid::EEPROMWriteString(byte pos, char* data)
{
  for (byte i = 0; i < strlen(data); i++)
  {
    EEPROM.write(pos + i, data[i]);
  }
}

byte rfid::getLen()
{
  return iTotalScanLength;
}

void rfid::getTagStates(byte tagStates[], bool& stateFlag)
{
  for (int i = 0; i < iTotalScanLength; i++)
  {
    if (tagFound[i] == false)
    {
      tagStates[i] = 0;
    }
    else if (tagFails[i] < 3)
    {
      tagStates[i] = 1;
    }
    else
    {
      tagStates[i] = -1;
    }
    if (tagStates[i] != tagStatesOld[i])
    {
      tagStatesOld[i] = tagStates[i];
      stateFlag = true;
    }
  }
}
