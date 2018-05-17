#ifndef game_rfid__h
#define game_rfid__h

#include <Arduino.h>
#include "network.h"
#include "game.h"

class Network;

class rfid : public Game
{
public:
	rfid();

	virtual bool learn();
	
	virtual bool isRFIDChanged();
	virtual String getTag(byte index);

	virtual byte getTagCount();

private:
	virtual void loop();

	bool data_flag = false;
};

#endif
