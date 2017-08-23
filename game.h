#ifndef game__h
#define game__h

#include "arduino.h"
#include "fx300.h"
#include "network.h"

// Game states
#define GAMESTATE_START   0
#define GAMESTATE_RUN     10
#define GAMESTATE_SOLVED  100
#define GAMESTATE_ENDLOOP 255

class Network;

class Game
{
  public:
    Game();

    virtual void tick();  // Called periodically to run hard timing

    virtual void loop();  // Called in loop

    virtual void reset(); // Call to reset game

    virtual void forceSolved(void);    // Called when the game needs to be force solved

    void setNetwork(Network *pn) { this->_pMyNetwork = pn; };

    int   isSolved(void)  {return _puzzleSolved; }

    void solved(); //

    void (*solvedCallback)(void) = NULL;

    Network *_pMyNetwork = NULL;

    // Global Game Variables
    int _puzzleSolved = 0;
    int _gameState = 0;

  private:

};

#endif
