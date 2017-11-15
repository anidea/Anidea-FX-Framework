/*
  
  Class for interacting with Houdini MC management software
  
  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef houdinimc_h
#define houdinimc_h

#include <Ethernet2.h>
#include "game.h"
#include "arduino.h"

class Game;

class houdinimc : public Network
{
  public:
    houdinimc(byte MyMac[], IPAddress MyIP, IPAddress HostIP) : Network(MyMac, MyIP, HostIP, true) {};

    virtual void loop(void);

  private:
    void processRequest(EthernetClient& client, String requestStr);
    
    void listenForEthernetClients();

    void sendChanges();

    void writeClientResponse(EthernetClient& client, String bodyStr);
    
    void writeClientResponseNotFound(EthernetClient& client);

    byte getPage(IPAddress ipBuf, int thisPort, char *page);

    bool sent;

    char pageAdd[64];

    int serverPort = 14999;

    EthernetClient client = server.available();

    char serverName[16];

    bool INPUT_STATE_OLD[NUM_INPUTS] = {false};

    bool OUTPUT_STATE_OLD[NUM_INPUTS] = {false};

    bool RELAY_STATE_OLD[NUM_INPUTS] = {false};
};

#endif
