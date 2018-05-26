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
    Serial.println(F("\nEnter anything to configure IP...\n"));
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
	
	#ifdef GENERATE_NAME
	// Check if name is written in EEPROM
	if (EEPROM.read(NAME_START) == 0) // Name is not saved in EEPROM
	{
		// Write name to EEPROM to match DHCP hostName

		// Write "WIZnet"
		const char* WIZnet = "WIZnet";
		for (int i = NAME_START + 1; i < NAME_START + 7; i++)
		{
			EEPROM.write(i, WIZnet[i - (NAME_START + 1)]);
		}

		// Write last 3 bytes of MAC
		char tmp[7];
		snprintf(tmp, 7, "%02X%02X%02X", MyMac[3], MyMac[4], MyMac[5]);
		for (int i = NAME_START + 7; i < NAME_START + 13; i++)
		{
			EEPROM.write(i, tmp[i - (NAME_START + 7)]);
		}
		EEPROM.write(NAME_START + 13, '\0');
		EEPROM.write(NAME_START, 1);
	}
	#endif // GENERATE_NAME

	// Check if IP has been set, if not fall back to EEPROM and then DHCP if enabled
	if (MyIP[0] == 0) // MyIP has not been set
	{
		//Serial.println(F("MyIP not set"));
		if (EEPROM.read(MyIP_START) == 1) // MyIP is saved in EEPROM
		{
			//Serial.println(F("Reading MyIP from EEPROM"));
			for (int i = MyIP_START + 1; i < MyIP_START + 5; i++) // Read in from EEPROM
			{
				MyIP[i - (MyIP_START + 1)] = EEPROM.read(i);
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
			Serial.println(F("Warning: No IP address has been set"));
		}
		#endif
	}

	if (HostIP[0] == 0) // HostIP has not been set
	{
		//Serial.println(F("HostIP not set"));
		if (EEPROM.read(HostIP_START) == 1) // HostIP is saved in EEPROM
		{
			//Serial.println(F("Reading HostIP from EEPROM"));
			for (int i = HostIP_START + 1; i < HostIP_START + 5; i++)
			{
				HostIP[i - (HostIP_START + 1)] = EEPROM.read(i);
			}
		}
	}

	// Write pin back to low
	#if defined(FX350)
	digitalWrite(RS485_ENABLE, LOW);
	#endif
  
    Ethernet.begin(MyMac, MyIP);
	#ifdef ENABLE_CONTROL_PANEL
	Udp.begin(UDPport);
	#endif
    
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
	#ifdef ENABLE_CONTROL_PANEL
	receiveUDP();
	#endif
}

#ifdef ENABLE_CONTROL_PANEL
void Network::receiveUDP()
{
	int packetSize = Udp.parsePacket();
	if (packetSize)
	{
		char packetBuffer[64] = ""; //buffer to hold incoming packet,
		/*Serial.print("Received packet of size ");
		Serial.println(packetSize);
		Serial.print("From ");
		Serial.print(Udp.remoteIP());
		Serial.print(", port ");
		Serial.println(Udp.remotePort());*/

		// Read the identifying part of the packet into packetBufffer
		Udp.read(packetBuffer, 8);
		//Serial.println(packetBuffer);

		// Packet is from control panel scanning for devices
		if (strcmp(packetBuffer, "AEI_scan") == 0)
		{
			//Serial.println(F("Received scan"));
			Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
			Udp.write("{\"DHCP\": \"");
			char IPBuff[3];
			Udp.write(utoa(EEPROM.read(MyIP_START), IPBuff, 10));
			Udp.write("\", \"IP\": \"");
			Udp.write(utoa(MyIP[0], IPBuff, 10));
			Udp.write(".");
			Udp.write(utoa(MyIP[1], IPBuff, 10));
			Udp.write(".");
			Udp.write(utoa(MyIP[2], IPBuff, 10));
			Udp.write(".");
			Udp.write(utoa(MyIP[3], IPBuff, 10));
			Udp.write("\", \"hostIP\": \"");
			Udp.write(utoa(HostIP[0], IPBuff, 10));
			Udp.write(".");
			Udp.write(utoa(HostIP[1], IPBuff, 10));
			Udp.write(".");
			Udp.write(utoa(HostIP[2], IPBuff, 10));
			Udp.write(".");
			Udp.write(utoa(HostIP[3], IPBuff, 10));
			Udp.write("\", \"Name\": \"");
			for (int i = NAME_START + 1; i < NAME_START + 21; i++)
			{
				if (EEPROM.read(i) == '\0')
				{
					break;
				}
				Udp.write(EEPROM.read(i));
			}
			Udp.write("\"}");
			Udp.endPacket();
		}

		// Packet is from control panel with new configuration for device
		else if(strcmp(packetBuffer, "AEI_conf") == 0)
		{
			//Serial.println(F("Received new configuration"));
			Udp.read(packetBuffer, 56);
			EEPROM.write(MyIP_START, atoi(strtok(packetBuffer, " ")));
			if (MyIP.fromString(strtok(NULL, " ")))
			{
				for (int i = MyIP_START + 1; i < MyIP_START + 5; i++) // Write MyIP to EEPROM
				{
					EEPROM.write(i, MyIP[i - (MyIP_START + 1)]);
				}
			}
			if (HostIP.fromString(strtok(NULL, " ")))
			{
				for (int i = HostIP_START + 1; i < HostIP_START + 5; i++) // Write HostIP to EEPROM
				{
					EEPROM.write(i, HostIP[i - (HostIP_START + 1)]);
				}
				EEPROM.write(5, 1); // Indicate that HostIP has been written
			}
			char *deviceName = strtok(NULL, " ");
			for (int i = NAME_START + 1; i < NAME_START + 21; i++)
			{
				EEPROM.write(i, deviceName[i - (NAME_START + 1)]);
				if (deviceName[i - (NAME_START + 1)] == '\0')
				{
					break;
				}
			}
		}
	}
}
#endif

void Network::getIP()
{
	#ifdef ENABLE_DHCP
		Serial.println(F("Please enter an IP address for the controller or type \"DHCP\" to auto generate one"));
	#else
		Serial.println(F("Please enter an IP address for the controller"));
	#endif
	while(Serial.available() == 0){} // Wait for input
	Serial.setTimeout(100);
	String input = Serial.readString();

	#ifdef ENABLE_DHCP
	if (input == F("DHCP"))
	{
		getIP_DHCP();
	}
	else
	{
		if (MyIP.fromString(input))
		{
			Serial.println(F("IP successfully read"));
			for (int i = MyIP_START + 1; i < MyIP_START + 5; i++) // Write MyIP to EEPROM
			{
				EEPROM.write(i, MyIP[i - (MyIP_START + 1)]);
			}
			EEPROM.write(MyIP_START, 1); // Indicate that MyIP has been written
		}
		else
		{
			Serial.println(F("IP invalid"));
		}
	}
	#else
	if (MyIP.fromString(input))
	{
		Serial.println(F("IP successfully read"));
		for (int i = MyIP_START + 1; i < MyIP_START + 5; i++) // Write MyIP to EEPROM
		{
			EEPROM.write(i, MyIP[i - (MyIP_START + 1)]);
		}
		EEPROM.write(MyIP_START, 1); // Indicate that MyIP has been written
	}
	else
	{
		Serial.println(F("IP invalid"));
	}
	#endif

	Serial.println(F("Please enter an IP address for the host (Enter 0 if using ERM)"));
	while(Serial.available() == 0){} // Wait for input
	Serial.setTimeout(100);
	input = Serial.readString();
  
	if (input == F("0"))
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
			Serial.println(F("IP successfully read"));
			for (int i = HostIP_START + 1; i < HostIP_START + 5; i++) // Write HostIP to EEPROM
			{
				EEPROM.write(i, HostIP[i - (HostIP_START + 1)]);
			}
			EEPROM.write(HostIP_START, 1); // Indicate that HostIP has been written
		}
		else
		{
		  Serial.println(F("IP invalid"));
		}
	}
}

#ifdef ENABLE_DHCP
void Network::getIP_DHCP()
{
	Serial.println(F("Configuring with DHCP"));
	if (Ethernet.begin(MyMac) == 0) // Start ethernet with DHCP
	{
		Serial.println(F("Failed to configure Ethernet using DHCP"));
	}
	else
	{
		Serial.println(F("Succesfuly configured with DHCP"));
		MyIP = Ethernet.localIP();
		Serial.println(MyIP);
		EEPROM.write(MyIP_START, 0); // Indicate that MyIP has not been written and we are using DHCP
		for (int i = MyIP_START + 1; i < MyIP_START + 5; i++) // Write IP gotten from DHCP to EEPROM because it might be needed anyway
		{
			EEPROM.write(i, MyIP[i - (MyIP_START + 1)]);
		}
	}
}
#endif
