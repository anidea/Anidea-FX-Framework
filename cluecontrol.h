/*
  
  Class for interacting with ClueControl management software

  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef cluecontrol_h
#define cluecontrol_h

#include <SPI.h>
#include <Ethernet2.h>
#include "game.h"
#include "arduino.h"

#define CCCoil      1 //Which coil to use when talking to ClueControl
#define ACTIVATE    1   //modbus value received to activate the puzzle
#define DEACTIVATE  2   //modbus value received to deactivate the puzzle

class Game;

class cluecontrol : public Network
{
  public:
    cluecontrol(byte MyMac[], IPAddress MyIP, IPAddress gateway, IPAddress subnet, IPAddress HostIP, int CCRegister) : Network(MyMac, MyIP, gateway, subnet, HostIP)
    {
      CCMod.ClueControlIP = HostIP;
      Register = CCRegister;
    };
  
    void sendGameSolved(void);

    void sendGameReset(void);

    virtual void loop(void);

  private:
    void listenForRequest();

    bool sent;
    
    CCModBus CCMod;

    int Register = 0;
};

#endif
