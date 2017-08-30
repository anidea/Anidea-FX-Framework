#include "network.h"
#include "escaperoommaster.h"

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
  else if (requestStr.startsWith(F("GET /INPUT0")) && pMyGame->INPUT0_OVERRIDE_ENABLE == 1)
  {
    Serial.println(F("polled for INPUT0 status!"));
    writeClientResponse(client, INPUT0_STATE ? F("triggered") : F("not triggered"));
  }
  else if (requestStr.startsWith(F("GET /INPUT1")) && pMyGame->INPUT1_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("polled for INPUT1 status!"));
    writeClientResponse(client, INPUT1_STATE ? F("triggered") : F("not triggered"));
  }
  else if (requestStr.startsWith(F("GET /INPUT2")) && pMyGame->INPUT2_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("polled for INPUT2 status!"));
    writeClientResponse(client, INPUT2_STATE ? F("triggered") : F("not triggered"));
  }
  else if (requestStr.startsWith(F("GET /INPUT3")) && pMyGame->INPUT3_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("polled for INPUT3 status!"));
    writeClientResponse(client, INPUT3_STATE ? F("triggered") : F("not triggered"));
  }
  else if (requestStr.startsWith(F("GET /INPUT4")) && pMyGame->INPUT4_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("polled for INPUT4 status!"));
    writeClientResponse(client, INPUT4_STATE ? F("triggered") : F("not triggered"));
  }
  else if (requestStr.startsWith(F("GET /INPUT5")) && pMyGame->INPUT5_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("polled for INPUT5 status!"));
    writeClientResponse(client, INPUT5_STATE ? F("triggered") : F("not triggered"));
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
  else if (requestStr.startsWith(F("GET /OUTPUT0_ON")) && pMyGame->OUTPUT0_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT0 turned on"));
    writeClientResponse(client, F("ok"));
    OUTPUT0_STATE = 1;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT0_OFF")) && pMyGame->OUTPUT0_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT0 turned off"));
    writeClientResponse(client, F("ok"));
    OUTPUT0_STATE = 0;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT1_ON")) && pMyGame->OUTPUT1_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT1 turned on"));
    writeClientResponse(client, F("ok"));
    OUTPUT1_STATE = 1;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT1_OFF")) && pMyGame->OUTPUT1_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT1 turned off"));
    writeClientResponse(client, F("ok"));
    OUTPUT1_STATE = 0;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT2_ON")) && pMyGame->OUTPUT2_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT2 turned on"));
    writeClientResponse(client, F("ok"));
    OUTPUT2_STATE = 1;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT2_OFF")) && pMyGame->OUTPUT2_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT2 turned off"));
    writeClientResponse(client, F("ok"));
    OUTPUT2_STATE = 0;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT3_ON")) && pMyGame->OUTPUT3_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT3 turned on"));
    writeClientResponse(client, F("ok"));
    OUTPUT3_STATE = 1;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT3_OFF")) && pMyGame->OUTPUT3_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT3 turned off"));
    writeClientResponse(client, F("ok"));
    OUTPUT3_STATE = 0;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT4_ON")) && pMyGame->OUTPUT4_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT4 turned on"));
    writeClientResponse(client, F("ok"));
    OUTPUT4_STATE = 1;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT4_OFF")) && pMyGame->OUTPUT4_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT4 turned off"));
    writeClientResponse(client, F("ok"));
    OUTPUT4_STATE = 0;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT5_ON")) && pMyGame->OUTPUT5_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT5 turned on"));
    writeClientResponse(client, F("ok"));
    OUTPUT5_STATE = 1;
  }
  else if (requestStr.startsWith(F("GET /OUTPUT5_OFF")) && pMyGame->OUTPUT5_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("OUTPUT5 turned off"));
    writeClientResponse(client, F("ok"));
    OUTPUT5_STATE = 0;
  }
  else if (requestStr.startsWith(F("GET /RELAY0_ON")) && pMyGame->RELAY0_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("RELAY0 turned on"));
    writeClientResponse(client, F("ok"));
    RELAY0_STATE = 1;
  }
  else if (requestStr.startsWith(F("GET /RELAY0_OFF")) && pMyGame->RELAY0_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("RELAY0 turned off"));
    writeClientResponse(client, F("ok"));
    RELAY0_STATE = 0;
  }
  else if (requestStr.startsWith(F("GET /RELAY1_ON")) && pMyGame->RELAY1_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("RELAY1 turned on"));
    writeClientResponse(client, F("ok"));
    RELAY1_STATE = 1;
  }
  else if (requestStr.startsWith(F("GET /RELAY1_OFF")) && pMyGame->RELAY1_OVERRIDE_ENABLE == 1) 
  {
    Serial.println(F("RELAY1 turned off"));
    writeClientResponse(client, F("ok"));
    RELAY1_STATE = 0;
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
