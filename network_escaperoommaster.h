/*
  
  Class for interacting with Escape Room Master management software
  
  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef escaperoommaster_h
#define escaperoommaster_h

#include <Ethernet2.h>
#include <ArduinoJson.h>
#include "game.h"
#include "arduino.h"

class Game;

class escaperoommaster : public Network
{
  public:
    escaperoommaster(byte MyMac[], IPAddress MyIP, IPAddress HostIP) : Network(MyMac, MyIP, HostIP, true) {};

    virtual void loop(void);

  private:
    void processRequest(EthernetClient& client, String requestStr);
    
    void listenForEthernetClients();

    void writeClientResponse(EthernetClient& client, String bodyStr);
    
    void writeClientResponseNotFound(EthernetClient& client);

    void createJSON(EthernetClient& client);
};

#endif
