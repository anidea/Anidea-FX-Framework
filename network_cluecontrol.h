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

#include <Ethernet2.h>
#include "game.h"
#include "arduino.h"
#include "CCModBus.h"

static const int ACTIVATE = 1; //modbus value received to activate the puzzle
static const int DEACTIVATE = 2; //modbus value received to deactivate the puzzle
static const int ENABLE = 3; //modbus value received to enable the puzzle
static const int CC_DISABLE = 4; //modbus value received to disable the puzzle
static const int RESET_ENABLE = 5; //modbus value received to reset and enable the puzzle
static const int RESET_DISABLE = 6; //modbus value received to reset and disable the puzzle

class Game;

class cluecontrol : public Network
{
  public:
    cluecontrol(byte _MyMac[], IPAddress _MyIP, IPAddress _HostIP) : Network(_MyMac, _MyIP, _HostIP, true)
    {
      CCMod.ClueControlIP = HostIP;
    };

    virtual void loop(void);

  private:
    void listenForRequest();

    void setTriggers();

    bool _puzzleSolved_old = false;
    bool _enabled_old = false;
    
    CCModBus CCMod;

    bool INPUT_STATE_OLD[NUM_INPUTS] = {false};

    bool OUTPUT_STATE_OLD[NUM_OUTPUTS] = {false};

    bool RELAY_STATE_OLD[NUM_RELAYS] = {false};
};

#endif

