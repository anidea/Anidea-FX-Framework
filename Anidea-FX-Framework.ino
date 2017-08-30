/*

  Anidea Controller System

  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#include <MsTimer2.h>

// Uncomment only one of these lines for the board you want
#include "fx300.h"
//#include fx###.h

#include "game.h"
#include "network.h"

// Networks
#include "escaperoommaster.h"
#include "cluecontrol.h"
#include "houdinimc.h"

// Include game headers here
#include "game_simplegame.h"
#include "game_lightsout.h"
#include "game_sequencedetect.h"
#include "game_sixwire.h"
#include "game_inputsequence.h"
#include "game_empty.h"

// Generic game and network objects
Game *myGame;
Network *myNetwork;

//-------------------------------------------------------------------

void setup() {
  Serial.begin(115200);          //  setup serial

  // Setup timer for simon says, etc.
  MsTimer2::set(100, tenHzTimer);
  MsTimer2::start();

  pinMode(RELAY0, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(OUTPUT0, OUTPUT);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);
  pinMode(OUTPUT3, OUTPUT);
  pinMode(OUTPUT4, OUTPUT);
  pinMode(OUTPUT5, OUTPUT);
  pinMode(LED, OUTPUT);

  pinMode(INPUT0, INPUT);
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(INPUT3, INPUT);
  pinMode(INPUT4, INPUT);
  pinMode(INPUT5, INPUT);

  // Start game last after hardware is setup

  // Uncomment only one of these lines for the game you want
//  myGame = new simplegame(); //Simple game provided as an example
//  myGame = new lightsout(); //Lights out game description
//  myGame = new sequencedetect(); //Sequencedetect description
//  myGame = new sixwire(); //Sixwire description
//  myGame = new inputsequence(); //Sequencedetect description
//  myGame = new empty(); // Empty game to manually control inputs/outputs only

  byte MyMac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5};   // This must be unique for each device
  IPAddress MyIP(10, 0, 1, 210);                          // This must be unique for each device
  IPAddress HostIP(10, 0, 1, 115);                           // This should be the IP of the device running the management software
  int CCRegister = 5; // Register for Clue Control

  // Uncomment only one of these lines for the network you want
//  myNetwork = new escaperoommaster(MyMac, MyIP, HostIP);
//  myNetwork = new cluecontrol(MyMac, MyIP, HostIP, CCRegister);
//  myNetwork = new nodered(MyMac, MyIP, HostIP);
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
