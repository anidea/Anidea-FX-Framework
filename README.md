# Anidea FX Controller

This application is targeted for the AEI FX series of boards, an Arduino compatible series of controllers from Anidea Engineering.  While this program is free (MIT LECENSE), please consider purchasing an FX300 or FX350 to support us making more free code. While this framework will run on an FX300 you will need the FX350 to use any of the management software connectivity.

You can purchase an FX300 at http://get.anidea-engineering.com/fx300_order_request  
You can purchase an FX350 at http://get.anidea-engineering.com/fx350_order_request

This is a generic framework for running different games on escape room props and connecting them with different management software.

## Downloading
Please note that when you first download and unzip the software you must rename the containing folder to "Anidea-FX-Framework" or the Arduino software will not be able to open it.

## Required Libraries
To compile the program you must have the following libraries installed:
* MsTimer2
* Ethernet2
* PubSubClient
* ArduinoJson

The PubSubClient library is only needed if you are using MQTT. The ArduinoJson library is only needed if you are using EscapeRoomMaster or MQTT. If you do not need to use either of these networks you can delete their files and remove their includes from the main file so you will not need to install these libraries.

## Includes
At the top of the includes there are several boards included. Only one can be uncommented to tell the program which pin definitions and functions to use.

Each game and each network have both a header and .cpp file that define their operation. Any new game or network added must have its header (.h) file included in the main .ino file.

```
  // Uncomment only one of these lines for the board you want
  #include "fx300.h"
  //#include fx###.h

  #include "game.h"
  #include "network.h"

  // Networks
  #include "escaperoommaster.h"
  #include "cluecontrol.h"
  #include "houdinimc.h"

  // Include game headers here
  #include "game_simplegame.h"
  #include "game_lightsout.h"
  #include "game_sequencedetect.h"
  #include "game_sixwire.h"
  #include "game_inputsequence.h"
  #include "game_empty.h"
```

## Game and Network
There are several declarations of "myGame". Only one can be uncommented to tell the program which game to run.

The next section is the network variables. These need to be configured for the specific device as well as for the management software. MyMac and MyIP are needed for any type of network. HostIP is needed by ClueControl and HoudiniMC but not EscapeRoomMaster.

Lastly is the network section. Only one can be uncommented to tell the program which management software to communicate with.

```
  // Uncomment only one of these lines for the game you want
//  myGame = new simplegame(); //Simple game provided as an example
//  myGame = new lightsout(); //Lights out game
//  myGame = new sequencedetect(); //Sequencedetect
//  myGame = new sixwire(); //Sixwire
//  myGame = new inputsequence(); //Detects a sequence of inputs
//  myGame = new empty(); // Empty game to manually control inputs and outputs only

  byte MyMac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5};   // This must be unique for each device
  IPAddress MyIP(10, 0, 1, 210);                         // This must be unique for each device
  IPAddress HostIP(10, 0, 1, 115);                       // This should be the IP of the device running the management software

  // Uncomment only one of these lines for the network you want
//  myNetwork = new escaperoommaster(MyMac, MyIP, HostIP);
//  myNetwork = new cluecontrol(MyMac, MyIP, HostIP);
//  myNetwork = new nodered(MyMac, MyIP, HostIP);
//  myNetwork = new houdinimc(MyMac, MyIP, HostIP);
```

## Overrides
Any input or output can be overridden to interface with the network independently from the puzzle. If they are enabled the outputs can be set and inputs can report their status regardless of the state of the puzzle currently running.

In the constructor of every game there is a section that enables or disables every input and output according to its needs. It is recommended that any input or output that is used by the game itself is disabled. By default they are all enabled.

```
// Only enable the inputs/outputs that are not going to be used by this game
  INPUT_OVERRIDE_ENABLE[0] = 1;
  INPUT_OVERRIDE_ENABLE[1] = 1;
  INPUT_OVERRIDE_ENABLE[2] = 1;
  INPUT_OVERRIDE_ENABLE[3] = 1;
  INPUT_OVERRIDE_ENABLE[4] = 1;
  INPUT_OVERRIDE_ENABLE[5] = 1;

  OUTPUT_OVERRIDE_ENABLE[0] = 1;
  OUTPUT_OVERRIDE_ENABLE[1] = 1;
  OUTPUT_OVERRIDE_ENABLE[2] = 1;
  OUTPUT_OVERRIDE_ENABLE[3] = 1;
  OUTPUT_OVERRIDE_ENABLE[4] = 1;
  OUTPUT_OVERRIDE_ENABLE[5] = 1;

  RELAY_OVERRIDE_ENABLE[0] = 1;
  RELAY_OVERRIDE_ENABLE[1] = 1;
```

There is also a game called "empty" that has nothing running and all inputs enabled that serves as a manual prop controller.

## Network Communication

In these examples 0.0.0.123 is used as MyIP. Remember to change that address to the IP of the device.

### EscapeRoomMaster

#### Input
Go to "Automation" -> Select your room -> Click "Add Event" -> Select "Network Polling (Listen for Prop)" at the top.

The prop will respond with either "triggered" or "not triggered". Use "triggered" in the "Trigger Value" field.

To get the status of the puzzle use the url:  
http://0.0.0.123/status

To get the status of any specific input use the url format:  
http://0.0.0.123/INPUT0  
Where 0 is the number of the input you are polling for.

Example:  
![erminputexample](https://user-images.githubusercontent.com/31215073/30436135-71b26c72-9939-11e7-9200-c41bf2f566e5.png)

#### Output
Go to "Automation" -> Select your room -> Click "Add Event" -> Select "Send Network Request (Trigger Prop)" at the bottom.

When outputs are set they will respond with "ok" if successful.

To trigger the puzzle use the url:  
http://0.0.0.123/trigger

To reset the puzzle use the url:  
http://0.0.0.123/reset

To manually set a specific ouput use this url for on:  
http://0.0.0.123/OUTPUT0_ON  
And this url for off:  
http://0.0.0.123/OUTPUT0_OFF  
Where 0 is the number of the output you are setting.

To manually set a specific relay use this url for on:  
http://0.0.0.123/RELAY0_ON  
And this url for off:  
http://0.0.0.123/RELAY0_OFF  
Where 0 is the number of the relay you are setting.

Example:  
![ermoutputexample](https://user-images.githubusercontent.com/31215073/30436136-71f4dec2-9939-11e7-8810-2eb0d7b8aec9.png)

### ClueControl

#### Input
Go to "System Setup" -> "ModBus Setup" -> Select the "Triggers" tab.

Since ClueControl does not distinguish between IP addresses it is receiving from, the last 3 digits of the IP are part of the register that the prop sends the update to.

The register that tells you if the game has been solved or not is just the last 3 digits alone. So in this case the register "123" will be triggered when the puzzle is solved.

The register that will be updated when a specific input is changed is the last 3 digits of the IP followed by the number of the input. So the register "1230" will be updated when the status of input 0 on the prop with that IP address changes.

Example:  
![ccinputexample](https://user-images.githubusercontent.com/31215073/30436407-090fa5bc-993a-11e7-921b-7505cf4f4aba.png)

#### Output
Go to "System Setup" -> "ModBus Setup" -> Select the "Switches" tab.

The IP Address field in ClueControl must be set to the IP of the prop.

To trigger the prop set "# to Send" to 1. To reset the prop set it to 2.

To set the value of a specific output a 3 digit combination is used.  
The first digit is the type of output. 1 for output and 2 for relay.  
The second digit is the number of the output you are targeting.  
The third digit is the status you want to update it to. 1 for on 0 for off.  
For example if you wanted to set output 3 to on you would use "131" as the "# to Send".

Example:  
![ccoutputexample](https://user-images.githubusercontent.com/31215073/30436408-090fe2c0-993a-11e7-8d70-0ef9ca9fa0fb.png)

### HoudiniMC

#### Input
Go to "Scheduled Events" -> Click "Add Event" -> Select "Incoming Event from a Smart Mechanism" from "Event Type".

Since HoudiniMC does not distinguish between IP addresses it is receiving from, the last 3 digits of the IP are part of the url that the prop sends the update to.

The register that tells you if the game has been solved is the last 3 digits of the IP followed by \_solved. So to have an event be triggered if the puzzle is solved use "123_solved" after "URL to Listen". To tell if it has been reset use \_reset instead

The url that will be updated when a specific input is changed is:  
The last 3 digits of the IP Address +  
\_INPUT +  
The number of the input +  
\_ON for on and \_OFF for off  
For example if you wanted to listen for input 3 being turned on you would use "123_INPUT3_ON" as the "URL to Listen".

Example:  
![houdiniinputexample](https://user-images.githubusercontent.com/31215073/30439878-fad17a80-9942-11e7-9484-b7b62f86b933.png)

#### Output
Go to "Automations" -> Select the "HTTP Requests" tab -> Select "GET Request"

When outputs are set they will respond with "ok" if successful.

To trigger the puzzle use the url:  
http://0.0.0.123/trigger

To reset the puzzle use the url:  
http://0.0.0.123/reset

To manually set a specific ouput use this url for on:  
http://0.0.0.123/OUTPUT0_ON  
And this url for off:  
http://0.0.0.123/OUTPUT0_OFF  
Where 0 is the number of the output you are setting.

To manually set a specific relay use this url for on:  
http://0.0.0.123/RELAY0_ON  
And this url for off:  
http://0.0.0.123/RELAY0_OFF  
Where 0 is the number of the relay you are setting.

Example:  
![houdinioutputexample](https://user-images.githubusercontent.com/31215073/30439877-facff67e-9942-11e7-80a8-0f918a1287a4.png)

### MQTT

All communication with the prop is done on the mqtt topic "Prop" followed by the last 3 digits of the IP. So in thise case the topic is "Prop123".  
Inputs and outputs are sent through the topic as JSON objects.  

#### Input
When the prop sends out the game state the object is formatted as follows:
```
{
    "TYPE": "GAMESTATE",
    "DIRECTION": "FROM",
    "VALUE": 1
}
```
This indicates that this is a message from the prop about the gamestate. The "VALUE" will be 1 if the prop has been solved or 0 if it has been reset.

For specific inputs the object is formatted as follows:  
```
{
    "TYPE": "INPUT",
    "INPUT": 0,
    "VALUE": 1
}
```
This indicates that the message is for an input and the input is number 0. The "VALUE" is the value of the input.

#### Output
To set the game state the object is formatted as follows:
```
{
    "TYPE": "GAMESTATE",
    "DIRECTION": "TO",
    "VALUE": 1
}
```
This indicates that this is a message to the prop about the gamestate. The "VALUE" should be 1 to solve the prop or 0 to reset the prop.

To set a specific output the object is formatted as follows:
```
{
    "TYPE": "OUTPUT",
    "OUTPUT": 0,
    "VALUE": 1
}
```
This indicates that the nessage is for an output and the output is number 0. The "VALUE" is the value you are setting the output to.

#### Node-RED
An easy way to work with the mqtt interface is using Node-RED. Here is an example layout:
![node-redexample](https://user-images.githubusercontent.com/31215073/31030159-53be35c6-a522-11e7-8afa-d47c1a696a16.png)
