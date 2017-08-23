/*
 * A prop with a button that sends a signal to ERM & can also trigger a relay
 */

#ifndef game_senet__h
#define game_senet__h

#include "arduino.h"
#include "fx300.h"
#include "network.h"
#include "game.h"

#define RELAY_PIN RELAY0

#define REED_PIN_1 INPUT0
#define REED_PIN_2 INPUT1
#define REED_PIN_3 INPUT2
#define REED_PIN_4 INPUT3
#define REED_PIN_5 INPUT4
#define REED_PIN_6 INPUT5

class Network;

class senetgame : public Game
{
  public:
    senetgame();

    virtual void loop();

    virtual void reset(); // Call to reset game

    void solved();

  private:
    bool relayTriggered = false;  // has the relay been opened?
};

#endif
