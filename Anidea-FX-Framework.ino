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

#include <MsTimer2.h>

// Uncomment only one of these lines for the board you want
#include "fx300.h"
//#include fx###.h

#include "game.h"
#include "network.h"

// Networks
#include "network_empty.h"
#include "network_escaperoommaster.h"
#include "network_cluecontrol.h"
#include "network_houdinimc.h"
#include "network_mqtt.h"

// Include game headers here
#include "game_empty.h"
#include "game_room.h"
#include "game_simplegame.h"
#include "game_sequencedetect.h"
#include "game_sixwire.h"
#include "game_inputsequence.h"

// Generic game and network objects
Game *myGame;
Network *myNetwork;

//-------------------------------------------------------------------

void setup() {
  Serial.begin(115200); // Setup serial

  // Setup timer for simon says, etc.
  MsTimer2::set(100, tenHzTimer);
  MsTimer2::start();

  // Start game last after hardware is setup

  // Uncomment only one of these lines for the game you want
//  myGame = new game_empty(); // Empty game to manually control inputs and outputs only
//  myGame = new room(); // Used to control a whole room
//  myGame = new simplegame(); //Simple game provided as an example
//  myGame = new sequencedetect(); //Sequencedetect
//  myGame = new sixwire(); //Sixwire
//  myGame = new inputsequence(); //Detects a sequence of inputs

   byte MyMac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5};   // This must be unique for each device
   IPAddress MyIP(10, 0, 1, 211);                         // This must be unique for each device on the network. Leave blank to configure at run time.
   IPAddress HostIP(10, 0, 1, 115);                       // This should be the IP of the device running the management software. Not needed for ERM

  // Uncomment only one of these lines for the network you want
//  myNetwork = new network_empty(); //Empty network for use with FX300
//  myNetwork = new escaperoommaster(MyMac, MyIP, HostIP);
//  myNetwork = new cluecontrol(MyMac, MyIP, HostIP);
//  myNetwork = new mqtt(MyMac, MyIP, HostIP);
//  myNetwork = new houdinimc(MyMac, MyIP, HostIP);

  myNetwork->setGame(myGame);
}

void loop()
{ 
  myGame->loop();

  myNetwork->loop();
}

void tenHzTimer()
{
  myGame->tick();
}
