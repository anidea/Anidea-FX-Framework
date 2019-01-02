#ifndef game_rfid__h
#define game_rfid__h

#include <Arduino.h>
#include "network.h"
#include "game.h"
#include "fx200.h"

class Network;

class rfid : public Game
{
public:
	rfid();

	virtual bool learn();
	
	virtual bool isRFIDChanged();
	virtual String getTag(byte index);

	virtual byte getTagCount();

	virtual void reset();

	void saveTags();

	void loadTags();

private:
	virtual void loop();

	virtual void solved() override;

	bool data_flag = false;

	const static int numberOfReaders = 4;

	static FX200Configuration config;
	static FX200<numberOfReaders> fx200;
};

#endif

