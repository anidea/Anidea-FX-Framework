/*

  CCModbus.h - an Arduino Library for creating a ModBus connection to ClueControl
  Copyright (c) 2016 Shawn Yates
  www.cluecontrol.com
  
  written and tested with Arduino 1.6.9
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

  For this library the following library is used as start point:
  
    [0]MgsModbus.h - an Arduino library for a Modbus TCP master and slave.
       V-0.1.1 Copyright (C) 2013  Marco Gerritse
  
    [1] Mudbus.h - an Arduino library for a Modbus TCP slave.
        Copyright (C) 2011  Dee Wykoff
		
    [2] Function codes 15 & 16 by Martin Pettersson
    
  The following references are used to write this library:
  
    [3] Open Modbus/Tcp specification, Release 1.0, 29 March 1999
        By Andy Swales, Schneider Electric
    [4] Modbus application protocol specification V1.1b3, 26 april 202
        From http:/www.modbus.org
        
  External software used for testing:
  
    [5] modpoll - www.modbusdriver.com/modpoll.html
    [6] ananas - www.tuomio.fi/ananas
    [7] mod_rssim - www.plcsimulator.org 
    [8] modbus master - www.cableone.net/mblansett/
    
  
  For the master the following modbus functions are implemented: 6
  For the slave the following modbus functions are implemented: 6
  
  This is a limited ModBus Master and/or Slave to minimze the code footprint.
  
  When set as a master, this device is a TRIGGER in ClueControl, able to tell ClueControl when something happens.
  The master can do the following:
     Initiate communications to the slave (ClueControl) and receive responses
     send a command to Write a 1 word value (in register #1) to the ClueControl (this is how this module can tell ClueControl something happened)
     Send a commadn to write true or false to a specified register to ClueControl to turn a boolean trigger off/on in ClueControl.
	 
  When set as a slave, thid device is a SWTICH in ClueControl, ClueControl can send commands to tell this device to do something
  The slave can do the following:
     Await command from the master (ClueControl) and send responses
	 Respond to a command to write 1 word register #1 (This is how ClueControl tells this module to do something)
	 
	  
  The following functions descrive the interface to this module
  
        Common
                        Mb.ReceiveModBus() - Receve data, this is needed for both master and slave. 
                        Keep this in your main loop so the process can check for received data from ClueControl
                        
	      Master
			                  ClueControlIP - set this to the IP address of the ClueControl computer
			                  Mb.SendToCC (NewVal) - Send the one WORD value (2 bytes) of NewVal to ClueControl
                        mb.SetTrigger (Address,Value) - Set the value of the indicated register address to value (true or false)
  
        Slave
                        Mb.CheckCC () - Check for a new value from ClueControl.  
                        CCValue - one WORD (2 bytes) value received from ClueControl.  
                          ** Suggestion: Set this to 0 after you read it so you can easily
                          **             tell when a new value is received.
                          
  V-0.2.0 2016-09-07
  Added SetTrigger function

  
  V-0.1.0 2013-03-02
  initinal version
*/


#include "Arduino.h"
#include <Ethernet2.h>


#ifndef CCModBus_h
#define CCModBus_h

#define MB_PORT 502

enum MB_FC {
  MB_FC_NONE                     = 0,
  MB_FC_READ_COILS               = 1,
  MB_FC_READ_DISCRETE_INPUT      = 2,
  MB_FC_READ_REGISTERS           = 3,
  MB_FC_READ_INPUT_REGISTER      = 4,
  MB_FC_WRITE_COIL               = 5,
  MB_FC_WRITE_REGISTER           = 6,
  MB_FC_WRITE_MULTIPLE_COILS     = 15,
  MB_FC_WRITE_MULTIPLE_REGISTERS = 16
};

class CCModBus
{
public:
  // general
  CCModBus();
  void ReceiveModBus();  
  
  // modbus master
  void SetTrigger(word RegAddr, bool TurnOn);
  void SendToCC(word NewVal);
  IPAddress ClueControlIP;
  
  //IPAddress remSlaveIP;
  // modbus slave
  
  word CCValue; // memory block that holds the new value from ClueControl

private: 
  // general
  int MbmCounter;
  
  // modbus master
  uint8_t MbmByteArray[50]; // send and recieve buffer
  void SendData();  //send updates to the slave  

  
  //modbus slave
  uint8_t MbsByteArray[50]; // send and recieve buffer
};

#endif















