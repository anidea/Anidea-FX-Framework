#ifndef game__h
#define game__h

#include "arduino.h"
#include "network.h"

// Boards
#ifdef ARDUINO_AEIFX_300
#include "fx300.h"
#endif

class Network;

class Game
{
  public:
    // Game states
    static const int GAMESTATE_START = 0;
    static const int GAMESTATE_RUN = 10;
    static const int GAMESTATE_SOLVED = 100;
    static const int GAMESTATE_ENDLOOP = 255;
    static const int EEPROM_START = 10; // Start all EEPROM storage from here because anything before is used by the framework
  
    Game();

    virtual void tick();  // Called periodically to run hard timing

    virtual void loop();  // Called in loop

    virtual void reset(); // Call to reset game

    virtual void solved(); // Called when the game is solved

    virtual void forceSolved(); // Called when the game needs to be force solved

    virtual void enable(); // Call to enable game

    virtual void disable(); // Call to disable game

    void setNetwork(Network *pn) { this->_pMyNetwork = pn; };

    bool   isSolved(void)  {return _puzzleSolved; }

    bool   isEnabled(void)  {return _enabled; }

    Network *_pMyNetwork = NULL;

    // Global Game Variables
    bool _puzzleSolved = 0;
    byte _gameState = 0;
    bool _enabled = 0;
    byte buttonState = 0;
    
    // Input override enables
    bool INPUT_OVERRIDE_ENABLE[NUM_INPUTS] = {};

    // Output override enables
    bool OUTPUT_OVERRIDE_ENABLE[NUM_OUTPUTS] = {};

    // Relay override enables
    bool RELAY_OVERRIDE_ENABLE[NUM_RELAYS] = {};

    static bool INPUT_STATES[NUM_INPUTS];

    static bool OUTPUT_STATES[NUM_OUTPUTS];
    static bool OUTPUT_STATES_FLAG[NUM_OUTPUTS];

    static bool RELAY_STATES[NUM_RELAYS];
    static bool RELAY_STATES_FLAG[NUM_OUTPUTS];

  private:

};

#endif
