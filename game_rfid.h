#ifndef game_rfid__h
#define game_rfid__h

#include <Arduino.h>
#include "network.h"
#include "game.h"

#define RFID_TAG_COUNT 4		// Number of FX200s
#define RFID_INTERVAL 300		// Scanning interval in ms
#define RFID_TIMEOUT 50			// Timeout time in ms
#define RFID_STR_LEN_MAX 20		// Max length of RFID tag ID strings

#define NONE "NONE"
#define NODATA "NODATA"
#define SAVEDDATA "SAVEDDATA"

//#define USE_GROUPS
#ifdef USE_GROUPS
#define RFID_GROUPS { {0}, {1}, {2}, {3} }// Make sure each group is padded with -1 if less than max
#define RFID_INNER_COUNT 1 // Number of elements in inner braces
#define RFID_OUTER_COUNT 4 // Number of arrays in outer braces
#define RFID_TIMING {RFID_INTERVAL*0, RFID_INTERVAL*1, RFID_INTERVAL*2 ,RFID_INTERVAL*3}
#endif

class Network;

class rfid : public Game
{
public:
	rfid();

	virtual void reset(); // Call to reset game
	virtual void tick();
	virtual byte learn();
	virtual void solved();
	
	virtual bool isRFIDChanged();
	virtual void getTagData(char * data, size_t size);

private:
	uint32_t uReceiveLen;

	void RS485_SendMessage(char *pMessage, char *pResponse, uint32_t *puLength);

	virtual void loop();

	void scanRFID();

	uint16_t readDelay = 0;

	char tag_data[RFID_TAG_COUNT][RFID_STR_LEN_MAX + 1]{}; //Currently read tag IDs
	char solved_data[RFID_TAG_COUNT][RFID_STR_LEN_MAX + 1]{}; //Stored ids

#ifdef USE_GROUPS
	int timings[4] = RFID_TIMING;
#endif

	bool data_flag = false;

	HardwareSerial* mySerialRfid;
};

#endif
