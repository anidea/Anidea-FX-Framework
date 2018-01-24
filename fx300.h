
/*

  FX300 general header file

  This application is targeted for the FX300, an Arduino compatible controller from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing and FX300 to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

// Select board type

#define FEATHERM0
//#define FX300
//#define FX350
//#define FX450


#ifndef fx300_h
#define fx300_h

// Includes for entire project
#if defined(FX450) || defined(FEATHERM0)

#include <FlashAsEEPROM.h>
#include <FlashStorage.h>

#endif

#if defined(FX300) || defined(FX350)
#include <EEPROM.h>
#endif


#if defined(FX300) || defined(FX350) || defined(FX450)

//FX300 Header
const int RELAY0 = 2;
const int RELAY1 = 3;

const int OUTPUT0 = 4;
const int OUTPUT1 = 5;
const int OUTPUT2 = 6;
const int OUTPUT3 = 7;
const int OUTPUT4 = 8;
const int OUTPUT5 = 9;

const int INPUT0 = 14;
const int INPUT1 = 15;
const int INPUT2 = 16;
const int INPUT3 = 17;
const int INPUT4 = 18;
const int INPUT5 = 19;

const int RS485_ENABLE = OUTPUT5;

const int LED = -1;

const int HALL = 6;

const int NUM_INPUTS = 6;

const int NUM_OUTPUTS = 6;

const int NUM_RELAYS = 2;

const int INPUTS[NUM_INPUTS] = {INPUT0, INPUT1, INPUT2, INPUT3, INPUT4, INPUT5};

const int OUTPUTS[NUM_OUTPUTS] = {OUTPUT0, OUTPUT1, OUTPUT2, OUTPUT3, OUTPUT4, OUTPUT5};

const int RELAYS[NUM_RELAYS] = {RELAY0, RELAY1};

#endif
