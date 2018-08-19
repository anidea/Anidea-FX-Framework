#include <PubSubClient.h>
#include "network_mqtt_m3.h"
#include "fx60.h"
#include <ArduinoJson.h>

// Configuration
char mqtt_m3::propName[] = "myProp";

const static char channel_command[] PROGMEM = "/Command";
const static char channel_enable[] PROGMEM = "/Command/Enable";
const static char channel_solve[] PROGMEM = "/Command/Solve";
const static char channel_reset[] PROGMEM = "/Command/Reset";
const static char channel_disable[] PROGMEM = "/Command/Disable";

const static char channel_heartbeat[] PROGMEM = "/heartbeat";

const static char channel_output[] PROGMEM = "/Output_";
const static char channel_input[] PROGMEM = "/Input_";
const static char channel_relay[] PROGMEM = "/Relay_";

#ifdef FX60_0_ENABLE
const static char channel_FX60_0_relay[] PROGMEM = propName "/Command/FX60_0_RELAY_";
const static char channel_FX60_0_output[] PROGMEM = propName "/Command/FX60_0_OUTPUT_";
#endif
#ifdef FX60_1_ENABLE
const static char channel_FX60_1_relay[] PROGMEM = propName "/Command/FX60_1_RELAY_";
const static char channel_FX60_1_output[] PROGMEM = propName "/Command/FX60_1_OUTPUT_";
#endif

#define DEBUG_MQTT_M3

mqtt_m3::mqtt_m3(byte _MyMac[], IPAddress _MyIP, IPAddress _HostIP) : Network(_MyMac, _MyIP, _HostIP, true)
{
	client = new PubSubClient();
	client->setClient(ethClient);
	client->setServer(HostIP, serverPort);
	client->setCallback(mqtt_m3::callback);
}

void mqtt_m3::loop(void)
{
	Network::loop();

	if (!client->connected())
	{
		netConnected = false;
		if (retry_timer > RETRY_INTERVAL)
		{
			reconnect();
			retry_timer = 0;
		}
	}
	else
	{
		netConnected = true;
		sendChanges();
		client->loop();
	}
}

void mqtt_m3::tick()
{
	heartbeat_timer += TIMER_INTERVAL;
	if (!netConnected) retry_timer += TIMER_INTERVAL;
}

void mqtt_m3::printData(char* data, char* channel)
{
#ifdef DEBUG_MQTT_M3
	Serial.print(F("Channel: "));
	Serial.print(channel);

	if (data[0])
	{
		Serial.print(F(" - Data: "));
		Serial.println(data);
	}
	else
	{
		Serial.println("");
	}
#endif
}

void mqtt_m3::sendChanges()
{
#define mqtt_m3_BUF_SZ 128
	char data[mqtt_m3_BUF_SZ];

	if (!pMyGame->solvedFlag && pMyGame->isSolved())
	{
		snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s", propName, channel_solve);

		client->publish(data, pMyGame->isSolved() ? "Yes" : "No");
		pMyGame->solvedFlag = true;
	}

	for (int i = 0; i < NUM_INPUTS; i++)
	{
		if (pMyGame->INPUT_STATES[i] != pMyGame->INPUT_STATE_OLD[i])
		{
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_input, pMyGame->INPUT_STATES[i]);

			client->publish(data, pMyGame->INPUT_STATES[i] ? "Yes" : "No");

			pMyGame->INPUT_STATE_OLD[i] = pMyGame->INPUT_STATES[i];
		}
	}

	for (int i = 0; i < NUM_OUTPUTS; i++)
	{
		if (pMyGame->OUTPUT_STATES[i] != pMyGame->OUTPUT_STATE_OLD[i])
		{
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_output, pMyGame->OUTPUT_STATES[i]);

			client->publish(data, pMyGame->OUTPUT_STATES[i] ? "Yes" : "No");

			pMyGame->OUTPUT_STATE_OLD[i] = pMyGame->OUTPUT_STATES[i];
		}
	}

	for (int i = 0; i < NUM_RELAYS; i++)
	{
		if (pMyGame->RELAY_STATES[i] != pMyGame->RELAY_STATE_OLD[i])
		{
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_relay, pMyGame->RELAY_STATES[i]);

			client->publish(data, pMyGame->RELAY_STATES[i] ? "Yes" : "No");

			pMyGame->RELAY_STATE_OLD[i] = pMyGame->RELAY_STATES[i];
		}
	}


#if defined(FX60_0_ENABLE)
	for (int i = 0; i < FX60_NUM_INPUTS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_0_INPUT_STATES & bitIndex) != (pMyGame->FX60_0_ISTATE_OLD & bitIndex) && (pMyGame->FX60_0_INPUT_ENABLE & bitIndex))
		{
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_FX60_0_relay, pMyGame->FX60_0_INPUT_STATES & bitIndex ? 1 : 0);

			client->publish(data, (pMyGame->FX60_0_INPUT_STATES & bitIndex) ? "Yes" : "No");

			// Update state so it's not published again
			pMyGame->FX60_0_ISTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_0_ISTATE_OLD |= pMyGame->FX60_0_INPUT_STATES & bitIndex; //set new bit
		}
	}

	for (int i = 0; i < FX60_NUM_OUTPUTS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_0_OUTPUT_STATES & bitIndex) != (pMyGame->FX60_0_OSTATE_OLD & bitIndex) && (pMyGame->FX60_0_OUTPUT_ENABLE & bitIndex))
		{
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_FX60_0_output, pMyGame->FX60_0_OUTPUT_STATES & bitIndex ? 1 : 0);

			client->publish(data, (pMyGame->FX60_0_OUTPUT_STATES & bitIndex) ? "Yes" : "No");

			// Update state so it's not published again
			pMyGame->FX60_0_OSTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_0_OSTATE_OLD |= pMyGame->FX60_0_OUTPUT_STATES & bitIndex; //set new bit
		}
	}

	for (int i = 0; i < FX60_NUM_RELAYS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_0_RELAY_STATES & bitIndex) != (pMyGame->FX60_0_RSTATE_OLD & bitIndex) && (pMyGame->FX60_0_RELAY_ENABLE & bitIndex))
		{
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_FX60_0_relay, pMyGame->FX60_0_RELAY_STATES & bitIndex ? 1 : 0);

			client->publish(data, (pMyGame->FX60_0_RELAY_STATES & bitIndex) ? "Yes" : "No");

			// Update state so it's not published again
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
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_FX60_1_relay, pMyGame->FX60_1_INPUT_STATES & bitIndex ? 1 : 0);

			client->publish(data, (pMyGame->FX60_1_INPUT_STATES & bitIndex) ? "Yes" : "No");

			// Update state so it's not published again
			pMyGame->FX60_1_ISTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_1_ISTATE_OLD |= pMyGame->FX60_1_INPUT_STATES & bitIndex; //set new bit
		}
	}

	for (int i = 0; i < FX60_NUM_OUTPUTS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_1_OUTPUT_STATES & bitIndex) != (pMyGame->FX60_1_OSTATE_OLD & bitIndex) && (pMyGame->FX60_1_OUTPUT_ENABLE & bitIndex))
		{
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_FX60_1_output, pMyGame->FX60_1_OUTPUT_STATES & bitIndex ? 1 : 0);

			client->publish(data, (pMyGame->FX60_1_OUTPUT_STATES & bitIndex) ? "Yes" : "No");


			// Update state so it's not published again
			pMyGame->FX60_1_OSTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_1_OSTATE_OLD |= pMyGame->FX60_1_OUTPUT_STATES & bitIndex; //set new bit
		}
	}

	for (int i = 0; i < FX60_NUM_RELAYS; i++)
	{
		uint8_t bitIndex = (1 << i);

		if ((pMyGame->FX60_1_RELAY_STATES & bitIndex) != (pMyGame->FX60_1_RSTATE_OLD & bitIndex) && (pMyGame->FX60_1_RELAY_ENABLE & bitIndex))
		{
			snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s%d", propName, channel_FX60_1_relay, pMyGame->FX60_1_RELAY_STATES & bitIndex ? 1 : 0);

			client->publish(data, (pMyGame->FX60_1_RELAY_STATES & bitIndex) ? "Yes" : "No");

			// Update state so it's not published again
			pMyGame->FX60_1_RSTATE_OLD &= ~bitIndex; //zero out current_bit
			pMyGame->FX60_1_RSTATE_OLD |= pMyGame->FX60_1_RELAY_STATES & bitIndex; //set new bit
		}
	}
#endif


	// Broadcast heartbeat every 10 seconds to indicate that it is running
	if (heartbeat_timer >= HEARTBEAT_INTERVAL)
	{
		snprintf(data, mqtt_m3_BUF_SZ, "/%s/%s", propName, channel_heartbeat);

		client->publish(data, "");

		heartbeat_timer -= HEARTBEAT_INTERVAL;
	}
}

void mqtt_m3::callback(char* topic, uint8_t* payload, unsigned int length)
{
	Serial.println(F("Callback"));
	Serial.println(topic);
	Serial.println((char*)payload);
	Serial.println(length);

	if (length == 0) return;

	String topicName = strtok(topic, "/");
	if (topicName != String(propName)) return;
	String topicCommand = strtok(NULL, "/");
	if ('/' + topicCommand != channel_command) return;
	String topicStripped = '/' + String(strtok(NULL, "/"));
	char* topicStrippedCstr = strdup(topicStripped.c_str());

	if (topicStripped == channel_enable) // Enable
	{
		pMyGame->enable();
	}
	else if (topicStripped == channel_solve) // Solved
	{
		pMyGame->forceSolved();
	}
	else if (topicStripped == channel_reset) // Solved
	{
		pMyGame->reset();
	}
	else if (topicStripped == channel_disable) // Solved
	{
		pMyGame->disable();
	}

	if (topicStripped.compareTo(channel_output) >= 15) // Digital outputs
	{
		strtok(topicStrippedCstr, "_");
		byte num = atoi(strtok(NULL, "_"));
		if (num > NUM_OUTPUTS) return;
		pMyGame->OUTPUT_STATES[num] = strncmp((char*)payload, "Yes", length) == 0;
		pMyGame->OUTPUT_STATES_FLAG[num] = true;
		return;
	}

	if (topicStripped.compareTo(channel_relay) >= 7) // Relays
	{
		strtok(topicStrippedCstr, "_");
		byte num = atoi(strtok(NULL, "_"));
		if (num > NUM_RELAYS) return;
		pMyGame->RELAY_STATES[num] = strncmp((char*)payload, "HIGH", length) == 0;
		pMyGame->RELAY_STATES_FLAG[num] = true;
		return;
	}

#ifdef FX60_0_ENABLE
	if (strcmp_P(topic, channel_FX60_0_relay) == 0)
	{
		strtok(topic, "_");
		strtok(NULL, "_");
		strtok(NULL, "_");
		byte relayNum = atoi(strtok(NULL, "_"));

		uint8_t bitIndex = 1 << relayNum;

		pMyGame->FX60_0_RELAY_STATES &= ~bitIndex;
		pMyGame->FX60_0_RELAY_STATES |= bitIndex * (strncmp(payload, "HIGH", length) == 0);
		pMyGame->FX60_0_RELAY_STATES_FLAG |= bitIndex;
		return;
	}
	if (strcmp_P(topic, channel_FX60_0_output) == 0)
	{
		strtok(topic, "_");
		strtok(NULL, "_");
		strtok(NULL, "_");
		byte outputNum = atoi(strtok(NULL, "_"));

		uint8_t bitIndex = 1 << outputNum;

		pMyGame->FX60_0_OUTPUT_STATES &= ~bitIndex;
		pMyGame->FX60_0_OUTPUT_STATES |= bitIndex * (strncmp(payload, "HIGH", length) == 0);
		pMyGame->FX60_0_OUTPUT_STATES_FLAG |= bitIndex;
		return;
	}
#endif
#ifdef FX60_1_ENABLE
	if (strcmp_P(topic, channel_FX60_1_relay) == 0)
	{
		strtok(topic, "_");
		strtok(NULL, "_");
		strtok(NULL, "_");
		byte relayNum = atoi(strtok(NULL, "_"));

		uint8_t bitIndex = 1 << relayNum;

		pMyGame->FX60_1_RELAY_STATES &= ~bitIndex;
		pMyGame->FX60_1_RELAY_STATES |= bitIndex * (strncmp(payload, "HIGH", length) == 0);
		pMyGame->FX60_1_RELAY_STATES_FLAG |= bitIndex;
		return;
	}
	if (strcmp_P(topic, channel_FX60_1_output) == 0)
	{
		strtok(topic, "_");
		strtok(NULL, "_");
		strtok(NULL, "_");
		byte outputNum = atoi(strtok(NULL, "_"));

		uint8_t bitIndex = 1 << outputNum;

		pMyGame->FX60_1_OUTPUT_STATES &= ~bitIndex;
		pMyGame->FX60_1_OUTPUT_STATES |= bitIndex * (strncmp(payload, "HIGH", length) == 0);
		pMyGame->FX60_1_OUTPUT_STATES_FLAG |= bitIndex;
		return;
	}
#endif
}

void mqtt_m3::reconnect()
{

#ifdef DEBUG_MQTT_M3
	Serial.print(F("Attempting MQTT connection with device "));
	Serial.println(propName);
#endif
	// Attempt to connect
	if (client->connect(propName))
	{
#ifdef DEBUG_MQTT_M3
		Serial.println(F("connected"));
#endif
		const int CHANNEL_BUF_SZ = 64;
		char channelBuffer[CHANNEL_BUF_SZ];
		char tmp[3];

		// Enable
		strcpy_P(channelBuffer, propName);
		strcat_P(channelBuffer, channel_enable);
		client->subscribe(channelBuffer);

		Serial.println(channelBuffer);

		// Solved
		strcpy_P(channelBuffer, propName);
		strcat_P(channelBuffer, channel_solve);
		client->subscribe(channelBuffer);

		// Solved
		strcpy_P(channelBuffer, propName);
		strcat_P(channelBuffer, channel_reset);
		client->subscribe(channelBuffer);

		Serial.println(channelBuffer);

		// Outputs
		for (int i = 0; i < NUM_OUTPUTS; i++)
		{
			strcpy_P(channelBuffer, propName);
			strcat_P(channelBuffer, channel_command);
			strcat_P(channelBuffer, channel_output);
			strcat(channelBuffer, itoa(i, tmp, 10));
			client->subscribe(channelBuffer);

			Serial.println(channelBuffer);
		}

		// Relays
		for (int i = 0; i < NUM_RELAYS; i++)
		{
			strcpy_P(channelBuffer, propName);
			strcat_P(channelBuffer, channel_command);
			strcat_P(channelBuffer, channel_relay);
			strcat(channelBuffer, itoa(i, tmp, 10));
			client->subscribe(channelBuffer);

			Serial.println(channelBuffer);
		}

		// Send a message on all topics

		// Command

		// Construct channel name/Command/Enable
		strcpy_P(channelBuffer, propName);
		strcat_P(channelBuffer, channel_enable);
		client->publish(channelBuffer, "");

		Serial.println(channelBuffer);

		// Construct channel name/Command/Solved
		strcpy_P(channelBuffer, propName);
		strcat_P(channelBuffer, channel_solve);
		client->publish(channelBuffer, "");

		Serial.println(channelBuffer);
		// Construct channel name/Command/digitalOutput_
		for (int i = 0; i < NUM_OUTPUTS; i++)
		{
			strcpy_P(channelBuffer, propName);
			strcat_P(channelBuffer, channel_output);
			strcat(channelBuffer, itoa(i, tmp, 10));
			client->publish(channelBuffer, "");

			Serial.println(channelBuffer);
		}

		// Construct channel name/Command/analogOutput_
		strcpy_P(channelBuffer, propName);
		strcat_P(channelBuffer, channel_output);
		client->publish(channelBuffer, "");

		Serial.println(channelBuffer);

		// Construct channel name/Command/Relay_
		for (int i = 0; i < NUM_RELAYS; i++)
		{
			strcpy_P(channelBuffer, propName);
			strcat_P(channelBuffer, channel_relay);
			strcat(channelBuffer, itoa(i, tmp, 10));
			client->publish(channelBuffer, "");

			Serial.println(channelBuffer);
		}

		// Properties

		// Construct channel name/Enable
		strcpy_P(channelBuffer, propName);
		strcat_P(channelBuffer, channel_enable);
		client->publish(channelBuffer, "");

		Serial.println(channelBuffer);

		for (int i = 0; i < NUM_INPUTS; i++)
		{
			strcpy_P(channelBuffer, propName);
			strcat_P(channelBuffer, channel_input);
			strcat(channelBuffer, itoa(i, tmp, 10));
			client->publish(channelBuffer, "");

			Serial.println(channelBuffer);
		}

		for (int i = 0; i < NUM_OUTPUTS; i++)
		{
			strcpy_P(channelBuffer, propName);
			strcat_P(channelBuffer, channel_output);
			strcat(channelBuffer, itoa(i, tmp, 10));
			client->publish(channelBuffer, "");

			Serial.println(channelBuffer);
		}

		for (int i = 0; i < NUM_RELAYS; i++)
		{
			strcpy_P(channelBuffer, propName);
			strcat_P(channelBuffer, channel_relay);
			strcat(channelBuffer, itoa(i, tmp, 10));
			client->publish(channelBuffer, "");

			Serial.println(channelBuffer);
		}

		strcpy_P(channelBuffer, propName);
		strcat_P(channelBuffer, channel_heartbeat);
		client->publish(channelBuffer, "");

		Serial.println(channelBuffer);
	}
	else
	{
#ifdef DEBUG_MQTT_M3
		Serial.print(F("failed, rc="));
		Serial.print(client->state());
		Serial.println(F(" try again in 5 seconds"));
#endif

	}
}
