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

Network::Network(byte _MyMac[], IPAddress _MyIP, IPAddress _HostIP, bool connectNetwork) : server(80)
{
  if (connectNetwork)
  {
    MyMac = _MyMac;
  
    if (_MyIP[0] == 0) // MyIP is not set by program
    {
  //    Serial.println("MyIP not set");
      if (EEPROM.read(0) == 1) // MyIP is saved in EEPROM
      {
  //      Serial.println("Reading MyIP from EEPROM");
        for (int i = 1; i < 5; i++)
        {
          MyIP[i - 1] = EEPROM.read(i);
        }
      }
      else // IP is not saved and needs to be provided by user
      {
        Serial.println();
  //      Serial.println("Getting IP from user");
        getIP(1);
      }
    }
    else // MyIP is set by program
    {
      MyIP = _MyIP;
    }
  
    if (_HostIP[0] == 0) // HostIP is not set by program
    {
  //    Serial.println("HostIP not set");
      if (EEPROM.read(5) == 1) // HostIP is saved in EEPROM
      {
  //      Serial.println("Reading HostIP from EEPROM");
        for (int i = 6; i < 10; i++)
        {
          HostIP[i - 6] = EEPROM.read(i);
        }
      }
      else // IP is not saved and needs to be provided by user
      {
        Serial.println();
  //      Serial.println("Getting IP from user");
        getIP(2);
      }
    }
    else // HostIP is set by program
    {
      HostIP = _HostIP;
    }
  
    unsigned long timeout = millis();
    Serial.println();
    Serial.println("Enter anything to configure IP...");
    Serial.println();
    Serial.setTimeout(3000);
    while (millis() - timeout < 3000)
    {
      if (Serial.available() > 0)
      {
        delay(10);
        while (Serial.available() > 0)
        {
          Serial.read();
        }
        getIP(0);
        Serial.println();
      }
    }
  
    Ethernet.begin(MyMac, MyIP);
    
    delay(500);
    
    Serial.println(F("Ethernet interface started"));
     
    // print the Host Address:
    Serial.print(F("Host IP address: "));
    Serial.println(HostIP);
      
    // print your local IP address:
    Serial.print(F("My IP address: "));
    Serial.println(Ethernet.localIP());
    Serial.println();
  }
}

void Network::loop(void)
{
  
}

void Network::getIP(int type)
{
  if (type == 0 || type == 1) // Get MyIP
  {
    Serial.println("Please enter an IP address for the controller or type \"DHCP\" to auto generate one");
    while(Serial.available() == 0){} // Wait for input
    Serial.setTimeout(100);
    String input = Serial.readString();
    if (input == "DHCP")
    {
      Serial.println("Configuring with DHCP");
      if (Ethernet.begin(MyMac) == 0) // Start ethernet with DHCP
      {
        Serial.println(F("Failed to configure Ethernet using DHCP"));
      }
      else
      {
        Serial.println("Succesfuly configured with DHCP");
        MyIP = Ethernet.localIP();
        Serial.println(MyIP);
        Serial.print("Host name: ");
        Serial.println(Ethernet.hostName());
      }
    }
    else
    {
      if (MyIP.fromString(input))
      {
        Serial.println("IP successfully read");
      }
      else
      {
        Serial.println("IP invalid");
      }
    }
    EEPROM.write(0, 1); // Indicate that MyIP has been written
    for (int i = 1; i < 5; i++) // Write MyIP to EEPROM
    {
      EEPROM.write(i, MyIP[i - 1]);
    }
  }

  if (type == 0 || type == 2) // Get HostIP
  {
    Serial.println("Please enter an IP address for the host (Enter 0 if using ERM)");
    while(Serial.available() == 0){} // Wait for input
    Serial.setTimeout(100);
    String input = Serial.readString();
    if (input == "0")
    {
      for (int i = 0; i < 4; i++) // Set HostIP to 0.0.0.0
      {
        HostIP[i] = 0;
      }
    }
    else
    {
      if (HostIP.fromString(input))
      {
        Serial.println("IP successfully read");
      }
      else
      {
        Serial.println("IP invalid");
      }
    }
    EEPROM.write(5, 1); // Indicate that HostIP has been written
    for (int i = 6; i < 10; i++) // Write HostIP to EEPROM
    {
      EEPROM.write(i, HostIP[i - 6]);
    }
  }
}
