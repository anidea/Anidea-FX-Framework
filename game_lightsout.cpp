#include <Wire.h>

#include <MsTimer2.h>
#include <EEPROM.h>

#include "fx300.h"
#include "game_lightsout.h"

/*

  LightsOut game

  This application is targeted for the FX300, an Arduino compatible controller from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing an FX300 to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/


/*
  // pin assignments

   AEI FX300
   ------------------------
 * *INPUTS*
    4 inputs for scannning button matrix

    Enable, when low, the game is enabled

    Reset, when high, the game is reset

 * *OUTPUTS*
    Light outputs controlled via I2C expansion port

    4 inputs for driving button matrix

 * *RELAY OUTPUTS*
   SOLVED       (Connect to relay1 COM and NO or NC, depending on requirements, the SOLVED state will connect COM to NO)
   

 * ***  SOLVED is run backward to provide safe operation such that if the prop dies or isn't powered, the maglock opens


  General operation:

  STARTUP
    Turn power on

  RESET
    Called at startup, after programming and when the reset input is activated
    Turn on the solved output
    Clear any in progress game play.
    Go to game play.

  GAME PLAY

   If enabled below, RESET high will hold the game in reset and clear the game play

   If enabled below, ENABLE low will allow the game to be played

   Once enabled, a preset pattern will displayed on the light/button matrix.

   Each time a button is pressed, each light in the X and Y will be inverted

   When all the lights are off, the game is won and solved is called
   
   
*/

///////////////////////////////////
// Constructor for the game
//
lightsout::lightsout() : Game()
{
  pinMode(RELAY0, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(OUTPUT0, OUTPUT);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);
  pinMode(OUTPUT3, OUTPUT);
  pinMode(OUTPUT4, OUTPUT);
  pinMode(OUTPUT5, OUTPUT);
  pinMode(LED, OUTPUT);


  pinMode(INPUT0, INPUT);
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(INPUT3, INPUT);
  //pinMode(INPUT4, INPUT); - need for i2c
  ///pinMode(INPUT5, INPUT); - need for i2c

  // Only enable the inputs/outputs that are not going to be used by this game
  INPUT_OVERRIDE_ENABLE[0] = 0;
  INPUT_OVERRIDE_ENABLE[1] = 0;
  INPUT_OVERRIDE_ENABLE[2] = 0;
  INPUT_OVERRIDE_ENABLE[3] = 0;
  INPUT_OVERRIDE_ENABLE[4] = 1;
  INPUT_OVERRIDE_ENABLE[5] = 1;

  OUTPUT_OVERRIDE_ENABLE[0] = 0;
  OUTPUT_OVERRIDE_ENABLE[1] = 0;
  OUTPUT_OVERRIDE_ENABLE[2] = 0;
  OUTPUT_OVERRIDE_ENABLE[3] = 0;
  OUTPUT_OVERRIDE_ENABLE[4] = 1;
  OUTPUT_OVERRIDE_ENABLE[5] = 1;

  RELAY_OVERRIDE_ENABLE[0] = 1;
  RELAY_OVERRIDE_ENABLE[1] = 0;

  Serial.println(F("Lights Out Game"));
  gameName = F("lightsout");

  // Init I2C devices
  Wire.begin();


  reset();


}

///////////////////////////////////
// void loop(void)
// Called in a continious loop from the arduino loop
// This function should exit and not be held up for long periods of time
//
void lightsout::loop(void)
{
  // put your main code here, to run repeatedly:

  //Do generic loop actions
  Game::loop();

  switch (_gameState)
  {
    case GAMESTATE_START:
      // Setup state

      // run reset
      reset();

      _gameState++;             // Run show

      break;

    case GAMESTATE_START + 1:
      // Flash lights and make it look interesting

      setGrid(0);
      
      

      // Look for a key
      if (scanMatrix() != NO_INPUT_DETECTED)
      {
        // Set puzzle - in Y, X
        
        _lightMatrix[2][0] = 1;         
        _lightMatrix[3][0] = 1;         
                                        
        _lightMatrix[1][1] = 1;         
        _lightMatrix[3][1] = 1;         
                                        
        _lightMatrix[1][2] = 1;         
        _lightMatrix[3][2] = 1;         
                                        
        _lightMatrix[0][3] = 1;         
        _lightMatrix[1][3] = 1;         

        // Invert matrix from game logic
        {
           int8_t *pLight = (int8_t*)_lightMatrix;
          
          for (int i = 0; i < GAME_LIGHT_OUTPUT; i++) 
          {
            pLight[i] ^= 1;  
          }
        }

        // Run
        _gameState = GAMESTATE_RUN;             // Run puzzle

        while (scanMatrix() != NO_INPUT_DETECTED) {};    // Wait for button release
        
      }else{
        
        int8_t *pLight = (int8_t*)_lightMatrix;
      
        for (int i = 0; i < 5; i++) // Set 5 lights on randomly
        {
          pLight[random(16)] = 1;  
        }
      
      }

      // Set lights
      setLights(0, 0);

      delay(250);

      break;


    case GAMESTATE_RUN:
  
      // Scan button
      _newKey = scanMatrix();

      _scanXkey = _newKey % GAME_KEYSCAN_X_COUNT;
      _scanYkey = _newKey / GAME_KEYSCAN_Y_COUNT;
   
  
      // If we have a button, toggle it and toggle neighbors
      if (_newKey != NO_INPUT_DETECTED)
      {
        Serial.print(F("Key pressed "));
        Serial.print(_scanXkey);
        Serial.print(", ");
        Serial.print(_scanYkey);
        Serial.println();

        

        
        toggleLights(_scanXkey, _scanYkey);

        dumpMatrix();


        // Update leds
        setLights(0, 0);
    
        
        // Check for win (all turned off)
        if (checkForWin() == true)
        {
          _gameState = GAMESTATE_SOLVED;  // Win
        }else{
          _gameState++;  // Play again
        }

      }else{
        //Serial.println("No key detected");
      }

      break;
      
    case GAMESTATE_RUN + 1:
      // Wait for no button to be pressed

      if (scanMatrix() == NO_INPUT_DETECTED)
      {
        _gameState = GAMESTATE_RUN;
      } 
      
      break;

    case GAMESTATE_SOLVED:
      // Run solved and any other one time routine
      solved();

      break;
 
    default:
    case GAMESTATE_ENDLOOP:
      // Wait for reset

      break;
      
  }
  
}

///////////////////////////////////
// void reset(void)
// Called to reset the game, called internally and externally
//
void lightsout::reset(void)
{
  // Routine run to reset the puzzle.  Run at start or by other means
  
  Serial.println(F("lightsout Reset"));
  
  //Reset global game variables
  Game::reset();

  // Set outputs
  digitalWrite(SOLVED, HIGH);    // Turn on maglock

  // Turn all the lights off, clear board
  setGrid(0);
  
  setLights(0, 0);  // Set lights to state
}

///////////////////////////////////
// int scanMatrix(void)
// Called to scan the button matrix, find the first key
//
int lightsout::scanMatrix(void)
{
  int xScan, yScan;
  int scanKey = NO_INPUT_DETECTED;

  for (yScan = 0; yScan < GAME_KEYSCAN_Y_COUNT; yScan++)
  {

    for (int i = 0; i < GAME_KEYSCAN_Y_COUNT; i++)
    {
      if (i == yScan)
      {
        digitalWrite(_outputKeyScanPinList[i], HIGH);  // Scan this row      
      }else{  
        digitalWrite(_outputKeyScanPinList[i], LOW);  // Don't scan this row
      }
    }

    delay(10);

       
    for (xScan = 0; xScan < GAME_KEYSCAN_X_COUNT; xScan++)
    {
        #if 0
        Serial.print(F("xScan "));
        Serial.print(xScan);
        Serial.print(F(", yScan"));
        Serial.println(yScan);
        #endif

      
      if (digitalRead(_inputKeyScanPinList[xScan]) == LOW)
      {
        scanKey = yScan + GAME_KEYSCAN_X_COUNT * xScan;

        //Serial.print(scanKey);
        //Serial.println(" HIT");

        break;  // We have a key, only one key
      }
      
    }

    
  }

  return(scanKey);

}


void lightsout::toggleLights(int x, int y)
{
  // Own light
  _lightMatrix[x][y] ^= 1;

  // light to the left
  if (x > 0)
  {
    _lightMatrix[x - 1][y] ^= 1;
  }

  // light to the right
  if (x < GAME_KEYSCAN_X_COUNT - 1)
  {
    _lightMatrix[x + 1][y] ^= 1;
  }
  
  // light up 
  if (y > 0)
  {
    _lightMatrix[x][y - 1] ^= 1;
  }

  // light below
  if (y < GAME_KEYSCAN_Y_COUNT - 1)
  {
    _lightMatrix[x][y + 1] ^= 1;
  }

}
  



void lightsout::setLights(int dontUseMatrix, int overrideState)
{
  // Write the array out the i2c light path

  int i, j;

  int light = 0;

  uint8_t lightOutput[GAME_LIGHT_OUTPUT / BITS_IN_A_BYTE] = {0};  

  if (dontUseMatrix)
  {
     for (j = 0; j < GAME_LIGHT_OUTPUT / BITS_IN_A_BYTE; j++)
     {
        lightOutput[j] = (overrideState) ? 0xFF : 0;
     }
  }else{
  
    for (j = 0; j < GAME_LIGHT_OUTPUT / BITS_IN_A_BYTE; j++)
    {
      // 8 Lights
      for (i = 0; i < BITS_IN_A_BYTE; i++)
      {
        int lightX = light % GAME_KEYSCAN_X_COUNT;
        int lightY = light / GAME_KEYSCAN_Y_COUNT;

        lightOutput[j] >>= 1;

        if (_lightMatrix[lightX][lightY] > 0)
        {
          lightOutput[j] |= 0x80;
          
          //Serial.print("Light on ");
          //Serial.println(i + j * 8);
        }
    
        light++;
      }
    }
  }

  // Write data out
  for (j = 0; j < GAME_LIGHT_OUTPUT / BITS_IN_A_BYTE; j++)
  {
    #if 0
    // I2c function here
    Serial.print(F("I2C Addr "));
    Serial.print(i2cLightAddresses[j]);
    Serial.print(F(", data "));
    Serial.println(lightOutput[j], HEX);
    

    
    Wire.beginTransmission(i2cLightAddresses[j]);
    Wire.write(~lightOutput[j]);  // Invert
    int ret = Wire.endTransmission();
    
  
  /*    for (i = 0; i < 127; i++)
    {
  
      Wire.beginTransmission(i);
      Wire.write(lightOutput[j]);
      int ret = Wire.endTransmission();
  */
    
    if (ret != 0)
    {
      Serial.print(F("Addr "));
      Serial.print(i);
      Serial.print(F(", I2C ret "));
      Serial.println(ret);
    }
    #endif
  }

}

int lightsout::checkForWin(void)
{
  int xScan, yScan;
  int win = true;

  for (yScan = 0; yScan < GAME_KEYSCAN_Y_COUNT; yScan++)
  {
    for (xScan = 0; xScan < GAME_KEYSCAN_X_COUNT; xScan++)
    {
      if (_lightMatrix[xScan][yScan] == 1)
      {
        win = false;
        return win;
      }
    }

    
  }

  return win;
  
}


void lightsout::setGrid(int8_t setGridState)
{
  int xScan, yScan;

  for (yScan = 0; yScan < GAME_KEYSCAN_Y_COUNT; yScan++)
  {
    for (xScan = 0; xScan < GAME_KEYSCAN_X_COUNT; xScan++)
    {
     _lightMatrix[xScan][yScan] = setGridState;
    }
  }


}
#if 0
int lightsout::scanInputsSteady(void)
{
  /*
  Scan all inputs looking for a steady state for a period of time. While this isn't
  important for buttons, when you have many hall sensors that may get triggered as
  magnets pass over them, we need to do some heavy filtering and look for a steady
  input for a period of time
  
  
  One caveat, if you have a really crappy input or the users don't settle on an input, the routine will stay here indefinitely

  TODO:  Add a max out timer
  
  */

  if (GAME_INPUT_COUNT > 8)
  {
    Serial.println(F("error Too many inputs for the number of bits"));
  }
  
  // Increase size types if ever on a machine with more inputs
  uint8_t allInputs, allPreviousInputs;
  uint8_t activeInputCount;
  int activeInput;
  
  uint8_t steadyStateCounter = STEADY_STATE_INPUTS_COUNT;
  
  
  while(1)
  {
    allInputs = 0;    // Clear all bits
    activeInputCount = 0;
    activeInput = NO_INPUT_DETECTED;
    
    // Load all inputs ports into a single byte for easier comparison
    for (int i = 0; i < GAME_INPUT_COUNT; i++)
    {
      if (digitalRead(_inputButtonPinList[i]) == 1) // Bit active
      {
        allInputs |= 1;     // Set bit
        
        allInputs <<= 1;    // Shift up

        activeInput = i;   // Save the current button, this is only used if a single input 

        activeInputCount++;
      }
    }

    // Test for steady, single input
    if ((allInputs != allPreviousInputs) || (activeInputCount > 1))
    {
      // Reset counter is more than one input is active or they are changing
      steadyStateCounter = STEADY_STATE_INPUTS_COUNT; 

      /*
      Serial.print("Counter reset: inputs ");
      Serial.print(allInputs, HEX);
      Serial.print(", Count ");
      Serial.print(activeInputCount, DEC);
      Serial.println();
      */
    }


    // Check to see if we are done
    if (steadyStateCounter == 0)
    {
      // We have been steady with a single input for some time now

      /*
      Serial.print("Active input returned ");
      Serial.print(activeInput, DEC);
      Serial.println();
      */
      return (activeInput);

    }

    // Store last scane
    allPreviousInputs = allInputs;

    steadyStateCounter--;   // Wait for steady state


    delay(STEADY_STATE_INPUTS_TIME);  // Wait a scan time

    
  }
  
}
#endif

void lightsout::allLightsOnOff(int state)
{
  setLights(1, state);
}

void lightsout::dumpMatrix(void)
{
  int xScan, yScan;
  
    for (yScan = 0; yScan < GAME_KEYSCAN_Y_COUNT; yScan++)
    {

      Serial.print(yScan);
      Serial.print(": ");
      
    for (xScan = 0; xScan < GAME_KEYSCAN_X_COUNT; xScan++)
    {
      Serial.print(_lightMatrix[xScan][yScan]);
      Serial.print(", ");
    }

    Serial.println();
  }
}

int lightsout::getDebouncedInput(int input)
{
  int inputCount = 0;
  
  for (int i = 0; i < DEBOUNCE_COUNT; i++)
  {
    if (digitalRead(input) == 1)
    {
      inputCount++;
    }else{
      inputCount--;
    }

    delay(DEBOUNCE_TIME);
  }

  if (inputCount >= DEBOUNCE_COUNT / 2)
  {
    return HIGH;
  }

  return LOW;
}
