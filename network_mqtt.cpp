#include "network.h"
#include "network_mqtt.h"

mqtt::mqtt(byte MyMac[], IPAddress MyIP, IPAddress HostIP) : Network(MyMac, MyIP, HostIP)
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
  if (pMyGame->_puzzleSolved == 1 && sent == 0) // Send puzzle solved
  {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["TYPE"] = "GAMESTATE";
    root["DIRECTION"] = "FROM";
    root["SOLVED"] = 1;
    char data[64];
    root.printTo(data);
    Serial.println(data);
    client.publish(propName, data);
    sent = 1;
  }
  else if (pMyGame->_puzzleSolved == 0 && sent == 1) // Send puzzle not solved
  {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["TYPE"] = "GAMESTATE";
    root["DIRECTION"] = "FROM";
    root["SOLVED"] = 0;
    char data[64];
    root.printTo(data);
    Serial.println(data);
    client.publish(propName, data);
    sent = 0;
  }

  for (int i = 0; i < NUM_INPUTS; i++)
  {
    if (INPUT_STATES[i] != INPUT_STATE_OLD[i] && pMyGame->INPUT_OVERRIDE_ENABLE[i] == 1) // Loop through inputs and send state if changed
    {
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      root["TYPE"] = "INPUT";
      root["INPUT"] = i;
      root["STATE"] = INPUT_STATES[i];
      char data[64];
      root.printTo(data);
      Serial.println(data);
      client.publish(propName, data);
      INPUT_STATE_OLD[i] = INPUT_STATES[i];
    }
  }
}

void mqtt::callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonBuffer<200> jsonBuffer;
  
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  JsonObject& root = jsonBuffer.parseObject(payload);

  const char* type = root["TYPE"];
  if (strcmp(type, "OUTPUT") == 0) // Change output state
  {
    int OUTPUTnum = root["OUTPUT"];
    bool OUTPUTstate = root["VALUE"];
    Serial.print("Setting output ");
    Serial.print(OUTPUTnum);
    Serial.print(" to ");
    Serial.println(OUTPUTstate);
    OUTPUT_STATES[OUTPUTnum] = OUTPUTstate;
  }
  else if (strcmp(type, "RELAY") == 0) // Change relay state
  {
    int RELAYnum = root["RELAY"];
    bool RELAYstate = root["VALUE"];
    RELAY_STATES[RELAYnum] = RELAYstate;
  }
  else if (strcmp(type, "GAMESTATE") == 0) // Change gamestate
  {
    Serial.println("GAMESTATE");
    const char* dir = root["DIRECTION"];
    if (strcmp(dir, "TO") == 0) // If it is not coming from the same prop
    {
      Serial.println("GAMESTATE TO");
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
}

void mqtt::reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect(propName)) {
      Serial.println(F("connected"));
      // Once connected, publish an announcement...
      client.publish(propName,"hello world");
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
