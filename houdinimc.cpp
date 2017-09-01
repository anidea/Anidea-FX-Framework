#include "network.h"
#include "houdinimc.h"

void houdinimc::loop(void)
{
  Network::loop();
  listenForEthernetClients();
  sendChanges();
}

void houdinimc::sendChanges(void)
{
  if (pMyGame->_puzzleSolved == 1 && sent == 0)
  {
    sprintf(pageAdd, " /%d_solved", MyIP[3]);
    (!getPage(HostIP, serverPort, pageAdd));
    sent = 1;
  }
  else if (pMyGame->_puzzleSolved == 0 && sent == 1)
  {
    sprintf(pageAdd, " /%d_reset", MyIP[3]);
    (!getPage(HostIP, serverPort, pageAdd));
    sent = 0;
  }

  for (int i = 0; i < NUM_INPUTS; i++)
  {
    if (INPUT_STATES[i] != INPUT_STATE_OLD[i] && pMyGame->INPUT_OVERRIDE_ENABLE[i] == 1) 
    {
      Serial.print(F("INPUT"));
      Serial.print(i);
      Serial.println(F(" status changed"));
      sprintf(pageAdd, " /%d_INPUT%d_%s", MyIP[3], i, INPUT_STATES[i] ? "ON" : "OFF");
      Serial.println(pageAdd);
      (!getPage(HostIP, serverPort, pageAdd));
      INPUT_STATE_OLD[i] = INPUT_STATES[i];
    }
  }
}

byte houdinimc::getPage(IPAddress ipBuf, int thisPort, char *page)
{
  int inChar;
  char outBuf[128];

  Serial.println(F("connecting..."));

  if (client.connect(ipBuf, thisPort) == 1)
  {
    Serial.println(F("connected"));

    sprintf(outBuf, "GET %s HTTP/1.1", page);
    client.println(outBuf);
    byte oct1 = HostIP[0];
    byte oct2 = HostIP[1];
    byte oct3 = HostIP[2];
    byte oct4 = HostIP[3];
    sprintf(serverName, "%d.%d.%d.%d", oct1, oct2, oct3, oct4);
    sprintf(outBuf, "Host: %s", serverName);
    client.println(outBuf);
    client.println(F("Connection: close\r\n"));
  }
  else
  {
    Serial.println(F("failed"));
    return 0;
  }

  // connectLoop controls the hardware fail timeout
  int connectLoop = 0;

  while (client.connected())
  {
    while (client.available())
    {
      inChar = client.read();
      Serial.write(inChar);
      // set connectLoop to zero if a packet arrives
      connectLoop = 0;
    }

    connectLoop++;

    // if more than 10000 milliseconds since the last packet
    if (connectLoop > 10000)
    {
      // then close the connection from this end.
      Serial.println();
      Serial.println(F("Timeout"));
      client.stop();
    }
    // this is a delay for the connectLoop timing
    delay(1);
  }

  Serial.println();

  Serial.println(F("disconnecting."));
  // close client end
  client.stop();

  return 1;
}

void houdinimc::processRequest(EthernetClient& client, String requestStr) 
{
  Serial.println(requestStr);

  if (requestStr.startsWith(F("GET /status")))
  {
    Serial.println(F("polled for status!"));
    writeClientResponse(client, pMyGame->isSolved() ? F("triggered") : F("not triggered"));
  }
  else if (requestStr.startsWith(F("GET /reset"))) 
  {
    Serial.println(F("Network Room reset"));
    writeClientResponse(client, F("ok"));
    pMyGame->reset();
  }
  else if (requestStr.startsWith(F("GET /trigger"))) 
  {
    Serial.println(F("Network prop solve"));
    writeClientResponse(client, F("ok"));
    pMyGame->forceSolved();
  }
  else if (requestStr.startsWith(F("GET /INPUT")))
  {
    for (int i = 0; i < NUM_INPUTS; i++)
    {
      if (requestStr.startsWith("GET /INPUT" + String(i)) && pMyGame->INPUT_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("polled for INPUT"));
        Serial.print(i);
        Serial.println(F(" status!"));
        writeClientResponse(client, INPUT_STATES[i] ? F("triggered") : F("not triggered"));
      }
    }
  }
  else if (requestStr.startsWith(F("GET /OUTPUT")))
  {
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
      if (requestStr.startsWith("GET /OUTPUT" + String(i) + "_OFF") && pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("OUTPUT"));
        Serial.print(i);
        Serial.println(F(" turned off"));
        writeClientResponse(client, F("ok"));
        OUTPUT_STATES[i] = 0;
      }
      else if (requestStr.startsWith("GET /OUTPUT" + String(i) + "_ON") && pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("OUTPUT"));
        Serial.print(i);
        Serial.println(F(" turned on"));
        writeClientResponse(client, F("ok"));
        OUTPUT_STATES[i] = 1;
      }
    }
  }
  else if (requestStr.startsWith(F("GET /RELAY")))
  {
    for (int i = 0; i < NUM_RELAYS; i++)
    {
      if (requestStr.startsWith("GET /RELAY" + String(i) + "_OFF") && pMyGame->RELAY_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("RELAY"));
        Serial.print(i);
        Serial.println(F(" turned off"));
        writeClientResponse(client, F("ok"));
        RELAY_STATES[i] = 0;
      }
      else if (requestStr.startsWith("GET /RELAY" + String(i) + "_ON") && pMyGame->RELAY_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("RELAY"));
        Serial.print(i);
        Serial.println(F(" turned off"));
        writeClientResponse(client, F("ok"));
        RELAY_STATES[i] = 1;
      }
    }
  }
  else
  {
    writeClientResponseNotFound(client);
  }
}

void houdinimc::listenForEthernetClients() 
{
  EthernetClient client = server.available();
  
  if (client) 
  {
    Serial.println(F("Got a client"));
    
    String requestStr;
    boolean firstLine = true;
    boolean currentLineIsBlank = true;
    
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) 
        {
          processRequest(client, requestStr);
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
          firstLine = false;
        } else if (c != '\r') 
        {
          currentLineIsBlank = false;
    
          if (firstLine) 
          {
            requestStr.concat(c);
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }
}

void houdinimc::writeClientResponse(EthernetClient& client, String bodyStr) 
{
  Serial.println(F("HTTP 200"));
  
  // send a standard http response header
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/plain"));
  client.println(F("Access-Control-Allow-Origin: *"));  // ERM will not be able to connect without this header!
  client.println();
  client.print(bodyStr);
}

void houdinimc::writeClientResponseNotFound(EthernetClient& client) 
{
  Serial.println(F("HTTP 404"));
  
  // send a standard http response header
  client.println(F("HTTP/1.1 404 Not Found"));
  client.println(F("Access-Control-Allow-Origin: *"));  // ERM will not be able to connect without this header!
  client.println();
} 
