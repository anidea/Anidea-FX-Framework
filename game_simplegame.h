#ifndef game_simplegame__h
#define game_simplegame__h

#include "arduino.h"
#include "fx300.h"
#include "network.h"
#include "game.h"

class Network;

class simplegame : public Game
{
  public:
    simplegame();

    virtual void loop();

    virtual void forceSolved();

    void solved();

    virtual void reset(); // Call to reset game

  private:
    int presscounter = 0;
    
    bool pressflag = 0;

    static const int buttonPin = INPUT5;
    static const int lightPin = OUTPUT4;
};

#endif
