#include "game_rfid.h"

FX200Configuration rfid::config
{
	/*  Read Style:*/ RFIDReadStyle::EVENODD, // EVENODD, SEQUENTIAL, or IMMEDIATE (used with user controlled timings)
	/*   RS485 Pin:*/ RS485_ENABLE, // This pin is defined in fx300.h and fx450.h
	/*  RS485 Uart:*/ RS485_SERIAL, // This UART is defined in fx300.h and fx450.h

	/* This parameters are optional*/
	/*   Read Time:*/ 400, // This is how long a reader will attempt to find a tag and also our interval
	/* Expire Time:*/ 2000, // This is how long a reader will keep tag information before expiring
};

FX200<rfid::numberOfReaders> rfid::fx200{ config };

rfid::rfid() : Game()
{
	// Delay on startup if using an FX300
	#if defined(FX350) || defined(FX300)
		delay(3000);
	#endif

	fx200.init();

	reset();

	Serial.println(F("RFID Game"));

	loadTags();
}

bool rfid::learn()
{
	Serial.println("Learning...");

	// Wait for magnet to save data
	while (!hallLearnCommand()) { fx200.scan(); }

	// See if learning was successful or not
	bool learned = fx200.learn();
	Serial.println(learned ? "Learning successful!" : "Learning failed! Tag missing!");

	return learned;
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
		// If hall programming detected, learn tags
		if (hallLearnCommand() && learn()) saveTags();

		// Scan tags for new data
		fx200.scan();

		// If new data set flag so the new data can be broadcasted later by the network
		if (fx200.hasNewData()) data_flag = true;

		// If read tags match the solve tags, puzzle is solved
		if (fx200.isSolved()) _gameState = GAMESTATE_SOLVED;

		break;
	case GAMESTATE_SOLVED:
		solved();
		_gameState = GAMESTATE_ENDLOOP;
		break;

	case GAMESTATE_ENDLOOP:

		break;

	}
}

void rfid::solved()
{
	Game::solved();

	digitalWrite(RELAY0, LOW);
	digitalWrite(OUTPUT3, HIGH);
}


bool rfid::isRFIDChanged()
{
	bool ret = data_flag;
	data_flag = false;
	return ret;
}

String rfid::getTag(byte index)
{
	return fx200.getTag(index);
}

byte rfid::getTagCount()
{
	return numberOfReaders;
}

void rfid::reset()
{
	Game::reset();

	uint32_t t = millis();

	while (millis() - t < 2000)
	{
		fx200.scan(); // purge tags
	}
}

void rfid::saveTags()
{
	EEPROM.write(EEPROM_START, numberOfReaders);
	for (int i = 0; i < numberOfReaders; i++)
	{
		String tag = fx200.getTag(i);
		EEPROMWriteString(EEPROM_START + 1 + i * 32, (char*) tag.c_str());
	}
}

void rfid::loadTags()
{
	byte count = EEPROM.read(EEPROM_START);
	if (count != numberOfReaders) return;

	char storedTag[32];
	for (int i = 0; i < numberOfReaders; i++)
	{
		EEPROMReadString(EEPROM_START + 1 + i * 32, 32, storedTag);
		fx200.setSolvedTag(i, storedTag);
	}
}

