#include "network_mqtt.h"
#include "fx60.h"

// Configuration
#define propName "myProp"
#define channelName "myChannel"
const static char channel[] PROGMEM = "/" propName "/" channelName;
const static char heartbeatChannel[] PROGMEM = "/" propName "/" channelName "/heartbeat";

#ifdef AUTO_FIND_MQTT_SERVER
EthernetUDP udp;
MDNS mdns(udp);
#endif

mqtt::mqtt(byte _MyMac[], IPAddress _MyIP, IPAddress _HostIP) : Network(_MyMac, _MyIP, _HostIP, true)
{
	#ifdef AUTO_FIND_MQTT_SERVER
	// Get HostIP from mDNS
	mdns.begin(Ethernet.localIP(), "arduino");
	mdns.setServiceFoundCallback(serviceFound);
	mdns.startDiscoveringService("_mqtt._tcp", MDNSServiceTCP, 5000);
	#endif

	client = new PubSubClient(ethClient);
	client->setServer(HostIP, serverPort);
	client->setCallback(mqtt::callback);
}

#ifdef AUTO_FIND_MQTT_SERVER
void mqtt::serviceFound(const char* type, MDNSServiceProtocol /*proto*/, const char* name, IPAddress ip, unsigned short port, const char* txtContent)
{
	mdns.stopDiscoveringService();
	client->setServer(ip, port);
	reconnect();
}
#endif

void mqtt::loop(void)
{
	Network::loop();
	if (!client->connected())
	{
		connected = false;
		if (retry_timer > RETRY_INTERVAL)
		{
			reconnect();
			retry_timer = 0;
		}
	}
	else
	{
		connected = true;
		sendChanges();
		client->loop();
	}
	#ifdef AUTO_FIND_MQTT_SERVER
	mdns.run();
	#endif
}

void mqtt::tick()
{
	heartbeat_timer += TIMER_INTERVAL;
	if (!connected) retry_timer += TIMER_INTERVAL;
}

void mqtt::printData(char* data, const char* channel)
{
	#ifdef DEBUG_MQTT
		Serial.print(F("Channel: "));
		Serial.print(channel);
	
		if (data[0])
		{
			Serial.print(F(" - Data: "));
			Serial.println(data);
		}
		else
		{
			Serial.println();
		}
	#endif
}

void mqtt::sendChanges(void)
{
	#define MQTT_BUF_SZ 128
	char data[MQTT_BUF_SZ];
	char tmp[3];

	const static char valueString[] PROGMEM = ", \"VALUE\": ";

	if (!pMyGame->solvedFlag && pMyGame->isSolved())
	{
		// Construct string
		const static char s1[] PROGMEM = "{\"DIRECTION\": \"FROM\", \"TYPE\": \"GAMESTATE\", \"VALUE\": ";
		strcpy_P(data, s1);
		strcat(data, pMyGame->_puzzleSolved == 1 ? "1" : "0");
		strcat(data, "}");

		// Publish and print
		client->publish(channel, data);
		printData(data, channel);

		// Update flag so it's not sent again
		pMyGame->solvedFlag = true;
	}

	for (int i = 0; i < NUM_INPUTS; i++)
	{
		if (pMyGame->INPUT_STATES[i] != pMyGame->INPUT_STATE_OLD[i] && pMyGame->INPUT_OVERRIDE_ENABLE[i]) // Loop through inputs and send state if changed
		{
			// Construct string
			const static char s1[] PROGMEM = "{\"DIRECTION\": \"FROM\", \"TYPE\": \"INPUT\", \"INDEX\": ";
			strcpy_P(data, s1);
			strcat(data, itoa(i, tmp, 10));
			strcat_P(data, valueString);
			strcat(data, pMyGame->INPUT_STATES[i] == 1 ? "1" : "0");
			strcat(data, "}");

			// Publish and print
			client->publish(channel, data);
			printData(data, channel);

			// Update state so it's not published again
			pMyGame->INPUT_STATE_OLD[i] = pMyGame->INPUT_STATES[i];
		}
	}

	for (int i = 0; i < NUM_OUTPUTS; i++)
	{
		if (pMyGame->OUTPUT_STATES[i] != pMyGame->OUTPUT_STATE_OLD[i] && pMyGame->OUTPUT_OVERRIDE_ENABLE[i])
		{
			// Construct string
			const static char s1[] PROGMEM = "{\"DIRECTION\": \"FROM\", \"TYPE\": \"OUTPUT\", \"INDEX\": ";
			strcpy_P(data, s1);
			strcat(data, itoa(i, tmp, 10));
			strcat_P(data, valueString);
			strcat(data, pMyGame->OUTPUT_STATES[i] == 1 ? "1" : "0");
			strcat(data, "}");

			// Publish and print
			client->publish(channel, data);
			printData(data, channel);

			// Update state so it's not published again
			pMyGame->OUTPUT_STATE_OLD[i] = pMyGame->OUTPUT_STATES[i];
		}
	}

	for (int i = 0; i < NUM_RELAYS; i++)
	{
		if (pMyGame->RELAY_STATES[i] != pMyGame->RELAY_STATE_OLD[i] && pMyGame->RELAY_OVERRIDE_ENABLE[i])
		{
			// Construct string
			const static char s1[] PROGMEM = "{\"DIRECTION\": \"FROM\", \"TYPE\": \"RELAY\", \"INDEX\": %d, \"VALUE\": ";
			strcpy_P(data, s1);
			strcat(data, itoa(i, tmp, 10));
			strcat_P(data, valueString);
			strcat(data, pMyGame->RELAY_STATES[i] == 1 ? "1" : "0");
			strcat(data, "}");

			// Publish and print
			client->publish(channel, data);
			printData(data, channel);

			// Update state so it's not published again
			pMyGame->RELAY_STATE_OLD[i] = pMyGame->RELAY_STATES[i];
		}
	}

#if defined(FX60_0_ENABLE)
	for (int i = 0; i < FX60_NUM_INPUTS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_0_INPUT_STATES & bitIndex) != (pMyGame->FX60_0_ISTATE_OLD & bitIndex) && (pMyGame->FX60_0_INPUT_ENABLE & bitIndex))
		{
			snprintf(data, MQTT_BUF_SZ, "{\"DIRECTION\": \"FROM\", \"TYPE\": \"FX60_0_INPUT\", \"INDEX\": %d, \"VALUE\": %d}", i, (pMyGame->FX60_0_INPUT_STATES & bitIndex) ? HIGH : LOW);
			client->publish(channel, data);
			printData(data, channel);
			pMyGame->FX60_0_ISTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_0_ISTATE_OLD |= pMyGame->FX60_0_INPUT_STATES & bitIndex; //set new bit
		}
	}

	for (int i = 0; i < FX60_NUM_OUTPUTS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_0_OUTPUT_STATES & bitIndex) != (pMyGame->FX60_0_OSTATE_OLD & bitIndex) && (pMyGame->FX60_0_OUTPUT_ENABLE & bitIndex))
		{
			snprintf(data, MQTT_BUF_SZ, "{\"DIRECTION\": \"FROM\", \"TYPE\": \"FX60_0_OUTPUT\", \"INDEX\": %d, \"VALUE\": %d}", i, (pMyGame->FX60_0_OUTPUT_STATES & bitIndex) ? HIGH : LOW);
			client->publish(channel, data);
			printData(data, channel);
			pMyGame->FX60_0_OSTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_0_OSTATE_OLD |= pMyGame->FX60_0_OUTPUT_STATES & bitIndex; //set new bit
		}
	}

	for (int i = 0; i < FX60_NUM_RELAYS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_0_RELAY_STATES & bitIndex) != (pMyGame->FX60_0_RSTATE_OLD & bitIndex) && (pMyGame->FX60_0_RELAY_ENABLE & bitIndex))
		{
			snprintf(data, MQTT_BUF_SZ, "{\"DIRECTION\": \"FROM\", \"TYPE\": \"FX60_0_RELAY\", \"INDEX\": %d, \"VALUE\": %d}", i, (pMyGame->FX60_0_RELAY_STATES & bitIndex) ? HIGH : LOW);
			client->publish(channel, data);
			printData(data, channel);
			pMyGame->FX60_0_RSTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_0_RSTATE_OLD |= pMyGame->FX60_0_RELAY_STATES & bitIndex; //set new bit
		}
	}
#endif

#if defined(FX60_1_ENABLE)
	for (int i = 0; i < FX60_NUM_INPUTS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_1_INPUT_STATES & bitIndex) != (pMyGame->FX60_1_ISTATE_OLD & bitIndex) && (pMyGame->FX60_1_INPUT_ENABLE & bitIndex))
		{
			snprintf(data, MQTT_BUF_SZ, "{\"DIRECTION\": \"FROM\", \"TYPE\": \"FX60_1_INPUT\", \"INDEX\": %d, \"VALUE\": %d}", i, (pMyGame->FX60_1_INPUT_STATES & bitIndex) ? HIGH : LOW);
			client->publish(channel, data);
			printData(data, channel);
			pMyGame->FX60_1_ISTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_1_ISTATE_OLD |= pMyGame->FX60_1_INPUT_STATES & bitIndex; //set new bit
		}
	}

	for (int i = 0; i < FX60_NUM_OUTPUTS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_1_OUTPUT_STATES & bitIndex) != (pMyGame->FX60_1_OSTATE_OLD & bitIndex) && (pMyGame->FX60_1_OUTPUT_ENABLE & bitIndex))
		{
			snprintf(data, MQTT_BUF_SZ, "{\"DIRECTION\": \"FROM\", \"TYPE\": \"FX60_1_OUTPUT\", \"INDEX\": %d, \"VALUE\": %d}", i, (pMyGame->FX60_1_OUTPUT_STATES & bitIndex) ? HIGH : LOW);
			client->publish(channel, data);
			printData(data, channel);
			pMyGame->FX60_1_OSTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_1_OSTATE_OLD |= pMyGame->FX60_1_OUTPUT_STATES & bitIndex; //set new bit
		}
	}

	for (int i = 0; i < FX60_NUM_RELAYS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_1_RELAY_STATES & bitIndex) != (pMyGame->FX60_1_RSTATE_OLD & bitIndex) && (pMyGame->FX60_1_RELAY_ENABLE & bitIndex))
		{
			snprintf(data, MQTT_BUF_SZ, "{\"DIRECTION\": \"FROM\", \"TYPE\": \"FX60_1_RELAY\", \"INDEX\": %d, \"VALUE\": %d}", i, (pMyGame->FX60_1_RELAY_STATES & bitIndex) ? HIGH : LOW);
			client->publish(channel, data);
			printData(data, channel);
			pMyGame->FX60_1_RSTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_1_RSTATE_OLD |= pMyGame->FX60_1_RELAY_STATES & bitIndex; //set new bit
		}
	}
#endif

	if (pMyGame->isRFIDChanged())
	{
		#define MQTT_TAG_BUF_SZ 50+RFID_TAG_COUNT*RFID_STR_LEN_MAX

		char tag_data[MQTT_TAG_BUF_SZ];
		pMyGame->getTagData(tag_data, MQTT_TAG_BUF_SZ);
		printData(tag_data, channel);
		client->publish(channel, tag_data);
	}

	// Broadcast heartbeat every 10 seconds to indicate that it is running
	if (heartbeat_timer >= HEARTBEAT_INTERVAL)
	{
		printData("", heartbeatChannel);
		client->publish(heartbeatChannel, propName);
		heartbeat_timer -= HEARTBEAT_INTERVAL;
	}
}

void mqtt::callback(char* topic, uint8_t* payload, unsigned int length)
{
	if (strcmp(topic, channel) == 0)
	{
		StaticJsonBuffer<512> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(payload);

		const char* dir = root["DIRECTION"];
		const char* type = root["TYPE"];
		if (strcmp(dir, "FROM") == 0) return; //from this prop, return

		if (strcmp(type, "OUTPUT") == 0) // Change output state
		{
			int OUTPUTnum = root["INDEX"];
			bool OUTPUTstate = root["VALUE"];

			pMyGame->OUTPUT_STATES[OUTPUTnum] = OUTPUTstate;
			pMyGame->OUTPUT_STATES_FLAG[OUTPUTnum] = true;
		}
		else if (strcmp(type, "RELAY") == 0) // Change relay state
		{
			int RELAYnum = root["INDEX"];
			bool RELAYstate = root["VALUE"];

			pMyGame->RELAY_STATES[RELAYnum] = RELAYstate;
			pMyGame->RELAY_STATES_FLAG[RELAYnum] = true;
		}
#ifdef FX60_0_ENABLE
		else if (strcmp(type, "FX60_0_RELAY") == 0)
		{
			int relayNum = root["INDEX"];
			int relayState = root["VALUE"];

			uint8_t bitIndex = 1 << relayNum;

			pMyGame->FX60_0_RELAY_STATES &= ~bitIndex;
			pMyGame->FX60_0_RELAY_STATES |= bitIndex * relayState;
			pMyGame->FX60_0_RELAY_STATES_FLAG |= bitIndex;
		}
		else if (strcmp(type, "FX60_0_OUTPUT") == 0)
		{
			int outputNum = root["INDEX"];
			int outputState = root["VALUE"];

			uint8_t bitIndex = 1 << outputNum;

			pMyGame->FX60_0_OUTPUT_STATES &= ~bitIndex;
			pMyGame->FX60_0_OUTPUT_STATES |= bitIndex * outputState;
			pMyGame->FX60_0_OUTPUT_STATES_FLAG |= bitIndex;
		}
#endif
#ifdef FX60_1_ENABLE
		else if (strcmp(type, "FX60_1_RELAY") == 0)
		{
			int relayNum = root["INDEX"];
			int relayState = root["VALUE"];

			uint8_t bitIndex = 1 << relayNum;

			pMyGame->FX60_1_RELAY_STATES &= ~bitIndex;
			pMyGame->FX60_1_RELAY_STATES |= bitIndex * relayState;
			pMyGame->FX60_1_RELAY_STATES_FLAG |= bitIndex;
		}
		else if (strcmp(type, "FX60_1_OUTPUT") == 0)
		{
			int outputNum = root["INDEX"];
			int outputState = root["VALUE"];

			uint8_t bitIndex = 1 << outputNum;

			pMyGame->FX60_1_OUTPUT_STATES &= ~bitIndex;
			pMyGame->FX60_1_OUTPUT_STATES |= bitIndex * outputState;
			pMyGame->FX60_1_OUTPUT_STATES_FLAG |= bitIndex;
		}
#endif
		else if (strcmp(type, "RESET") == 0)
		{
			pMyGame->reset();
		}
		else if (strcmp(type, "GAMESTATE") == 0)
		{
			bool solved = root["VALUE"];
			if (solved) pMyGame->forceSolved();
			else pMyGame->reset();
		}
	}
}

void mqtt::reconnect()
{
	#ifdef DEBUG_MQTT
		Serial.print(F("Attempting MQTT connection with device "));
		Serial.print(propName);
		Serial.print(F(" on channel "));
		Serial.println(channelName);
	#endif
	// Attempt to connect
	if (client->connect(propName))
	{
		#ifdef DEBUG_MQTT
			Serial.println(F("connected"));
		#endif
		client->subscribe(channel);
	}
	else
	{
		#ifdef DEBUG_MQTT
			Serial.print(F("failed, rc="));
			Serial.print(client->state());
			Serial.println(F(" try again in 5 seconds"));
		#endif
	}
}
