#include "network.h"
#include "network_escaperoommaster.h"

void escaperoommaster::loop(void)
{
  Network::loop();
  listenForEthernetClients();
}

void escaperoommaster::processRequest(EthernetClient& client, String requestStr) 
{
//  //Serial.println(requestStr);

  if (requestStr.startsWith(F("GET /json")))
  {
//    //Serial.println(F("polled for JSON!"));
    createJSON(client);
  }
  else if (requestStr.startsWith(F("GET /status")))
  {
//    //Serial.println(F("polled for status!"));
    writeClientResponse(client, pMyGame->isSolved() ? F("triggered") : F("not_triggered"));
  }
  else if (requestStr.startsWith(F("GET /enable_status"))) 
  {
//    //Serial.println(F("polled for enable_status!"));
    writeClientResponse(client, pMyGame->isEnabled() ? F("enabled") : F("disabled"));
  }
  else if (requestStr.startsWith(F("GET /reset_enable"))) 
  {
//    //Serial.println(F("Network prop reset_enable"));
    writeClientResponse(client, F("ok"));
    pMyGame->reset();
    pMyGame->enable();
  }
  else if (requestStr.startsWith(F("GET /reset_disable"))) 
  {
//    //Serial.println(F("Network prop reset_disable"));
    writeClientResponse(client, F("ok"));
    pMyGame->reset();
    pMyGame->disable();
  }
  else if (requestStr.startsWith(F("GET /reset"))) 
  {
//    //Serial.println(F("Network Room reset"));
    writeClientResponse(client, F("ok"));
    pMyGame->reset();
  }
  else if (requestStr.startsWith(F("GET /trigger"))) 
  {
//    //Serial.println(F("Network prop solve"));
    writeClientResponse(client, F("ok"));
    pMyGame->forceSolved();
  }
  else if (requestStr.startsWith(F("GET /enable"))) 
  {
//    //Serial.println(F("Network prop enable"));
    writeClientResponse(client, F("ok"));
    pMyGame->enable();
  }
  else if (requestStr.startsWith(F("GET /disable"))) 
  {
//    //Serial.println(F("Network prop disable"));
    writeClientResponse(client, F("ok"));
    pMyGame->disable();
  }
  else if (requestStr.startsWith(F("GET /INPUT")))
  {
    for (int i = 0; i < NUM_INPUTS; i++)
    {
      if (requestStr.startsWith("GET /INPUT" + String(i))) 
      {
//        //Serial.print(F("polled for INPUT"));
//        //Serial.print(i);
//        //Serial.println(F(" status!"));
        writeClientResponse(client, pMyGame->INPUT_STATES[i] ? F("triggered") : F("not_triggered"));
      }
    }
  }
  else if (requestStr.startsWith(F("GET /OUTPUT")))
  {
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
      if (requestStr.startsWith("GET /OUTPUT" + String(i) + "_OFF")) 
      {
        if (pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == true)
        {
//          //Serial.print(F("OUTPUT"));
//          //Serial.print(i);
//          //Serial.println(F(" turned off"));
          writeClientResponse(client, F("ok"));
          pMyGame->OUTPUT_STATES[i] = false;
          pMyGame->OUTPUT_STATES_FLAG[i] = true;
        }
      }
      else if (requestStr.startsWith("GET /OUTPUT" + String(i) + "_ON")) 
      {
        if (pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == true)
        {
          //Serial.print(F("OUTPUT"));
          //Serial.print(i);
          //Serial.println(F(" turned on"));
          writeClientResponse(client, F("ok"));
          pMyGame->OUTPUT_STATES[i] = true;
          pMyGame->OUTPUT_STATES_FLAG[i] = true;
        }
      }
      else if (requestStr.startsWith("GET /OUTPUT" + String(i))) 
      {
//        //Serial.print(F("polled for OUTPUT"));
//        //Serial.print(i);
//        //Serial.println(F(" status!"));
        writeClientResponse(client, digitalRead(OUTPUTS[i]) ? F("triggered") : F("not_triggered"));
      }
    }
  }
  else if (requestStr.startsWith(F("GET /RELAY")))
  {
    for (int i = 0; i < NUM_RELAYS; i++)
    {
      if (requestStr.startsWith("GET /RELAY" + String(i) + "_OFF")) 
      {
        if (pMyGame->RELAY_OVERRIDE_ENABLE[i] == true)
        {
//          //Serial.print(F("RELAY"));
//          //Serial.print(i);
//          //Serial.println(F(" turned off"));
          writeClientResponse(client, F("ok"));
          pMyGame->RELAY_STATES[i] = false;
          pMyGame->RELAY_STATES_FLAG[i] = true;
        }
      }
      else if (requestStr.startsWith("GET /RELAY" + String(i) + "_ON")) 
      {
        if (pMyGame->RELAY_OVERRIDE_ENABLE[i] == true)
        {
//          //Serial.print(F("RELAY"));
//          //Serial.print(i);
//          //Serial.println(F(" turned off"));
          writeClientResponse(client, F("ok"));
          pMyGame->RELAY_STATES[i] = true;
          pMyGame->RELAY_STATES_FLAG[i] = true;
        }
      }
      else if (requestStr.startsWith("GET /RELAY" + String(i))) 
      {
//        //Serial.print(F("polled for RELAY"));
//        //Serial.print(i);
//        //Serial.println(F(" status!"));
        writeClientResponse(client, digitalRead(RELAYS[i]) ? F("triggered") : F("not_triggered"));
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
//    //Serial.println(F("Got a client"));
    
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
//  //Serial.println(F("HTTP 200"));
  
  // send a standard http response header
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/plain"));
  client.println(F("Access-Control-Allow-Origin: *"));  // ERM will not be able to connect without this header!
  client.println();
  client.print(bodyStr);
}

void escaperoommaster::writeClientResponseNotFound(EthernetClient& client) 
{
//  //Serial.println(F("HTTP 404"));
  
  // send a standard http response header
  client.println(F("HTTP/1.1 404 Not Found"));
  client.println(F("Access-Control-Allow-Origin: *"));  // ERM will not be able to connect without this header!
  client.println();
}

void escaperoommaster::createJSON(EthernetClient& client)
{
  // send a standard http response header
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/plain"));
  client.println(F("Access-Control-Allow-Origin: *"));  // ERM will not be able to connect without this header!
  client.println();
  client.print(F("{"));

  client.print(F("\"status\":"));
  client.print(pMyGame->isSolved() ? '1' : '0');
  client.print(F(","));
  
  client.print(F("\"enabled\":"));
  client.print(pMyGame->isEnabled() ? '1' : '0');
  client.print(F(","));

  client.print(F("\"INPUT0\":"));
  client.print(digitalRead(INPUT0) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"INPUT1\":"));
  client.print(digitalRead(INPUT1) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"INPUT2\":"));
  client.print(digitalRead(INPUT2) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"INPUT3\":"));
  client.print(digitalRead(INPUT3) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"INPUT4\":"));
  client.print(digitalRead(INPUT4) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"INPUT5\":"));
  client.print(digitalRead(INPUT5) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"OUTPUT0\":"));
  client.print(digitalRead(OUTPUT0) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"OUTPUT1\":"));
  client.print(digitalRead(OUTPUT1) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"OUTPUT2\":"));
  client.print(digitalRead(OUTPUT2) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"OUTPUT3\":"));
  client.print(digitalRead(OUTPUT3) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"OUTPUT4\":"));
  client.print(digitalRead(OUTPUT4) == HIGH ? '1' : '0');
  client.print(F(","));

  client.print(F("\"OUTPUT5\":"));
  client.print(digitalRead(OUTPUT5) == HIGH ? '1' : '0');

  client.print(F("}"));
}


