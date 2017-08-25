#ifndef game__h
#define game__h

#include "arduino.h"
#include "fx300.h"
#include "network.h"

class Network;

class Game
{
  public:
    // Game states
    static const int GAMESTATE_START = 0;
    static const int GAMESTATE_RUN = 10;
    static const int GAMESTATE_SOLVED = 100;
    static const int GAMESTATE_ENDLOOP = 255;
  
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

    String gameName;

    int GAME_INPUT_RESET = 0;  // When high and enabled (defined), the game will be reset
    int GAME_INPUT_ENABLE = 0;  // When low and enabled (defined), the game will be enabled

  private:

};

#endif
