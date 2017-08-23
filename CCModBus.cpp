
#include "CCModBus.h"

// For Arduino 1.0
EthernetServer MbServer(MB_PORT);
EthernetClient MbmClient;

#define DEBUG
#ifdef DEBUG
  #include <SPI.h>
#endif

CCModBus::CCModBus()
{
}

void CCModBus::SetTrigger(word RegNumber, bool TurnOn)
{
  
  MbmByteArray[0] = 0;  // ID high byte
  MbmByteArray[1] = 1;  // ID low byte
  MbmByteArray[2] = 0;  // protocol high byte
  MbmByteArray[3] = 0;  // protocol low byte
  MbmByteArray[5] = 6;  // Lenght low byte;
  MbmByteArray[4] = 0;  // Lenght high byte
  MbmByteArray[6] = 1;  // unit ID
  MbmByteArray[7] = MB_FC_WRITE_COIL; // function code
  MbmByteArray[8] = highByte(RegNumber);   // highbyte of the register address;
  MbmByteArray[9] = lowByte(RegNumber);   // lowByte of the register address
  if (TurnOn) 
  {
        MbmByteArray[10] = 255;  //high byte of the new value
  }else
  {
        MbmByteArray[10] = 0;  //high byte of the new value
  }
  
  MbmByteArray[11] = 0; //low byte of the new value
  
  SendData();
}

//****************** Send data for ModBusMaster ****************
void CCModBus::SendToCC(word NewVal)
{
  
  MbmByteArray[0] = 0;  // ID high byte
  MbmByteArray[1] = 1;  // ID low byte
  MbmByteArray[2] = 0;  // protocol high byte
  MbmByteArray[3] = 0;  // protocol low byte
  MbmByteArray[5] = 6;  // Lenght low byte;
  MbmByteArray[4] = 0;  // Lenght high byte
  MbmByteArray[6] = 1;  // unit ID
  MbmByteArray[7] = MB_FC_WRITE_REGISTER; // function code
  MbmByteArray[8] = 0;   // byte of the register address;
  MbmByteArray[9] = 1;   // lowByte of the register address
  MbmByteArray[10] = highByte(NewVal);
  MbmByteArray[11] = lowByte(NewVal);
  
  SendData();
}

void CCModBus::SendData()
{
  //****************** Connect to Slave and send data ******************
  if (MbmClient.connect(ClueControlIP,502)) 
  {
    #ifdef DEBUG
      Serial.print("connected with ClueControl on IP ");
      Serial.println(ClueControlIP);
      Serial.print("Sending request: ");
      for(int i=0;i<MbmByteArray[5]+6;i++) 
      {
        if(MbmByteArray[i] < 16){Serial.print("0");}
        Serial.print(MbmByteArray[i],HEX);
        if (i != MbmByteArray[5]+5) {Serial.print(".");} else {Serial.println();}
      }
    #endif    
    
    MbmClient.write(MbmByteArray,MbmByteArray[5]+6);
    
    MbmByteArray[7] = 0;
  } 
  else 
  {
    #ifdef DEBUG
      Serial.print("connection with ClueControl failed at IP ");
      Serial.println(ClueControlIP);
    #endif    
    MbmClient.stop();
  }
}

//todo:  Add retries and receive confirmation


 

void CCModBus::ReceiveModBus()
{  
  
  
  //****************** Check for response from ClueControl ****************
  while (MbmClient.available()) 
  {
    MbmByteArray[MbmCounter] = MbmClient.read();
    if (MbmCounter > 4)  
    {
      if (MbmCounter == MbmByteArray[5] + 5) // the full answer is recieved  
      {
        MbmClient.stop();
        
        #ifdef DEBUG
          Serial.println("Received ACK from ClueControl");
          Serial.println ();
        #endif
  
        //if more processing of ACK is  needed, do it here
       }
    }
    MbmCounter++;
  }
  MbmCounter = 0;
  
  //****************** Read from socket ****************
  EthernetClient client = MbServer.available();
  if(!client.available()) {return;}
  //only execute anything else if there was somethign avaiable in the buffer
  
    delay(10);
    int i = 0;
    while(client.available())
    {
      MbsByteArray[i] = client.read();
      i++;
    }
    
    //****************** Decode the received data ******************
    #ifdef DEBUG
       Serial.print ("Received from CC:  ");
       for (int i=0;i<MbsByteArray[5]+6;i++) 
       {
          if(MbsByteArray[i] < 16) {Serial.print("0");}
          Serial.print(MbsByteArray[i],HEX);
          if (i != MbsByteArray[5]+5) 
          {
              Serial.print(".");
          } 
          else
         {
              Serial.println();
         }
       }
      #endif 
    
    if(MbsByteArray[7] == MB_FC_WRITE_REGISTER)
    {
       if(MbsByteArray[8] == 0 && MbsByteArray[9] == 1) //this is a command to update from CC
       {
          CCValue = word(MbsByteArray[10],MbsByteArray[11]);
          MbsByteArray[5] = 6; //Number of bytes after this one.
          client.write(MbsByteArray, 12);
          MbsByteArray[7]=0;
        
          #ifdef DEBUG
            Serial.print ("New Value Received from CC:  ");
            Serial.println (CCValue);
            Serial.println ();
          #endif

        }
 
    }
  
}

