#include "network.h"
#include "network_mqtt_adv.h"

// Configuration
char mqtt_adv::propName[] = "testName";
char mqtt_adv::channelName[] = "testChannel";

// Send options
//#define MQTT_TAGSTATES_SEND
//#define MQTT_TAGS_SEND
//#define MQTT_HEARTBEAT_SEND
//#define MQTT_BUTTONS_SEND
//#define MQTT_KEYPAD_SEND

// Receive options
//#define MQTT_VIDEO_RECEIVE
//#define MQTT_LED_RECEIVE
//#define MQTT_AUDIO_RECEIVE
//#define MQTT_LOCK_RECEIVE
//#define MQTT_LIGHT_RECEIVE
//#define MQTT_SCROLLINGLIGHT_RECEIVE
//#define MQTT_TEXT_RECEIVE

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
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s", channelName, propName);
  
  if (pMyGame->_puzzleSolved == 1 && sent == 0) // Send puzzle solved
  {
    snprintf(data, MQTT_BUF_SZ, c, '1');
    Serial.print("Channel: ");
    Serial.print(channel);
    Serial.print(" - Data: ");
    Serial.println(data);
    client.publish(channel, data);
    sent = 1;
  }
  else if (pMyGame->_puzzleSolved == 0 && sent == 1) // Send puzzle not solved
  {
    snprintf(data, MQTT_BUF_SZ, c, '0');
    Serial.print("Channel: ");
    Serial.print(channel);
    Serial.print(" - Data: ");
    Serial.println(data);
    client.publish(channel, data);
    sent = 0;
  }

  for (int i = 0; i < NUM_INPUTS; i++)
  {
    if (pMyGame->INPUT_STATES[i] != INPUT_STATE_OLD[i] && pMyGame->INPUT_OVERRIDE_ENABLE[i] == 1) // Loop through inputs and send state if changed
    {
      snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"INPUT\", \"INPUT\": %d, \"STATE\": %d}", i, pMyGame->INPUT_STATES[i]);
      client.publish(channel, data);
      Serial.print("Channel: ");
      Serial.print(channel);
      Serial.print(" - Data: ");
      Serial.println(data);
      INPUT_STATE_OLD[i] = pMyGame->INPUT_STATES[i];
    }
  }

  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (pMyGame->OUTPUT_STATES[i] != OUTPUT_STATE_OLD[i]) 
    {
      snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"OUTPUT\", \"OUTPUT\": %d, \"STATE\": %d}", i, pMyGame->OUTPUT_STATES[i]);
      client.publish(channel, data);
      Serial.print("Channel: ");
      Serial.print(channel);
      Serial.print(" - Data: ");
      Serial.println(data);
      OUTPUT_STATE_OLD[i] = pMyGame->OUTPUT_STATES[i];
    }
  }

  for (int i = 0; i < NUM_RELAYS; i++)
  {
    if (pMyGame->RELAY_STATES[i] != RELAY_STATE_OLD[i]) 
    {
      snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"RELAY\", \"RELAY\": %d, \"STATE\": %d}", i, pMyGame->RELAY_STATES[i]);
      client.publish(channel, data);
      Serial.print("Channel: ");
      Serial.print(channel);
      Serial.print(" - Data: ");
      Serial.println(data);
      RELAY_STATE_OLD[i] = pMyGame->RELAY_STATES[i];
    }
  }

  byte len = pMyGame->getLen();
  byte tagStates[len];
  bool stateFlag = false;

  #if defined(MQTT_TAGSTATES_SEND)
  pMyGame->getTagStates(tagStates, stateFlag);
  if (stateFlag)
  {
    snprintf(channel, MQTT_BUF_SZ, "/%s/%s", channelName, propName);
    snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"TAG_STATES\"");
    for (int j = 0; j < len; j++)
    {
      snprintf(data + strlen(data), MQTT_BUF_SZ, ", %d: %d", j, tagStates[j]);
    }
    snprintf(data + strlen(data), MQTT_BUF_SZ, "}");
    Serial.print("Channel: ");
    Serial.print(channel);
    Serial.print(" - Data: ");
    Serial.println(data);
    client.publish(propName, data);
    stateFlag = false;
  }
  #endif

  if (learnResponse != learnResponseOld)
  {
    snprintf(channel, MQTT_BUF_SZ, "/%s/%s", channelName, propName);
    snprintf(data, MQTT_BUF_SZ, "{\"TYPE\": \"LEARN\", \"DIRECTION\": \"FROM\", \"STATUS\": %c}", learnResponse);
    Serial.print("Channel: ");
    Serial.print(channel);
    Serial.print(" - Data: ");
    Serial.println(data);
    client.publish(channel, data);
    learnResponseOld = learnResponse;
  }

  #if defined(MQTT_TAGS_SEND)
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
  #endif

  // Broadcast heartbeat every 10 seconds to indicate that it is running
  #if defined(MQTT_HEARTBEAT_SEND)
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
  #endif

  #if defined(MQTT_BUTTONS_SEND)
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
  #endif

  #if defined(MQTT_KEYPAD_SEND)
  if (rfidGame->keypadCodeFlag == true)
  {
    snprintf(channel, MQTT_BUF_SZ, "/%s/%s/code", channelName, propName);
    snprintf(data, MQTT_BUF_SZ, "");
    for (int i = 0; i < rfidGame->keypadCodeLength; i++)
    {
      snprintf(data + strlen(data), MQTT_BUF_SZ, "%d", rfidGame->keypadCode[i]);
    }
    Serial.print("Channel: ");
    Serial.print(channel);
    Serial.print(" - Data: ");
    Serial.println(data);
    client.publish(channel, data);
    rfidGame->keypadCodeFlag = false;
  }
  #endif
}

void mqtt_adv::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  rfid *rfidGame = static_cast<rfid*>(pMyGame);
  char channel[MQTT_BUF_SZ];
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s", channelName, propName);
  if (strcmp(topic, channel) == 0)
  {
    StaticJsonBuffer<512> jsonBuffer;
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
  #if defined(MQTT_VIDEO_RECEIVE)
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s/video/play", channelName, propName);
  if (strcmp(topic, channel) == 0)
  {
    payload[length] = '\0';
    rfidGame->playVideo(atoi((const char *)payload));
  }
  #endif
  #if defined(MQTT_LED_RECEIVE)
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s/buttons/led", channelName, propName);
  if (strcmp(topic, channel) == 0)
  {
    payload[length] = '\0';
    byte index = 0;
    bool value = 0;
    sscanf((const char *)payload, "%d:%d", &index, &value);
    rfidGame->setLedState(index, value);
  }
  #endif
  #if defined(MQTT_AUDIO_RECEIVE)
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s/audio/play", channelName, propName);
  if (strcmp(topic, channel) == 0)
  {
    payload[length] = '\0';
    rfidGame->playAudio(atoi((const char *)payload));
  }
  #endif
  #if defined(MQTT_LOCK_RECEIVE)
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s/lock", channelName, propName);
  if (strcmp(topic, channel) == 0)
  {
    payload[length] = '\0';
    rfidGame->setLock(atoi((const char *)payload));
  }
  #endif
  #if defined(MQTT_LIGHT_RECEIVE)
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s/light", channelName, propName);
  if (strcmp(topic, channel) == 0)
  {
    payload[length] = '\0';
    rfidGame->setLight(atoi((const char *)payload));
  }
  #endif
  #if defined(MQTT_SCROLLINGLIGHT_RECEIVE)
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s/scrollingLight", channelName, propName);
  if (strcmp(topic, channel) == 0)
  {
    payload[length] = '\0';
    rfidGame->setScrollingLight(atoi((const char *)payload));
  }
  #endif
  #if defined(MQTT_TEXT_RECEIVE)
  snprintf(channel, MQTT_BUF_SZ, "/%s/%s/text", channelName, propName);
  if (strcmp(topic, channel) == 0)
  {
    payload[length] = '\0';
    if (strcmp((const char *)payload, "NULL") == 0)
    {
      rfidGame->setDisplay(0, "");
    }
    else
    {
      rfidGame->setDisplay(1, (const char *)payload);
    }
  }
  #endif
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
      char channel[MQTT_BUF_SZ];
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s", channelName, propName);
      client.subscribe(channel);
      #if defined(MQTT_VIDEO_RECEIVE)
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s/video/play", channelName, propName);
      client.subscribe(channel);
      #endif
      #if defined(MQTT_LED_RECEIVE)
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s/buttons/led", channelName, propName);
      client.subscribe(channel);
      #endif
      #if defined(MQTT_AUDIO_RECEIVE)
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s/audio/play", channelName, propName);
      client.subscribe(channel);
      #endif
      #if defined(MQTT_LOCK_RECEIVE)
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s/lock", channelName, propName);
      client.subscribe(channel);
      #endif
      #if defined(MQTT_LIGHT_RECEIVE)
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s/light", channelName, propName);
      client.subscribe(channel);
      #endif
      #if defined(MQTT_SCROLLINGLIGHT_RECEIVE)
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s/scrollingLight", channelName, propName);
      client.subscribe(channel);
      #endif
      #if defined(MQTT_TEXT_RECEIVE)
      snprintf(channel, MQTT_BUF_SZ, "/%s/%s/text", channelName, propName);
      client.subscribe(channel);
      #endif
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
