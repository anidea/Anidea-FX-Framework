#include <PubSubClient.h>
#include "network_mqtt_m3.h"

#if defined(FX60_0_ENABLE) || defined(FX60_1_ENABLE)
#include "fx60.h"
#endif
//#include <ArduinoJson.h>

#ifdef FX350
#define FX350MQTTPROGMEM
#else
#define FX350MQTTPROGMEM PROGMEM
#endif

// Configuration
char mqtt_m3::propName[] = "myProp";
bool mqtt_m3::recon = false;
const static char channel_command[] FX350MQTTPROGMEM = "set";
const static char channel_enable[] FX350MQTTPROGMEM = "enable";
const static char channel_solve[] FX350MQTTPROGMEM = "solve";
const static char channel_reset[] FX350MQTTPROGMEM = "reset";
const static char channel_disable[] FX350MQTTPROGMEM = "disable";
const static char channel_learn[] FX350MQTTPROGMEM = "learn";

const static char channel_heartbeat[] FX350MQTTPROGMEM = "heartbeat";

const static char channel_output[] FX350MQTTPROGMEM = "output";
const static char channel_input[] FX350MQTTPROGMEM = "input";
const static char channel_relay[] FX350MQTTPROGMEM = "relay";

#ifdef FX60_0_ENABLE
const static char channel_FX60_0[] FX350MQTTPROGMEM = propName "fx60_0";
#endif
#ifdef FX60_1_ENABLE
const static char channel_FX60_1[] FX350MQTTPROGMEM = propName "fx60_1";
#endif

#define DEBUG_MQTT

/*
#if defined(FX350)  
#define mqtt_m3_BUF_SZ 128
	// NOTE **** change MQTT_MAX_PACKET_SIZE in PubSubClient.h
#else*/
#define mqtt_m3_BUF_SZ 128
/*#endif*/

mqtt_m3::mqtt_m3(byte _MyMac[], IPAddress _MyIP, IPAddress _HostIP) : Network(_MyMac, _MyIP, _HostIP, true)
{
	client = new PubSubClient();
	client->setClient(ethClient);
	client->setServer(HostIP, serverPort);
	client->setCallback(mqtt_m3::callback);

	reconnect();
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

  //Serial.println(F("Tick"));
}

void mqtt_m3::printData(char* data, char* channel)
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
    Serial.println("");
  }
#endif
}

void mqtt_m3::sendChanges()
{


  char data[mqtt_m3_BUF_SZ];
  
  if (!pMyGame->solvedFlag)
  {
    snprintf(data, mqtt_m3_BUF_SZ, "%s/%s", propName, channel_solve);
	
	client->publish(data, pMyGame->isSolved() ? "Yes" : "No");
    
	pMyGame->solvedFlag = true;

	printData(data, NULL);
  }
  
  for (int i = 0; i < NUM_INPUTS; i++)
  {
    if (pMyGame->INPUT_STATES[i] != pMyGame->INPUT_STATE_OLD[i])
    {
      snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_input, i);

      client->publish(data, pMyGame->INPUT_STATES[i]? "Yes" : "No");
	  
      pMyGame->INPUT_STATE_OLD[i] = pMyGame->INPUT_STATES[i];

	  printData(data, NULL);
    }
  }

  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (pMyGame->OUTPUT_STATES[i] != pMyGame->OUTPUT_STATE_OLD[i])
    {
      snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_output, i);

      client->publish(data, pMyGame->OUTPUT_STATES[i]? "Yes" : "No");

      pMyGame->OUTPUT_STATE_OLD[i] = pMyGame->OUTPUT_STATES[i];

	  printData(data, NULL);
    }
  }

  for (int i = 0; i < NUM_RELAYS; i++)
  {
    if (pMyGame->RELAY_STATES[i] != pMyGame->RELAY_STATE_OLD[i])
    {
      snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_relay, i);
      
      client->publish(data, pMyGame->RELAY_STATES[i]? "Yes" : "No");

      pMyGame->RELAY_STATE_OLD[i] = pMyGame->RELAY_STATES[i];

	  printData(data, NULL);
    }
  }


#if defined(FX60_0_ENABLE)
  for (int i = 0; i < FX60_NUM_INPUTS; i++)
  {
    uint8_t bitIndex = (1 << i);

    if ((pMyGame->FX60_0_INPUT_STATES & bitIndex) != (pMyGame->FX60_0_ISTATE_OLD & bitIndex) && (pMyGame->FX60_0_INPUT_ENABLE & bitIndex))
    {
      snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_FX60_0_relay, pMyGame->FX60_0_INPUT_STATES & bitIndex ? 1 : 0);

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
      snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_FX60_0_output, pMyGame->FX60_0_OUTPUT_STATES & bitIndex ? 1 : 0);

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
      snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_FX60_0_relay, pMyGame->FX60_0_RELAY_STATES & bitIndex ? 1 : 0);

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
       snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_FX60_1_relay, pMyGame->FX60_1_INPUT_STATES & bitIndex ? 1 : 0);

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
       snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_FX60_1_output, pMyGame->FX60_1_OUTPUT_STATES & bitIndex ? 1 : 0);

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
      snprintf(data, mqtt_m3_BUF_SZ, "%s/%s%d", propName, channel_FX60_1_relay, pMyGame->FX60_1_RELAY_STATES & bitIndex ? 1 : 0);

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
    snprintf(data, mqtt_m3_BUF_SZ, "%s/%s", propName, channel_heartbeat);

    client->publish(data, "");

    heartbeat_timer -= HEARTBEAT_INTERVAL;
  }
}

void mqtt_m3::callback(char* topic, uint8_t* payload, unsigned int length)
{
	if (recon) return;
#ifdef DEBUG_MQTT
  Serial.println(F("Callback"));
  Serial.println(topic);
  for (int i = 0; i < length; i++)
  {
	  Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println(length);
#endif
  if (!length) return;

  auto getValue = [&]() -> bool
  {
	  String p;
	  for (int i = 0; i < length; i++)
	  {
		  p += (char) payload[i];
	  }
	  p.toLowerCase();

	  return (p == F("high") ||
			  p == F("yes") ||
			  p == F("on") ||
			  p == F("one") ||
			  p == F("1") ||
			  p == F("true"));
  };

  String topicName = strtok(topic, "/");
  //Serial.println(topicName);
  if (topicName !=  String(propName)) return;
  String topicCommand = strtok(NULL, "/");
  //Serial.println(topicCommand);
  if (topicCommand != channel_command) return;
  String topicStripped = String(strtok(NULL, "/"));
  //Serial.println(topicStripped);
  char* topicStrippedCstr = strdup(topicStripped.c_str());

  if (topicStripped == channel_learn) // Learn
  {
    pMyGame->learn();
  }
  else if (topicStripped == channel_enable) // Enable
  {
	  pMyGame->enable();
  }
  else if (topicStripped == channel_solve) // Solved
  {
    pMyGame->solved();
  }
  else if (topicStripped == channel_reset) // Solved
  {
    pMyGame->reset();
  }
  else if (topicStripped == channel_disable) // Solved
  {
    pMyGame->disable();
  }

  if (topicStripped.startsWith(channel_output)) // Digital outputs
  {
	  uint8_t index = strlen(channel_output);
	  if (index >= topicStripped.length()) return;

	  byte num = atol(&topicStripped[index]);
    if (num > NUM_OUTPUTS) return;
	pMyGame->OUTPUT_STATES[num] = getValue();
    pMyGame->OUTPUT_STATES_FLAG[num] = true;
    return;
  }

  if (topicStripped.startsWith(channel_relay)) // Relays
  {
	  uint8_t index = strlen(channel_relay);
	  if (index >= topicStripped.length()) return;

	  byte num = atol(&topicStripped[index]);
    if (num > NUM_RELAYS) return;
    pMyGame->RELAY_STATES[num] = getValue();
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
	recon = true;
#ifdef DEBUG_MQTT
  Serial.print(F("Attempting MQTT connection with device "));
  Serial.println( propName);
#endif
  // Attempt to connect
  if (client->connect( propName))
  {
#ifdef DEBUG_MQTT
    Serial.println(F("connected"));
#endif
    const int CHANNEL_BUF_SZ = mqtt_m3_BUF_SZ;
    char channelBuffer[CHANNEL_BUF_SZ];
    char tmp[3];

	// Learn
	snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s/%s", propName, channel_command, channel_learn);
	client->subscribe(channelBuffer);
	client->publish(channelBuffer, "");

    // Enable
	snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s/%s", propName, channel_command, channel_enable);
	client->subscribe(channelBuffer);
	client->publish(channelBuffer, "");

	snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s", propName, channel_enable);
	client->publish(channelBuffer, "");

    // Solved
	snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s/%s", propName, channel_command, channel_solve);
	client->subscribe(channelBuffer);
	client->publish(channelBuffer, "");

	snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s", propName, channel_solve);
	client->publish(channelBuffer, "");

    // Reset
	snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s/%s", propName, channel_command, channel_reset);
	client->subscribe(channelBuffer);
	client->publish(channelBuffer, "");

	snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s", propName, channel_reset);
	client->publish(channelBuffer, "");

	// Inputs
	for (int i = 0; i < NUM_INPUTS; i++)
	{
		snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s/%s%d", propName, channel_command, channel_input, i);
		client->subscribe(channelBuffer);
		client->publish(channelBuffer, "");

		snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s%d", propName, channel_input, i);
		client->publish(channelBuffer, "");
	}

    // Outputs
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
	  snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s/%s%d", propName, channel_command, channel_output, i);
	  client->subscribe(channelBuffer);
	  client->publish(channelBuffer, "");

	  snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s%d", propName, channel_output, i);
	  client->publish(channelBuffer, "");
    }

    // Relays
    for (int i = 0; i < NUM_RELAYS; i++)
    {
		snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s/%s%d", propName, channel_command, channel_relay, i);
		client->subscribe(channelBuffer);
		client->publish(channelBuffer, "");

		snprintf(channelBuffer, CHANNEL_BUF_SZ, "%s/%s%d", propName, channel_relay, i);
		client->publish(channelBuffer, "");
    }
  }
  else
  {
#ifdef DEBUG_MQTT
    Serial.print(F("failed, rc="));
    Serial.print(client->state());
    Serial.println(F(" try again in 5 seconds"));
#endif

  }

  recon = false;
}
