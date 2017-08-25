/*

  Game Sequence Detect header file

  This application is targeted for the FX300, an Arduino compatible controller from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing and FX300 to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef game_sequencedetect_h
#define game_sequencedetect_h

#include "arduino.h"

#include "fx300.h"
#include "network.h"
#include "game.h"

class sequencedetect : public Game
{
  public:
    sequencedetect();

    virtual void loop();

    virtual void forceSolved();

    void solved();

    virtual void reset(); // Call to reset game

    virtual void tick();

  private:
    void (*solvedCallback)(void)= NULL;
  
    // Global Game Variables
    int _puzzleSolved = 0;
    int _gameState = 0;
    int _inputSequencePosition = 0;
    int _scanPreviousInputButton = NO_INPUT_DETECTED;
    int _runLightSequence = 0;
    int _gameResetDownTimer = 0;
    int _waitForNoInput = 0;

    void recordInputButtonSequence(void);
    int scanInputsSteady(void);
    void allLightsOnOff(int state);

    static const int HALL_NORTH_THRESH = (.4 * 0x3FF);
    static const int HALL_SOUTH_THRESH = (.6 * 0x3FF);
    
    // Software configuration defines - used for simple debounce
    static const int DEBOUNCE_COUNT = 6;   // Try to keep even
    static const int DEBOUNCE_TIME = 5;
    
    // Configure the steady state input filter
    // Inputs must be steady for COUNT * TIME before being accepted
    static const int STEADY_STATE_INPUTS_COUNT = 25;
    static const int STEADY_STATE_INPUTS_TIME = 10;  // 25 * 10 = 1/4 second
    static const int NO_INPUT_DETECTED = -1;
    
    // Game Definition Variables
    static const int GAME_INPUT_COUNT = 5;                   // Number of inputs/buttons/hall sensors in the sequence
    static const int SEQUENCE_GAME_LIGHT_OUTPUT = GAME_INPUT_COUNT;    // Number of light outputs
    static const int GAME_MAX_SEQUENCE = 10;                  // Max sequence lenght.  Really limited by the eeprom memory, but here to sanity
    static const int GAME_MAX_SOLVE_TIME = 150;                 // in tenths of seconds (15 seconds)
    
    // Light output mode, enable only one
    static const int GAME_LIGHT_OUTPUT_NONE = 0; // No lights
    static const int GAME_LIGHT_OUTPUT_SEQUENCE = 0; // Turn a light on for every good button received (note hardware limitations)
    static const int GAME_LIGHT_OUTPUT_SIMONSAYS = 1; // Play the sequence out - NOT incremental challenge like orginal game
    
    // Light output mode options
    static const int GAME_LIGHT_OUTPUT_SIMONSAYS_ON_TIME = 5;  // In tenths of seconds
    static const int GAME_LIGHT_OUTPUT_SIMONSAYS_OFF_TIME = 1; // In tenths of seconds
    static const int GAME_LIGHT_OUTPUT_SIMONSAYS_GAP_TIME = 10; // In tenths of seconds
    
    const int GAME_LIGHT_OUTPUT_SEQUENCE_ONEATATIME = 0; // Enable if you want only one light on a time, otherwise they will be additive
    
    // FX300 Relays
    static const int SOLVED = RELAY1;

    // FX300 Inputs for game
    int _inputButtonPinList[GAME_INPUT_COUNT] =  {INPUT0, INPUT1, INPUT2, INPUT3, INPUT4};

    // FX300 Outputs
    int _outputLightsPinList[SEQUENCE_GAME_LIGHT_OUTPUT] =  {OUTPUT0, OUTPUT1, OUTPUT2, OUTPUT3, OUTPUT4};

};

#endif

