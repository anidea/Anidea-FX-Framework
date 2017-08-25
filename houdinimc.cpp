#include "network.h"
#include "houdinimc.h"

void houdinimc::loop(void)
{
  listenForEthernetClients();

  Serial.println("IP??");
  Serial.println(HostIP);

  if (pMyGame->_puzzleSolved == 1 && sent == 0)
  {
    sprintf(pageAdd, " / ");
    sprintf(pageAdd, pMyGame->gameName.c_str());
    sprintf(pageAdd, "_solved"); (!getPage(HostIP, serverPort, pageAdd));
    sent = 1;
  }
  else if (pMyGame->_puzzleSolved == 0 && sent == 1)
  {
    sprintf(pageAdd, " / ");
    sprintf(pageAdd, pMyGame->gameName.c_str());
    sprintf(pageAdd, "_reset"); (!getPage(HostIP, serverPort, pageAdd));
    sent = 0;
  }
}

byte houdinimc::getPage(IPAddress ipBuf, int thisPort, char *page)
{
  int inChar;
  char outBuf[128];

  Serial.print(F("connecting…"));

  Serial.println(ipBuf);
  Serial.println(thisPort);

  if (client.connect(ipBuf, thisPort) == 1)
  {
    Serial.println(F("connected"));

    sprintf(outBuf, "GET % s HTTP / 1.1", page);
    client.println(outBuf);
    sprintf(serverName,"%d.%d.%d.%d", HostIP[0],HostIP[1],HostIP[2],HostIP[3]);
    Serial.println();
    Serial.println(HostIP[0]);
    Serial.println(HostIP[1]);
    Serial.println(HostIP[2]);
    Serial.println(HostIP[3]);
    Serial.println();
    Serial.println(serverName);
    Serial.println();
    sprintf(outBuf, "Host: % s", serverName);
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

void houdinimc::listenForEthernetClients() 
{
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

void houdinimc::writeClientResponse(EthernetClient& client, String bodyStr) 
{
  Serial.println("HTTP 200");
  
  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Access-Control-Allow-Origin: *");  // ERM will not be able to connect without this header!
  client.println();
  client.print(bodyStr);
}

void houdinimc::writeClientResponseNotFound(EthernetClient& client) 
{
  Serial.println("HTTP 404");
  
  // send a standard http response header
  client.println("HTTP/1.1 404 Not Found");
  client.println("Access-Control-Allow-Origin: *");  // ERM will not be able to connect without this header!
  client.println();
} 
