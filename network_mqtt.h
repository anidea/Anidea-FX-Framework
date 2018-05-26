/*
  
  Class for interacting with mqtt
  
  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef network_mqtt_h
#define network_mqtt_h

#include "game.h"
#include "arduino.h"
#include "game_rfid.h"
#include "network.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Options
//#define AUTO_FIND_MQTT_SERVER // Enable locating MQTT server using mDNS
//#define DEBUG_MQTT // Show debug information through serial

#ifdef AUTO_FIND_MQTT_SERVER
#include <EthernetUdp2.h>
#include <ArduinoMDNS.h>
#endif

class Game;
class PubSubClient;
class mqtt : public Network
{
public:
	mqtt(byte[], IPAddress, IPAddress);

	virtual void loop(void);

	virtual void tick();

private:
	void sendChanges();

	void reconnect();

	static void callback(char*, uint8_t*, unsigned int);


	#ifdef AUTO_FIND_MQTT_SERVER
	static void serviceFound(const char* type, MDNSServiceProtocol /*proto*/, const char* name, IPAddress ip, unsigned short port, const char* txtContent);
	#endif

	void printData(char* data, const char* channel);

	EthernetClient ethClient = server.available();

	PubSubClient* client = nullptr;

	int serverPort = 1883;

	bool sent;

	static const uint16_t HEARTBEAT_INTERVAL = 10000; // 10 second interval
	uint16_t heartbeat_timer = HEARTBEAT_INTERVAL;

	static const uint16_t RETRY_INTERVAL = 5000; // 5 second interval
	uint16_t retry_timer = 0;

	bool connected = false;
};

#endif