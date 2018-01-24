#include "network.h"
#include "network_mqtt_adv.h"

char mqtt_adv::propName[] = "bottleCap1";
char mqtt_adv::channelName[] = "OTC";

byte mqtt_adv::learnResponse = -1;
byte mqtt_adv::learnResponseOld = -1;

mqtt_adv::mqtt_adv(byte _MyMac[], IPAddress _MyIP, IPAddress _HostIP) : Network(_MyMac, _MyIP, _HostIP, true)
{
  client.setClient(ethClient);
  client.setServer(HostIP, serverPort);
  client.setCallback(mqtt_adv::callback);
}

void mqtt_adv::loop(void)
{
  Network::loop();
  if (!client.connected()) {
    reconnect();
  }
  sendChanges();
  client.loop();
}

void mqtt_adv::tick(void)
{
  tagTimer++;
  heartbeatTimer++;
}

void mqtt_adv::sendChanges(void)
{
  rfid *rfidGame = static_cast<rfid*>(pMyGame);
  #define MQTT_BUF_SZ 64
  char data[MQTT_BUF_SZ];
  char channel[MQTT_BUF_SZ];

  const char *c = "{\"TYPE\": \"GAMESTATE\", \"DIRECTION\": \"FROM\", \"SOLVED\": %c}";
  
  if (pMyGame->_puzzleSolved == 1 && sent == 0) // Send puzzle solved
  {
    snprintf(data, MQTT_BUF_SZ, c, '1');
    client.publish(propName, data);
    sent = 1;
  }
  else if (pMyGame->_puzzleSolved == 0 && sent == 1) // Send puzzle not solved
  {
    snprintf(data, MQTT_BUF_SZ, c, '0');
    client.publish(propName, data);
    sent = 0;
  }

  for (int i = 0; i < NUM_INPUTS; i++)
  {
    if (pMyGame->INPUT_STATES[i] != INPUT_STATE_OLD[i] && pMyGame->INPUT_OVERRIDE_ENABLE[i] == 1) // Loop through inputs and send state if changed
    {
      snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"INPUT\", \"INPUT\": %d, \"STATE\": %d}", i, pMyGame->INPUT_STATES[i]);
      client.publish(propName, data);
      INPUT_STATE_OLD[i] = pMyGame->INPUT_STATES[i];
    }
  }

  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (pMyGame->OUTPUT_STATES[i] != OUTPUT_STATE_OLD[i]) 
    {
      snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"OUTPUT\", \"OUTPUT\": %d, \"STATE\": %d}", i, pMyGame->OUTPUT_STATES[i]);
      client.publish(propName, data);
      OUTPUT_STATE_OLD[i] = pMyGame->OUTPUT_STATES[i];
    }
  }

  for (int i = 0; i < NUM_RELAYS; i++)
  {
    if (pMyGame->RELAY_STATES[i] != RELAY_STATE_OLD[i]) 
    {
      snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"RELAY\", \"RELAY\": %d, \"STATE\": %d}", i, pMyGame->RELAY_STATES[i]);
      client.publish(propName, data);
      RELAY_STATE_OLD[i] = pMyGame->RELAY_STATES[i];
    }
  }

  byte len = pMyGame->getLen();
  byte tagStates[len];
  bool stateFlag = false;
  pMyGame->getTagStates(tagStates, stateFlag);
  if (stateFlag)
  {
    snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"TAG_STATES\"");
    for (int j = 0; j < len; j++)
    {
      snprintf(data + strlen(data), MQTT_BUF_SZ, ", %d: %d", j, tagStates[j]);
    }
    snprintf(data + strlen(data), MQTT_BUF_SZ, "}");
    Serial.println(data);
    client.publish(propName, data);
    stateFlag = false;
  }

  if (learnResponse != learnResponseOld)
  {
    snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"LEARN\", \"DIRECTION\": \"FROM\", \"STATUS\": %c}", learnResponse);
    client.publish(propName, data);
    learnResponseOld = learnResponse;
  }

  if (tagTimer >= 10)
  {
    for (int i = 0; i < len; i++)
    {
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s/RFID/%d", channelName, propName, i);
      snprintf(data, MQTT_BUF_SZ, "%d", rfidGame->tagValues[i]);
      Serial.print("Channel: ");
      Serial.print(channel);
      Serial.print(" - Data: ");
      Serial.println(data);
      client.publish(channel, data);
    }
  }

  // Broadcast heartbeat every 10 seconds to indicate that it is running
  if (heartbeatTimer >= 100)
  {
    snprintf(channel, MQTT_BUF_SZ, "/%s/heartbeat", channelName);
    Serial.print("Channel: ");
    Serial.print(channel);
    Serial.print(" - Data: ");
    Serial.println(propName);
    client.publish(channel, propName);
    heartbeatTimer = 0;
  }

  if (rfidGame->buttonState != rfidGame->buttonStateOld)
  {
    snprintf(channel, MQTT_BUF_SZ, "/%s/%s/buttons/state", channelName, propName);
    snprintf(data, MQTT_BUF_SZ, "%d", rfidGame->buttonState);
    Serial.print("Channel: ");
    Serial.print(channel);
    Serial.print(" - Data: ");
    Serial.println(data);
    client.publish(channel, data);
    rfidGame->buttonStateOld = rfidGame->buttonState;
  }
}

void mqtt_adv::callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonBuffer<512> jsonBuffer;
  
//  Serial.print(F("Message arrived ["));
//  Serial.print(topic);
//  Serial.print("] ");
//  for (int i=0;i<length;i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();

  JsonObject& root = jsonBuffer.parseObject(payload);

  const char* type = root["TYPE"];
  if (strcmp(type, "OUTPUT") == 0) // Change output state
  {
    int OUTPUTnum = root["OUTPUT"];
    bool OUTPUTstate = root["VALUE"];
//    Serial.print("Setting output ");
//    Serial.print(OUTPUTnum);
//    Serial.print(" to ");
//    Serial.println(OUTPUTstate);
    pMyGame->OUTPUT_STATES[OUTPUTnum] = OUTPUTstate;
    pMyGame->OUTPUT_STATES_FLAG[OUTPUTnum] = true;
  }
  else if (strcmp(type, "RELAY") == 0) // Change relay state
  {
    int RELAYnum = root["RELAY"];
    bool RELAYstate = root["VALUE"];
    pMyGame->RELAY_STATES[RELAYnum] = RELAYstate;
    pMyGame->RELAY_STATES_FLAG[RELAYnum] = true;
  }
  else if (strcmp(type, "GAMESTATE") == 0) // Change gamestate
  {
//    Serial.println("GAMESTATE");
    const char* dir = root["DIRECTION"];
    if (strcmp(dir, "TO") == 0) // If it is not coming from the same prop
    {
//      Serial.println("GAMESTATE TO");
      bool state = root["VALUE"];
      if (state == 1)
      {
        pMyGame->forceSolved();
      }
      else if (state == 0)
      {
        pMyGame->reset();
      }
    }
  }
  else if (strcmp(type, "ENABLE") == 0) // Change enable
  {
//    Serial.println("ENABLE");
    const char* dir = root["DIRECTION"];
    if (strcmp(dir, "TO") == 0) // If it is not coming from the same prop
    {
//      Serial.println("ENABLE TO");
      bool state = root["VALUE"];
      if (state == 1)
      {
        pMyGame->enable();
      }
      else if (state == 0)
      {
        pMyGame->disable();
      }
      else if (state == 2)
      {
        pMyGame->reset();
        pMyGame->enable();
      }
      else if (state == 3)
      {
        pMyGame->reset();
        pMyGame->disable();
      }
    }
  }
  else if (strcmp(type, "LEARN") == 0) // Learn new sequence
  {
//    Serial.println("LEARN");
    const char* dir = root["DIRECTION"];
    if (strcmp(dir, "TO") == 0) // If it is not coming from the same prop
    {
      learnResponse = pMyGame->learn();
    }
  }
}

void mqtt_adv::reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect(propName)) {
      Serial.println(F("connected"));
      // Once connected, publish an announcement...
//      client.publish(propName,"hello world");
      // ... and resubscribe
      client.subscribe(propName);
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
