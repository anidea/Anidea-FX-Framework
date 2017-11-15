#ifndef game_room__h
#define game_room__h

#include "arduino.h"
#include "fx300.h"
#include "network.h"
#include "game.h"

class Network;

class room : public Game
{
  public:
    room();

    virtual void loop();

    void solved();

    virtual void reset(); // Call to reset game

  private:
    bool solvedFlag;

    unsigned long button1Timer = 0;
    bool button1Flag = false;

    unsigned long button2Timer = 0;
    bool button2Flag = false;
};

#endif
