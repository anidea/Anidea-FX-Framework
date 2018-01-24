/*

  Anidea Controller System

  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.5

   -------------------
*/

#include "arduino.h"

#include "game.h"
#include "network.h"

#if defined(FX300) || defined(FX350)
#include <MsTimer2.h>
#endif

#if defined(FX450) || defined(FEATHERM0)
// Also add library (zip) https://github.com/adafruit/Adafruit_ZeroTimer.git
// Also add library (zip) https://github.com/avandalen/avdweb_SAMDtimer.git
// Also add library (zip) https://github.com/adafruit/Adafruit_ASFcore.git, maybe not needed anymore

#include <avdweb_SAMDtimer.h>
void tenHzTimer(struct tc_module *const module_inst);
SAMDtimer *timer3_10Hz;
#endif

// Networks
#include "network_empty.h"
#include "network_escaperoommaster.h"
#include "network_cluecontrol.h"
#include "network_houdinimc.h"
#include "network_mqtt.h"
#include "network_mqtt_adv.h"

// Include game headers here
#include "game_empty.h"
#include "game_room.h"
#include "game_simplegame.h"
#include "game_sequencedetect.h"
#include "game_sixwire.h"
#include "game_inputsequence.h"
#include "game_rfid.h"

// Generic game and network objects
Game *myGame;
Network *myNetwork;

//-------------------------------------------------------------------

void setup() {
  Serial.begin(115200); // Setup serial

  #if 0 // make 1 if you want the code to wait for the serial monitor to come up
  while (!Serial)
  {
  ; // wait for serial port to connect. Needed for native USB port only
  }
  #endif

  Serial.println("Welcome to the FX-Framework");

  // Start game last after hardware is setup

  // Uncomment only one of these lines for the game you want
//  myGame = new game_empty(); // Empty game to manually control inputs and outputs only
//  myGame = new room(); // Used to control a whole room
//  myGame = new simplegame(); //Simple game provided as an example
//  myGame = new sequencedetect(); //Sequencedetect
//  myGame = new sixwire(); //Sixwire
//  myGame = new inputsequence(); //Detects a sequence of inputs
  myGame = new rfid();

  byte MyMac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB6};   // This must be unique for each device
  IPAddress MyIP(0, 0, 0, 0);                         // This must be unique for each device on the network. Leave blank to configure at run time.
  IPAddress HostIP(0, 0, 0, 0);                       // This should be the IP of the device running the management software. Not needed for ERM
//  IPAddress MyIP(10, 0, 1, 205);                         // This must be unique for each device on the network. Leave blank to configure at run time.
//  IPAddress HostIP(10, 0, 1, 115);                       // This should be the IP of the device running the management software. Not needed for ERM

  // Uncomment only one of these lines for the network you want
//  myNetwork = new network_empty(); //Empty network for use with FX300
//  myNetwork = new escaperoommaster(MyMac, MyIP, HostIP);
//  myNetwork = new cluecontrol(MyMac, MyIP, HostIP);
//  myNetwork = new mqtt(MyMac, MyIP, HostIP);
  myNetwork = new mqtt_adv(MyMac, MyIP, HostIP);
//  myNetwork = new houdinimc(MyMac, MyIP, HostIP);

  myNetwork->setGame(myGame);

  // Setup timer for simon says, etc.
  #if defined(FX300) || defined(FX350)
    MsTimer2::set(100, tenHzTimer);
    MsTimer2::start();
  #endif
  
  #if defined(FX450) || defined(FEATHERM0)
    timer3_10Hz = new SAMDtimer(3, tenHzTimer, 1e5); // 10Hz Timer
  #endif

}

void loop()
{
  myGame->loop();

  myNetwork->loop();
}

#if defined(FX300) || defined(FX350)
void tenHzTimer()
{
  myGame->tick();
  myNetwork->tick();
}
#endif

#if defined(FX450) || defined(FEATHERM0)
void tenHzTimer(struct tc_module *const module_inst) 
{
  myGame->tick();
  myNetwork->tick();
  //Serial.println("Tick");
}
#endif

