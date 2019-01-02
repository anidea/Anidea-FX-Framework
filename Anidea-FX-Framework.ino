/*

  Escape Room Techs Controller System

  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.5

   -------------------
*/

#include <Arduino.h>
#include "game.h"
#include "network.h"

// Networks
//#include "network_empty.h"
//#include "network_escaperoommaster.h"
//#include "network_cluecontrol.h"
//#include "network_houdinimc.h"
//#include "network_mqtt.h"
#include "network_mqtt_M3.h"

// Games
//#include "game_empty.h"
#include "game_room.h"
//#include "game_simplegame.h"
//#include "game_sequencedetect.h"
//#include "game_sixwire.h"
//#include "game_inputsequence.h"
//#include "game_rfid.h"

// Generic game and network objects
Game *myGame = nullptr;
Network *myNetwork = nullptr;

#if defined(FX300) || defined(FX350)
#include <MsTimer2.h>
#endif

#if defined(FX450)
#include <avdweb_SAMDtimer.h>
void tenHzTimer(struct tc_module *const module_inst);
SAMDtimer *timer3_10Hz;
#endif


room myRoom();

//-------------------------------------------------------------------

void setup()
{
  Serial.begin(115200); // Setup serial

  int timeout = 0;
  while (!Serial)
  {
	  delay(50);
	  if (++timeout > 20) break; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Welcome to the FX-Framework");
  Serial.print("Build date: ");
  Serial.println(__DATE__);
  Serial.print("Build time: ");
  Serial.println(__TIME__);

  // Start game last after hardware is setup

#if defined(FX60_0_ENABLE) || defined(FX60_1_ENABLE)  || defined(DIGITAL_HALL)
  Wire.begin();
#endif

  // Uncomment only one of these lines for the game you want
  //myGame = new game_empty(); // Empty game to manually control inputs and outputs only
  myGame = new room(); // Used to control a whole room
  //myGame = new simplegame(); //Simple game provided as an example
  //myGame = new sequencedetect(); //Sequencedetect
  //myGame = new sixwire(); //Sixwire
  //myGame = new inputsequence(); //Detects a sequence of inputs
  //myGame = new rfid();

  byte MyMac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xA2};   // This must be unique for each device
  IPAddress MyIP(0, 0, 0, 0);                         // This must be unique for each device on the network. Leave blank to configure at run time.
  IPAddress HostIP(0, 0, 0, 0);                       // This should be the IP of the device running the management software. Not needed for ERM

  // Uncomment only one of these lines for the network you want
  //myNetwork = new network_empty(); //Empty network for use with FX300
  //myNetwork = new escaperoommaster(MyMac, MyIP, HostIP);
  //myNetwork = new cluecontrol(MyMac, MyIP, HostIP);
  //myNetwork = new mqtt(MyMac, MyIP, HostIP);
myNetwork = new mqtt_m3(MyMac, MyIP, HostIP);
	//myNetwork = new houdinimc(MyMac, MyIP, HostIP);


  bool missing = false;
  
  if (!myGame)
  {
    Serial.println("No game selected! Halting execution...");
    missing = true;
  }
  
  if (!myNetwork)
  {
    Serial.println("No network selected! Halting execution...");
    missing = true;
  }

  if (missing) while (1) {}
  
  myNetwork->setGame(myGame);

  // Setup timer for simon says, etc.
  #if defined(FX300) || defined(FX350)
    MsTimer2::set(TIMER_INTERVAL, tenHzTimer);
    MsTimer2::start();
  #endif
  
  #if defined(FX450)
    timer3_10Hz = new SAMDtimer(3, tenHzTimer, 1e3 * TIMER_INTERVAL); // 10Hz Timer
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
  if (myGame != nullptr) myGame->tick();
}
#endif

#if defined(FX450)
void tenHzTimer(struct tc_module *const module_inst) 
{
  if (myGame != nullptr) myGame->tick();

  if (myNetwork != nullptr) myNetwork->tick();
	
  //Serial.println("Tick");
}
#endif


