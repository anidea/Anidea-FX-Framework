/*
  
  Empty network
  
  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef network_empty_h
#define network_empty_h

#include "game.h"
#include "arduino.h"

class Game;

class network_empty : public Network
{
	byte MyMac[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	IPAddress MyIP;
	IPAddress HostIP;

  public:
    network_empty() : Network(MyMac, MyIP, HostIP, false) {};

  private:
    
};

#endif

