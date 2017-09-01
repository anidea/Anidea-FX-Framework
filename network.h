/*

  Generic class for all types of networks

  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef network_h
#define network_h

#include <Ethernet2.h>

#include "game.h"
#include "arduino.h"


class Game;

class Network
{
  public:
    Network();
  
    Network(byte [], IPAddress, IPAddress);

    void sendGameSolved(void);

    void receiveGameReset(void);

    virtual void loop(void);

    virtual void setGame(Game *pg) { pMyGame = pg; };

    byte *MyMac;
    
    IPAddress MyIP;
    IPAddress HostIP;

    Game *pMyGame = NULL;

    EthernetServer server;

    bool INPUT_STATES[6] = {false};

    bool OUTPUT_STATES[6] = {false};

    bool RELAY_STATES[2] = {false};

  private:
    
};

#endif
