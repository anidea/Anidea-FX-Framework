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
  if (pMyGame->INPUT0_OVERRIDE_ENABLE == 1)
  {
    INPUT0_STATE = digitalRead(INPUT0);
  }
  if (pMyGame->INPUT1_OVERRIDE_ENABLE == 1)
  {
    INPUT1_STATE = digitalRead(INPUT1);
  }
  if (pMyGame->INPUT2_OVERRIDE_ENABLE == 1)
  {
    INPUT2_STATE = digitalRead(INPUT2);
  }
  if (pMyGame->INPUT3_OVERRIDE_ENABLE == 1)
  {
    INPUT3_STATE = digitalRead(INPUT3);
  }
  if (pMyGame->INPUT4_OVERRIDE_ENABLE == 1)
  {
    INPUT4_STATE = digitalRead(INPUT4);
  }
  if (pMyGame->INPUT5_OVERRIDE_ENABLE == 1)
  {
    INPUT5_STATE = digitalRead(INPUT5);
  }

  if (pMyGame->OUTPUT0_OVERRIDE_ENABLE == 1)
  {
    digitalWrite(OUTPUT0, OUTPUT0_STATE);
  }
  if (pMyGame->OUTPUT1_OVERRIDE_ENABLE == 1)
  {
    digitalWrite(OUTPUT1, OUTPUT1_STATE);
  }
  if (pMyGame->OUTPUT2_OVERRIDE_ENABLE == 1)
  {
    digitalWrite(OUTPUT2, OUTPUT2_STATE);
  }
  if (pMyGame->OUTPUT3_OVERRIDE_ENABLE == 1)
  {
    digitalWrite(OUTPUT3, OUTPUT3_STATE);
  }
  if (pMyGame->OUTPUT4_OVERRIDE_ENABLE == 1)
  {
    digitalWrite(OUTPUT4, OUTPUT4_STATE);
  }
  if (pMyGame->OUTPUT5_OVERRIDE_ENABLE == 1)
  {
    digitalWrite(OUTPUT5, OUTPUT5_STATE);
  }

  if (pMyGame->RELAY0_OVERRIDE_ENABLE == 1)
  {
    digitalWrite(RELAY0, RELAY0_STATE);
  }
  if (pMyGame->RELAY1_OVERRIDE_ENABLE == 1)
  {
    digitalWrite(RELAY1, RELAY1_STATE);
  }

}

void Network::sendGameSolved(void)
{
  
}

void Network::receiveGameReset(void)
{
  
}
