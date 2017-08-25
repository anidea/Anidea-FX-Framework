/*

  Game Sequence Detect header file

  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef game_lightsout_h
#define game_lightsout_h

#include "arduino.h"
#include "fx300.h"
#include "network.h"
#include "game.h"

class Network;

class lightsout : public Game
{
  public:
    lightsout();

    virtual void loop();

    virtual void reset();

  private:
    int _newKey;

    int _scanXkey;
    int _scanYkey;
    
    int scanMatrix(void);
    //int scanInputsSteady(void);
    void allLightsOnOff(int state);
    void toggleLights(int x, int y);
    void setLights(int dontUseMatrix, int overrideState);
    int  checkForWin(void);
    void setGrid(int8_t setGridState);
    void dumpMatrix(void);
    int getDebouncedInput(int);
    
    static const int HALL_NORTH_THRESH = (.4 * 0x3FF);
    static const int HALL_SOUTH_THRESH = (.6 * 0x3FF);

    // Software configuration defines - used for simple debounce
    const int DEBOUNCE_COUNT = 6;   // Try to keep even
    const int DEBOUNCE_TIME = 5;
    
    // Configure the steady state input filter
    // Inputs must be steady for COUNT * TIME before being accepted
    static const int STEADY_STATE_INPUTS_COUNT = 25;
    static const int STEADY_STATE_INPUTS_TIME = 10;  // 25 * 10 = 1/4 second
    static const int NO_INPUT_DETECTED = -1;

    // Game Definition Variables
    static const int GAME_KEYSCAN_X_COUNT = 4;
    static const int GAME_KEYSCAN_Y_COUNT = 4;
    
    static const int LIGHTS_I2CADDR_LOW = 56;
    static const int LIGHTS_I2CADDR_HIGH = 57;
    
    static const int GAME_LIGHT_OUTPUT = (GAME_KEYSCAN_X_COUNT * GAME_KEYSCAN_Y_COUNT);
    
    static const int BITS_IN_A_BYTE = 8;
    
    //static const int GAME_INPUT_COUNT = 5;                   // Number of inputs/buttons/hall sensors in the sequence
    //static const int GAME_LIGHT_OUTPUT = GAME_INPUT_COUNT;    // Number of light outputs
    //static const int GAME_MAX_SEQUENCE = 10;                  // Max sequence lenght.  Really limited by the eeprom memory, but here to sanity
    //static const int GAME_MAX_SOLVE_TIME = 150;                 // in tenths of seconds (15 seconds)
    
    // FX300 Relays
    static const int SOLVED = RELAY1;

    // FX300 Inputs for key scan
    int _inputKeyScanPinList[GAME_KEYSCAN_X_COUNT] =  {INPUT0, INPUT1, INPUT2, INPUT3};
    int _outputKeyScanPinList[GAME_KEYSCAN_Y_COUNT] =  {OUTPUT0, OUTPUT1, OUTPUT2, OUTPUT3};

    int8_t _lightMatrix[GAME_KEYSCAN_X_COUNT][GAME_KEYSCAN_Y_COUNT];

    uint8_t i2cLightAddresses[GAME_LIGHT_OUTPUT / BITS_IN_A_BYTE] = {LIGHTS_I2CADDR_LOW, LIGHTS_I2CADDR_HIGH};
};

#endif

