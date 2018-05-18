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
	MyIP = _MyIP;
	HostIP = _HostIP;

	// Write RS485_ENABLE pin to high so serial input will work correctly on this board
	#if defined(FX350)
	digitalWrite(RS485_ENABLE, HIGH);
	#endif
	
	// Give user a chance to configure IP through serial
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
        getIP();
        Serial.println();
      }
    }

	// Check if IP has been set, if not fall back to EEPROM and then DHCP if enabled
	if (MyIP[0] == 0) // MyIP has not been set
	{
		Serial.println("MyIP not set");
		if (EEPROM.read(0) == 1) // MyIP is saved in EEPROM
		{
			Serial.println("Reading MyIP from EEPROM");
			for (int i = 1; i < 5; i++) // Read in from EEPROM
			{
				MyIP[i - 1] = EEPROM.read(i);
			}
		}
		#ifdef ENABLE_DHCP
		else // IP is not saved and can be gotten through DHCP
		{
			getIP_DHCP();
		}
		#else
		else // IP is not saved
		{
			Serial.println("Warning: No IP address has been set");
		}
		#endif
	}

	if (HostIP[0] == 0) // HostIP has not been set
	{
		//Serial.println("HostIP not set");
		if (EEPROM.read(5) == 1) // HostIP is saved in EEPROM
		{
			//Serial.println("Reading HostIP from EEPROM");
			for (int i = 6; i < 10; i++)
			{
				HostIP[i - 6] = EEPROM.read(i);
			}
		}
	}

	// Write pin back to low
	#if defined(FX350)
	digitalWrite(RS485_ENABLE, LOW);
	#endif
  
    Ethernet.begin(MyMac, MyIP);
    
    delay(500);
    
    Serial.println(F("Ethernet interface started"));
     
    // print the Host Address:
    Serial.print(F("Host IP address: "));
    Serial.println(HostIP);
      
    // print your local IP address:
    Serial.print(F("My IP address: "));
    Serial.println(MyIP);
    Serial.println();
  }
}

void Network::loop(void)
{
  
}

void Network::getIP()
{
	#ifdef ENABLE_DHCP
		Serial.println("Please enter an IP address for the controller or type \"DHCP\" to auto generate one");
	#else
		Serial.println("Please enter an IP address for the controller");
	#endif
	while(Serial.available() == 0){} // Wait for input
	Serial.setTimeout(100);
	String input = Serial.readString();

	#ifdef ENABLE_DHCP
	if (input == "DHCP")
	{
		getIP_DHCP();
	}
	else
	{
		if (MyIP.fromString(input))
		{
			Serial.println("IP successfully read");
			EEPROM.write(0, 1); // Indicate that MyIP has been written
			for (int i = 1; i < 5; i++) // Write MyIP to EEPROM
			{
				EEPROM.write(i, MyIP[i - 1]);
			}
		}
		else
		{
			Serial.println("IP invalid");
		}
	}
	#else
	if (MyIP.fromString(input))
	{
		Serial.println("IP successfully read");
		EEPROM.write(0, 1); // Indicate that MyIP has been written
		for (int i = 1; i < 5; i++) // Write MyIP to EEPROM
		{
			EEPROM.write(i, MyIP[i - 1]);
		}
	}
	else
	{
		Serial.println("IP invalid");
	}
	#endif

	Serial.println("Please enter an IP address for the host (Enter 0 if using ERM)");
	while(Serial.available() == 0){} // Wait for input
	Serial.setTimeout(100);
	input = Serial.readString();
  
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
			EEPROM.write(5, 1); // Indicate that HostIP has been written
			for (int i = 6; i < 10; i++) // Write HostIP to EEPROM
			{
				EEPROM.write(i, HostIP[i - 6]);
			}
		}
		else
		{
		  Serial.println("IP invalid");
		}
	}
}

#ifdef ENABLE_DHCP
void Network::getIP_DHCP()
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
	}
}
#endif
