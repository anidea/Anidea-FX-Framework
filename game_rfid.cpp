#include "game_rfid.h"

//Uncomment to enable Serial debugging output
#define DEBUG_RFID
#include "fx200.h"

// Number of readers
const int numberOfReaders = 3;

// Our FX200 configuration
FX200Configuration config
{
	/*  Read Style:*/ RFIDReadStyle::SEQUENTIAL, // EVENODD, SEQUENTIAL, or IMMEDIATE (used with user controlled timings)
	/*   RS485 Pin:*/ RS485_ENABLE, // This pin is defined in fx300.h and fx450.h
	/*  RS485 Uart:*/ &Serial, // This is the UART we want to use

	/* This parameters are optional*/
	/*   Read Time:*/ 400, // This is how long a reader will attempt to find a tag and also our interval
	/* Expire Time:*/ 2000, // This is how long a reader will keep tag information before expiring
	/*EEPROM Start:*/ Game::EEPROM_START
};

FX200<numberOfReaders> fx200(config);

rfid::rfid() : Game()
{
	fx200.init();

	reset();

	Serial.println(F("RFID Game"));
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
	//Game::loop();

	switch (_gameState)
	{
	case GAMESTATE_START:
		_gameState = GAMESTATE_RUN;
		break;

	case GAMESTATE_RUN:
		// If hall programming detected, learn tags
		if (hallLearnCommand()) learn();

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

bool rfid::isRFIDChanged()
{
	bool ret = data_flag;
	data_flag = false;
	return ret;
}

// For MQTT Networking
String rfid::getTag(byte index)
{
	return fx200.getTag(index);
}

byte rfid::getTagCount()
{
	return numberOfReaders;
}