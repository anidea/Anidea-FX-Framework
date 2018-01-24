
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

//#define FEATHERM0
//#define FX300
//#define FX350
#define FX450


#ifndef fx_framework_h
#define fx_framework_h

// Boards
#if defined(ARDUINO_AEIFX_300) || defined(ARDUINO_AVR_UNO)
#include "fx300.h"
#endif

#if defined(ARDUINO_AEIFX_450) || defined(ARDUINO_SAMD_ZERO)
#include "fx450.h"
#endif



#endif
