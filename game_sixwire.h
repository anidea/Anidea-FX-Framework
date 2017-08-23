#ifndef game_sixwire__h
#define game_sixwire__h

#include "arduino.h"
#include "fx300.h"
#include "network.h"
#include "game.h"

// Software configuration defines
#define DEBOUNCE_COUNT        6   // Try to keep even
#define DEBOUNCE_TIME         5

// Game Definition Variables
#define PATCH_COUNT         6

// FX300 Relays
#define SOLVED       RELAY1

class Network;

class sixwire : public Game
{
  public:
    sixwire();

    virtual void loop();

    virtual void forceSolved();

    void solved();

    virtual void reset(); // Call to reset game

  private:
    int getDebouncedInput(int);
  
    // FX300 Inputs for game
    int inputPinList[PATCH_COUNT] =  {INPUT0, INPUT1, INPUT2, INPUT3, INPUT4, INPUT5};
    
    //#define RESET /* you could reduce the PATCH_COUNT and set a pin here for a reset signal */
    
    // FX300 Outputs
    int outputPinList[PATCH_COUNT] =  {OUTPUT0, OUTPUT1, OUTPUT2, OUTPUT3, OUTPUT4, OUTPUT5};
};

#endif
