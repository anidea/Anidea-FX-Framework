/*

  Network program file

  This application is targeted for the line of FX boards from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing a board to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/

#include "network.h"

Game *Network::pMyGame = NULL;

bool Network::OUTPUT_STATES[NUM_OUTPUTS] = {false};
bool Network::INPUT_STATES[NUM_INPUTS] = {false};
bool Network::RELAY_STATES[NUM_RELAYS] = {false};

Network::Network(byte _MyMac[], IPAddress _MyIP, IPAddress _HostIP) : server(80)
{
  MyMac = _MyMac;
  MyIP = _MyIP;
  HostIP = _HostIP;
  
  #if 0
  if (Ethernet.begin(MyMac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    Ethernet.begin(MyMac, MyIP);
  }
  #endif

  Ethernet.begin(MyMac, MyIP);
  
  delay(5000);
  Serial.println(F("Ethernet interface started"));
 
  // print the Host Address:
  Serial.print(F("Host IP address: "));
  Serial.println(HostIP);
  Serial.println();
    
  // print your local IP address:
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());
}

void Network::loop(void)
{
  // Read input states
  for (int i = 0; i < NUM_INPUTS; i++)
  {
    if (pMyGame->INPUT_OVERRIDE_ENABLE[i] == 1)
    {
      INPUT_STATES[i] = digitalRead(INPUTS[i]);
    }
  }

  // Write output states
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == 1)
    {
      digitalWrite(OUTPUTS[i], OUTPUT_STATES[i]);
    }
  }

  // Write relay states
  for (int i = 0; i < NUM_RELAYS; i++)
  {
    if (pMyGame->RELAY_OVERRIDE_ENABLE[i] == 1)
    {
      digitalWrite(RELAYS[i], RELAY_STATES[i]);
    }
  }
}

void Network::sendGameSolved(void)
{
  
}

void Network::receiveGameReset(void)
{
  
}
