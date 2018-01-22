#include "network.h"
#include "network_mqtt.h"

byte mqtt::learnResponse = -1;
byte mqtt::learnResponseOld = -1;

mqtt::mqtt(byte _MyMac[], IPAddress _MyIP, IPAddress _HostIP) : Network(_MyMac, _MyIP, _HostIP, true)
{
  client.setClient(ethClient);
  client.setServer(HostIP, serverPort);
  client.setCallback(mqtt::callback);
  sprintf(propName, "Prop%d", MyIP[3]);
}

void mqtt::loop(void)
{
  Network::loop();
  if (!client.connected()) {
    reconnect();
  }
  sendChanges();
  client.loop();
}

void mqtt::sendChanges(void)
{
  #define MQTT_BUF_SZ 64
  char data[MQTT_BUF_SZ];

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
  bool changedFlag = false;
  pMyGame->getTagStates(tagStates, changedFlag);
  if (changedFlag)
  {
    snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"TAG\"");
    for (int j = 0; j < len; j++)
    {
      snprintf(data + strlen(data), MQTT_BUF_SZ, ", %d: %d", j, tagStates[j]);
    }
    snprintf(data + strlen(data), MQTT_BUF_SZ, "}");
    Serial.println(data);
    client.publish(propName, data);
    changedFlag = false;
  }

  if (learnResponse != learnResponseOld)
  {
    snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"LEARN\", \"DIRECTION\": \"FROM\", \"STATUS\": %c}", learnResponse);
    client.publish(propName, data);
    learnResponseOld = learnResponse;
  }
}

void mqtt::callback(char* topic, byte* payload, unsigned int length) {
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

void mqtt::reconnect() {
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
