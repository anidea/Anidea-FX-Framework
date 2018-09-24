/*

Class for interacting with mqtt_m3

This application is targeted for the line of FX boards from Anidea Engineering.
While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

2017 Gabriel Goldstein
Anidea Engineering
http://www.anidea-engineering.com

written and tested with Arduino 1.8.1

-------------------
*/

#ifndef network_mqtt_m3_h
#define network_mqtt_m3_h

#include "game.h"
#include "arduino.h"
#include "network.h"

class Game;
class PubSubClient;
class mqtt_m3 : public Network
{
public:
	mqtt_m3(byte[], IPAddress, IPAddress);

	virtual void loop(void);

	virtual void tick();

private:
	void sendChanges();

	void reconnect();

	static void callback(char*, uint8_t*, unsigned int);

	void printData(char* data, char* channel);

	EthernetClient ethClient = server.available();

	PubSubClient* client = nullptr;

	int serverPort = 1883;

	static char propName[];

	bool sent;

	static const uint16_t HEARTBEAT_INTERVAL = 10000; // 10 second interval
	uint16_t heartbeat_timer = HEARTBEAT_INTERVAL;

	static const uint16_t RETRY_INTERVAL = 5000; // 5 second interval
	uint16_t retry_timer = 0;

	bool netConnected = false;
	static bool recon;
};

#endif

