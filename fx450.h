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

// Uncomment revision used

//#define FX450_REV0
//#define FX450_REV1
#if !defined(FX450_REV0) && !defined(FX450_REV1)
#error "Please select a revision"
#endif

#include <EEPROM.h>
#include <variant.h>
//FX450 Header

#ifdef FX450_REV1

#define DIGITAL_HALL

const int RELAY0 = 3;
const int RELAY1 = 2;
#else
const int RELAY0 = 2;
const int RELAY1 = 3;
#endif

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

#ifdef FX450_REV1
const int INPUT6 = A7;
const int INPUT7 = A6;
#endif

const int ANALOG_IN = 9;
const int HALL = 8;

const int RS485_ENABLE = 1;
HardwareSerial* const RS485_SERIAL = &Serial5;

const int STAT_LED = 13;

#ifdef FX450_REV1
const int RX_LED = 25;
#endif

#ifdef FX450_REV1
const int NUM_INPUTS = 8;
#else
const int NUM_INPUTS = 6;
#endif

const int NUM_OUTPUTS = 6;

const int NUM_RELAYS = 2;

const int INPUTS[NUM_INPUTS] =
{
  INPUT0, INPUT1, INPUT2, INPUT3, INPUT4, INPUT5,
#ifdef FX450_REV1  
  INPUT6, INPUT7
#endif
};

const int OUTPUTS[NUM_OUTPUTS] = { OUTPUT0, OUTPUT1, OUTPUT2, OUTPUT3, OUTPUT4, OUTPUT5 };

const int RELAYS[NUM_RELAYS] = { RELAY0, RELAY1 };

const int WIZ_CS = 10;

#if defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#endif


