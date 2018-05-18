/*

  FX450 general header file

  This application is targeted for the FX450, an Arduino compatible controller from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing an FX450 to support us making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#ifndef fx450_h
#define fx450_h

#include <EEPROM.h>
#include <variant.h>
//FX450 Header
const int RELAY0 = 2;
const int RELAY1 = 3;

const int OUTPUT0 = 4;
const int OUTPUT1 = 5;
const int OUTPUT2 = 6;
const int OUTPUT3 = 7;
const int OUTPUT4 = 11;
const int OUTPUT5 = 12;

const int INPUT0 = A0;
const int INPUT1 = A1;
const int INPUT2 = A2;
const int INPUT3 = A3;
const int INPUT4 = A4;
const int INPUT5 = A5;

const int ANALOG_IN = 9;

const int RS485_ENABLE = 1;
HardwareSerial* const RS485_SERIAL = &Serial5;

const int LED = 13;

const int HALL = 8;

const int NUM_INPUTS = 6;

const int NUM_OUTPUTS = 6;

const int NUM_RELAYS = 2;

const int INPUTS[NUM_INPUTS] = {INPUT0, INPUT1, INPUT2, INPUT3, INPUT4, INPUT5};

const int OUTPUTS[NUM_OUTPUTS] = {OUTPUT0, OUTPUT1, OUTPUT2, OUTPUT3, OUTPUT4, OUTPUT5};

const int RELAYS[NUM_RELAYS] = {RELAY0, RELAY1};

const int WIZ_CS = 10;

#if defined(SERIAL_PORT_USBVIRTUAL)
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

#endif
