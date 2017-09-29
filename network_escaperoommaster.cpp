#include "network.h"
#include "network_escaperoommaster.h"

void escaperoommaster::loop(void)
{
  Network::loop();
  listenForEthernetClients();
}

void escaperoommaster::processRequest(EthernetClient& client, String requestStr) 
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

void escaperoommaster::listenForEthernetClients() 
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

void escaperoommaster::writeClientResponse(EthernetClient& client, String bodyStr) 
{
  Serial.println(F("HTTP 200"));
  
  // send a standard http response header
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/plain"));
  client.println(F("Access-Control-Allow-Origin: *"));  // ERM will not be able to connect without this header!
  client.println();
  client.print(bodyStr);
}

void escaperoommaster::writeClientResponseNotFound(EthernetClient& client) 
{
  Serial.println(F("HTTP 404"));
  
  // send a standard http response header
  client.println(F("HTTP/1.1 404 Not Found"));
  client.println(F("Access-Control-Allow-Origin: *"));  // ERM will not be able to connect without this header!
  client.println();
} 
