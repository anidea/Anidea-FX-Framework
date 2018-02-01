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

#include <Ethernet2.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "game.h"
#include "arduino.h"

class Game;

class mqtt : public Network
{
  public:
    mqtt(byte[], IPAddress, IPAddress);

    virtual void loop(void);

  private:
    void sendChanges();

    void reconnect();

    static void callback(char*, byte*, unsigned int);

    EthernetClient ethClient = server.available();
    
    PubSubClient client;

    int serverPort = 1883;

    char propName[16];

    bool sent;

    char serverName[16];

    bool INPUT_STATE_OLD[NUM_INPUTS] = {false};

    bool OUTPUT_STATE_OLD[NUM_INPUTS] = {false};

    bool RELAY_STATE_OLD[NUM_INPUTS] = {false};
};

#endif
