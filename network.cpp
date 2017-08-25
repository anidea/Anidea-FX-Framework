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

Network::Network(byte _MyMac[], IPAddress _MyIP, IPAddress _gateway, IPAddress _subnet, IPAddress _HostIP) : server(80)
{
  MyMac = _MyMac;
  MyIP = _MyIP;
  gateway = _gateway;
  subnet = _subnet;
  HostIP = _HostIP;
  
  #if 0
  if (Ethernet.begin(MyMac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(MyMac, MyIP, gateway, subnet);
  }
  #endif

  Ethernet.begin(MyMac, MyIP, gateway, subnet);
  
  delay(5000);
  Serial.println("Ethernet interface started");
 
  // print the Host Address:
  Serial.print("Host IP address: ");
  Serial.println(HostIP);
  Serial.println();
    
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void Network::sendGameSolved(void)
{
  
}

void Network::receiveGameReset(void)
{
  
}

void Network::loop(void)
{
  
}
