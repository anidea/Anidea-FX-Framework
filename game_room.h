#ifndef game_room__h
#define game_room__h

#include "arduino.h"
#include "network.h"
#include "game.h"

// Select which version of the room controller you have
#define ONEBUTTON
//#define TWOBUTTON

class Network;

class room : public Game
{
  public:
    room();

    virtual void loop();

    void solved();

    virtual void reset(); // Call to reset game
};

#endif
