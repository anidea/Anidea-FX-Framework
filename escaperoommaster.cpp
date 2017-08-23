#include "network.h"
#include "escaperoommaster.h"

void escaperoommaster::loop(void)
{
  listenForEthernetClients();
}

void escaperoommaster::processRequest(EthernetClient& client, String requestStr) 
{
  Serial.println(requestStr);

  if (requestStr.startsWith("GET /status")) 
  {
    Serial.println("polled for status!");
    writeClientResponse(client, pMyGame->isSolved() ? "triggered" : "not triggered");
  } else if (requestStr.startsWith("GET /reset")) 
  {
    Serial.println("Network Room reset");
    writeClientResponse(client, "ok");
    pMyGame->reset();
  } else if (requestStr.startsWith("GET /trigger")) 
  {
    Serial.println("Network prop solve");
    writeClientResponse(client, "ok");
    pMyGame->forceSolved();
  } else {
    writeClientResponseNotFound(client);
  }
}

void escaperoommaster::listenForEthernetClients() 
{
  EthernetClient client = server.available();

  if (client) 
  {
    Serial.println("Got a client");
    
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
  Serial.println("HTTP 200");
  
  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Access-Control-Allow-Origin: *");  // ERM will not be able to connect without this header!
  client.println();
  client.print(bodyStr);
}

void escaperoommaster::writeClientResponseNotFound(EthernetClient& client) 
{
  Serial.println("HTTP 404");
  
  // send a standard http response header
  client.println("HTTP/1.1 404 Not Found");
  client.println("Access-Control-Allow-Origin: *");  // ERM will not be able to connect without this header!
  client.println();
} 
