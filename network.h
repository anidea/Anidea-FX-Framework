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

    bool INPUT0_STATE = 0;
    bool INPUT1_STATE = 0;
    bool INPUT2_STATE = 0;
    bool INPUT3_STATE = 0;
    bool INPUT4_STATE = 0;
    bool INPUT5_STATE = 0;

    bool OUTPUT0_STATE = 0;
    bool OUTPUT1_STATE = 0;
    bool OUTPUT2_STATE = 0;
    bool OUTPUT3_STATE = 0;
    bool OUTPUT4_STATE = 0;
    bool OUTPUT5_STATE = 0;

    bool RELAY0_STATE = 0;
    bool RELAY1_STATE = 0;

  private:
    
};

#endif
