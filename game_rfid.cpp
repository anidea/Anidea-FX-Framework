#include "game_rfid.h"
#include <wiring_private.h>

//#define DEBUG_GAME_RFID

rfid::rfid() : Game()
{
	// Things that need to be set up when the game is first created
	pinMode(RS485_ENABLE, OUTPUT);
	digitalWrite(RS485_ENABLE, LOW);

	// Setup RFID serial port
	#ifdef FX450
		mySerialRfid = &Serial5;
	#elif defined(FX350)
		mySerialRfid = &Serial;
	#endif

	mySerialRfid->begin(115200);

	Serial.print(F("!!!Anidea FX - "));
	Serial.print(RFID_TAG_COUNT);
	Serial.println(F(" port RFID Scanner!!!"));

	reset();
}

void rfid::reset(void)
{
	Game::reset();

	char savedData[RFID_STR_LEN_MAX];
	EEPROMReadString(EEPROM_START + RFID_STR_LEN_MAX * RFID_TAG_COUNT, RFID_STR_LEN_MAX, savedData);

	bool dataStored = !strcmp(savedData, SAVEDDATA);

	for (size_t i = 0; i < RFID_TAG_COUNT; i++)
	{
		Serial.println(i);
		strcpy(tag_data[i], NONE);

		if (dataStored)
		{
			EEPROMReadString(EEPROM_START + RFID_STR_LEN_MAX * i, RFID_STR_LEN_MAX, savedData);
			strcpy(solved_data[i], savedData);
			Serial.println(savedData);
		}
		else
		{
			strcpy(solved_data[i], NODATA);
			Serial.println(NODATA);
		}
	}

	if (dataStored) Serial.println("Read from EEPROM");
	else Serial.println("No saved data");
}

void rfid::tick()
{
	Game::tick();
}

byte rfid::learn()
{
	bool full = true;
	for (size_t i = 0; i < RFID_TAG_COUNT; i++)
	{
		if (!strcmp(tag_data[i], NONE)) full = false;
	}

	if (!full)
	{
		Serial.println("Learning failed!");
		return 0;
	}

	for (size_t i = 0; i < RFID_TAG_COUNT; i++)
	{
		EEPROMWriteString(EEPROM_START + RFID_STR_LEN_MAX * i, tag_data[i]); // Save entry
		strcpy(solved_data[i], tag_data[i]);
	}

	EEPROMWriteString(EEPROM_START + RFID_STR_LEN_MAX * RFID_TAG_COUNT, SAVEDDATA);

	Serial.println("Learning successful!");
}

void rfid::solved()
{
	Game::solved();
}

void rfid::loop()
{
	Game::loop();

	switch (_gameState)
	{
	case GAMESTATE_START:
		_gameState = GAMESTATE_RUN;
		break;

	case GAMESTATE_RUN:
		if (analogRead(HALL) < HALL_NORTH_THRESH)
		{
			learn();
			while (analogRead(HALL) < HALL_NORTH_THRESH) {}
		}

		if (freeRunningTimer >= RFID_INTERVAL)
		{
			scanRFID();

			freeRunningTimer = 0;
		}

		for (size_t i = 0; i < RFID_TAG_COUNT; i++)
		{
			if (strcmp(tag_data[i], solved_data[i])) return; // If mismatch, exit
		}
		
		_gameState = GAMESTATE_SOLVED;

		break;
	case GAMESTATE_SOLVED:
		solved();
		_gameState = GAMESTATE_ENDLOOP;
		break;

	case GAMESTATE_ENDLOOP:

		break;

	}
}

void rfid::scanRFID()
{
	char cBufOut[32], cBufIn[32];

	static bool SCAN = true;

	if (SCAN)
	{
		for (int i = 0; i < RFID_TAG_COUNT; i++)
		{
#ifdef USE_GROUPS
			sprintf(cBufOut, "###255,%d,TRG%04d###", i, timings[i]); //scan command
#else
			sprintf(cBufOut, "###255,%d,TRG%04d###", i, RFID_INTERVAL*i); //scan command
#endif

			RS485_SendMessage(cBufOut, nullptr, nullptr);
		}

		SCAN = false;

		return;
	}
	else
	{
#ifdef USE_GROUPS
		static uint8_t groupIndex = 0;
		int8_t groups[][RFID_INNER_COUNT] = RFID_GROUPS;
		for (byte j = 0; j < RFID_INNER_COUNT; j++)
		{
			int8_t i = groups[groupIndex][j];
			if (i == -1) continue;
#else
		static byte i = 0;
		{
#endif
#ifdef DEBUG_GAME_RFID
			Serial.print("Checking RFID ");
			Serial.print(i);
			Serial.print(" in group ");
			Serial.println(groupIndex);
#endif

			sprintf(cBufOut, "###255,%d,REQRFID###", i); //request rfid data

			uReceiveLen = sizeof(cBufIn);

			memset(cBufIn, 0, 32);

			RS485_SendMessage(cBufOut, cBufIn, &uReceiveLen);

			int uSrc = RFID_TAG_COUNT + 1, uDest = 0;
			char cMsg[RFID_STR_LEN_MAX];
			memset(cMsg, 0, RFID_STR_LEN_MAX);
			sscanf(cBufIn, "%d,%d,%s", &uSrc, &uDest, cMsg);

			if (!uReceiveLen)
			{
				Serial.print("Skipping: ");
				Serial.println(i);

#ifdef USE_GROUPS
				continue; //If no read continue
#else
				return;
#endif
			}

			if (!strcmp("NONEXXXXXXXX", cMsg) || cMsg[0] == 0) strcpy(cMsg, NONE); //If empty string or NONEXXXXXXX make none

			if (strcmp(cMsg, tag_data[i]))
			{
				strcpy(tag_data[i], cMsg);		// Write new ID
				data_flag = true;			// Change state
			}
		}

#ifdef USE_GROUPS
		if (++groupIndex == RFID_OUTER_COUNT)
		{
			groupIndex = 0;
			SCAN = true;
		}
#else
		if (++i == RFID_TAG_COUNT)
		{
			i = 0;
			SCAN = true;
		}
#endif
		}
}

bool rfid::isRFIDChanged()
{
	bool ret = data_flag;
	data_flag = false;
	return ret;
}

void rfid::getTagData(char * data, size_t size)
{
	snprintf(data, size, "{\"DIRECTION\":\"FROM\", \"TYPE\":\"RFID\", \"VALUE\":[");
	for (int i = 0; i < RFID_TAG_COUNT; i++)
	{
		snprintf(data + strlen(data), size, "\"%s\"", tag_data[i]);
		if (i != RFID_TAG_COUNT - 1) snprintf(data + strlen(data), size, ", ");
	}
	snprintf(data + strlen(data), size, "] }");
}

void rfid::RS485_SendMessage(char *pMessage, char *pResponse, uint32_t *puLength)
{
	delay(10);
	byte pos = 0;
	digitalWrite(RS485_ENABLE, HIGH);

	mySerialRfid->write(pMessage);
	mySerialRfid->flush();
	delay(2);

	digitalWrite(RS485_ENABLE, LOW);
	delay(10);

	if (!pResponse) return;
	// Spin while waiting for time out, and not signals
	byte timeout = 0;
	while (!mySerialRfid->available() && timeout < RFID_TIMEOUT)
	{
		delay(1);
		timeout++;
	}

	delay(10);

	while (mySerialRfid->available())
	{
		pResponse[pos] = (char)mySerialRfid->read();
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

			#ifdef DEBUG_GAME_RFID
				Serial.print(F("Write "));
				Serial.print(pMessage);
				Serial.println(F(", Read NOTHING"));
			#endif

			return;
		}

		// Find the last and start the message reading there.
		do
		{
			pStartMsgPos++;
			uReadOffset++;
		} while (*pStartMsgPos == '#');

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
	} while (1);


	// Do we have a full message
	if (pTermMsgPos)
	{
		// We have a full message, is it for us

		// Terminate our message
		*pTermMsgPos = 0;

		strcpy(pResponse, pStartMsgPos);
		*puLength = strlen(pStartMsgPos);
	}
	#ifdef DEBUG_GAME_RFID
		Serial.print(F("Write "));
		Serial.print(pMessage);
		Serial.print(F(", Read "));
		Serial.println(pResponse);
	#endif
}