#ifndef game_room__h
#define game_room__h

#include "arduino.h"
#include "network.h"
#include "game.h"

class Network;

class room : public Game
{
  public:
    room();

    virtual void loop();
    virtual void reset();
};

#endif

