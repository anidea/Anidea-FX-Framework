#ifndef game_inputsequence__h
#define game_inputsequence__h

#include "arduino.h"
#include "network.h"
#include "game.h"

class Network;

class inputsequence : public Game
{
  public:
    inputsequence();

    virtual void loop();

    void solved();

    virtual void reset(); // Call to reset game

  private:

    // Configure the steady state input filter
    // Inputs must be steady for COUNT * TIME before being accepted
    static const int STEADY_STATE_INPUTS_COUNT = 25;
    static const int STEADY_STATE_INPUTS_TIME = 10;  // 25 * 10 = 1/4 second
    static const int NO_INPUT_DETECTED = -1;

    // Game Definition Variables
    static const int GAME_INPUT_COUNT = 6;                   // Number of inputs/buttons/hall sensors in the sequence
    static const int GAME_MAX_SOLVE_TIME = 150;                 // in tenths of seconds (15 seconds)
    
    // Inputs
    static const int button0 = INPUT0;
    static const int button1 = INPUT1;
    static const int button2 = INPUT2;
    static const int button3 = INPUT3;
    static const int button4 = INPUT4;
    static const int button5 = INPUT5;
    
    // Outputs
    static const int feedbackLight = OUTPUT0;
    static const int solvedOutput = RELAY0;
  
    void recordInputButtonSequence(void);

    int scanInputsSteady(void);
  
    int _inputSequencePosition = 0;

    int _scanPreviousInputButton = NO_INPUT_DETECTED;

    int _gameResetDownTimer = 0;
    
    int _waitForNoInput = 0;

    // FX300 Inputs for game
    int _inputButtonPinList[GAME_INPUT_COUNT] =  {button0, button1, button2, button3, button4, button5};
};

#endif
