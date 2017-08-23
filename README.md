# Anidea FX Controller

This application is targeted for the AEI FX series of boards, an Arduino compatible series of controllers from Anidea Engineering.  While this program is free (MIT LECENSE), please consider purchasing and FX300 to support us making more free code.

You can purchase a FX300 at http://get.anidea-engineering.com/fx300_order_request

Generic sequence detection/simon says prop controller

This project was designed to be a very flexible input sequence detector and light conntroller.

Basic setup is provide a list of inputs, set a list of outputs, and then set defined to control the light responses, etc.

The sequnce is programmed into EEPROM via the hall sensor (North to start programming, South to end programming).  Also have the serial port attached at 9600 bps to monitor the status.

## AEI FX300
 ------------------------
* *INPUTS*
  Button or hall inputs are specified in a list below.  High is active

  Enable, when low, the game is enabled

  Reset, when high, the game is reset

* *OUTPUTS*
  Light outputs either turn on in sequence when the correct input is received or they can illustrate the pattern in a simon says mode

* *RELAY OUTPUTS*
 SOLVED       (Connect to relay1 COM and NO or NC, depending on requirements, the SOLVED state will connect COM to NO)


* ***  SOLVED is run backward to provide safe operation such that if the prop dies or isn't powered, the maglock opens


## General operation:

STARTUP
  Turn power on

RESET
  Called at startup, after programming and when the reset input is activated
  Turn on the solved output
  Clear any in progress game play.
  Go to game play.

CONFIGURATION
  There are a number of defines which set the game play and operation of the program.  Read through them below for details.

  To program the sequence, bring up the serial console at 9600 bps.  Then introduce a NORTH magnet to the hall sensor.  Then run through your inputs (buttons, hall, switches, etc).
  Each sucessful input will respond with console output and light 0 (first) one flashing.
  When the entire sequenced is entered, provide a SOUTH magnet to the hall sensor.  The sequence is programmed into EEPROM.

GAME PLAY

 If enbaled below, RESET high will hold the game in reset and clear the game play

 If enabled below, ENABLE low will allow the game to be played

 Once enabled, the light sequence will start.  If simon says, then the pattern will play.  If sequence mode, all lights will be off until a a correct input is received.  Simon Mode does not build.  It shows a pattern that must be replicated.

 As correct inputs are received, the outputs will progress giving positive feedback to the player

 Once the entire sequence is entered, the solved routine is run and the solved relay will disengage.
```
// Configure the steady state input filter
// Inputs must be steady for COUNT * TIME before being accepted
#define STEADY_STATE_INPUTS_COUNT    50
#define STEADY_STATE_INPUTS_TIME     10  // 50 * 10 = 1/2 second
#define NO_INPUT_DETECTED           -1

// Game Definition Variables
#define GAME_INPUT_COUNT          5                   // Number of inputs/buttons/hall sensors in the sequence
#define GAME_LIGHT_OUTPUT         GAME_INPUT_COUNT    // Number of light outputs
#define GAME_MAX_SEQUENCE         10                  // Max sequence lenght.  Really limited by the eeprom memory, but here to sanity
#define GAME_MAX_SOLVE_TIME       150                 // in tenths of seconds (15 seconds)

// Light output mode, enable only one
//#define GAME_LIGHT_OUTPUT_NONE        // No lights
//#define GAME_LIGHT_OUTPUT_SEQUENCE    // Turn a light on for every good button received (note hardware limitations)
#define GAME_LIGHT_OUTPUT_SIMONSAYS   // Play the sequence out

// Light output mode options
#define GAME_LIGHT_OUTPUT_SIMONSAYS_TIME  5 // In tenths of seconds

//#define GAME_LIGHT_OUTPUT_SEQUENCE_ONEATATIME // Enable if you want only one light on a time, otherwise they will be additive

// FX300 Inputs for game
static int inputButtonPinList[GAME_INPUT_COUNT] =  {INPUT0, INPUT1, INPUT2, INPUT3, INPUT4};

#define GAME_INPUT_RESET  INPUT5  // When high and enabled (defined), the game will be reset
#define GAME_INPUT_ENABLE INPUT5  // When low and enabled (defined), the game will be enabled

// FX300 Outputs
static int outputLightsPinList[GAME_LIGHT_OUTPUT] =  {OUTPUT0, OUTPUT1, OUTPUT2, OUTPUT3, OUTPUT4};
```
