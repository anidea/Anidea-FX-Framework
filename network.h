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

// Options
#define ENABLE_DHCP // Enables obtaining an IP address with DHCP
#define ENABLE_CONTROL_PANEL // Enables interacting with control panel software
//#define GENERATE_NAME

#ifdef ENABLE_CONTROL_PANEL
#include <EthernetUdp2.h>
#endif

class Game;

class Network
{
  public:
    Network(byte [], IPAddress, IPAddress, bool);

    virtual void loop(void);

    virtual void setGame(Game *pg) { pMyGame = pg; };

    byte *MyMac;
    
    IPAddress MyIP;
    IPAddress HostIP;

    static Game *pMyGame;

    EthernetServer server;

  private:
    void getIP();

	#ifdef ENABLE_DHCP
	void getIP_DHCP();
	#endif

	#ifdef ENABLE_CONTROL_PANEL
	EthernetUDP Udp;

	int UDPport = 8888;

	void receiveUDP(void);
	#endif
};

#endif
