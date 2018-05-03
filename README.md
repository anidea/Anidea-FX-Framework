# Escape Room Techs FX Controller

This application is targeted for the FX 300 and 400 series of boards, an Arduino compatible series of controllers from Escape Room Techs.  While this program is free (MIT LECENSE), please consider purchasing an FX350 or FX450 to support us making more free code. While this framework will run on an FX300 you will need the FX350 or FX450 to use any of the networking.

[Purchase an FX300](https://www.escaperoomtechs.com/arduino-compatible-prop-controller-FX300-p/fx300ardpropctrl.htm)

[Purchase an FX350](https://www.escaperoomtechs.com/escape-room-prop-controller-ethernet-arduino-fx350-p/fx350.htm)

[Purchase an FX450](https://www.escaperoomtechs.com/escape-room-prop-controller-ethernet-arduino-fx450-p/fx450.htm)


This is a generic framework for running different games on escape room props and connecting them with different management software.

## Downloading
Please note that when you first download and unzip the software you should rename the containing folder to "Anidea-FX-Framework". The IDE will prompt to do this automatically if you do not and if you so choose will relocate the folder to your Arduino sketchbook folder (typically under My Documents/Arduino). 

## Includes
Each game and each network have both a header and .cpp file that define their operation. Any new game or network added must have its header (.h) file included in the main .ino file.

```
#include "game.h"
#include "network.h"

// Networks
#include "network_empty.h"
#include "network_escaperoommaster.h"
#include "network_cluecontrol.h"
#include "network_houdinimc.h"
#include "network_mqtt.h"

// Include game headers here
#include "game_empty.h"
#include "game_room.h"
#include "game_simplegame.h"
#include "game_sequencedetect.h"
#include "game_sixwire.h"
#include "game_inputsequence.h"
#include "game_rfid.h"
```

## Game and Network
There are several declarations of "myGame". Only one can be uncommented to tell the program which game to run.

The next section is the network variables. These need to be configured for the specific device as well as for the management software. MyMac and MyIP are needed for any type of network. HostIP is needed by ClueControl, HoudiniMC, and MQTT but not EscapeRoomMaster.

Lastly is the network section. Only one can be uncommented to tell the program which management software to communicate with.

```
  // Uncomment only one of these lines for the game you want
//  myGame = new game_empty(); // Empty game to manually control inputs and outputs only
//  myGame = new room(); // Used to control a whole room
//  myGame = new simplegame(); //Simple game provided as an example
//  myGame = new sequencedetect(); //Sequencedetect
//  myGame = new sixwire(); //Sixwire
//  myGame = new inputsequence(); //Detects a sequence of inputs

  byte MyMac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5};   // This must be unique for each device
  IPAddress MyIP(0, 0, 0, 0);                         // This must be unique for each device on the network. Leave blank to configure at run time.
  IPAddress HostIP(0, 0, 0, 0);                       // This should be the IP of the device running the management software. Not needed for ERM

  // Uncomment only one of these lines for the network you want
//  myNetwork = new network_empty(); //Empty network for use with FX300
//  myNetwork = new escaperoommaster(MyMac, MyIP, HostIP);
//  myNetwork = new cluecontrol(MyMac, MyIP, HostIP);
//  myNetwork = new mqtt(MyMac, MyIP, HostIP);
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

There is also a game called "empty" that has nothing running and all inputs/outputs enabled that serves as a manual prop controller.

## Network Communication

In these examples 0.0.0.123 is used as MyIP. Remember to change that address to the IP of the device.

### EscapeRoomMaster

#### Input (Manual Polling)
Go to "Automation" -> Select your room -> Click "Add Event" -> Select "Network Polling (Listen for Prop)" at the top.

The prop will respond with either "triggered" or "not_triggered". Use "triggered" in the "Trigger Value" field.

To get the status of the puzzle use the url:  
http://0.0.0.123/status

To get the status of any specific input use the url format:  
http://0.0.0.123/INPUT0  
Where 0 is the number of the input you are polling for.

To find out if the prop is enabled use the url:  
http://0.0.0.123/enable_status  
This responds with "enabled" and "disabled" instead of "triggered" and "not_triggered".

If you are using the room controller game there are specific urls for the statuses of the buttons:  
http://0.0.0.123/button1  
http://0.0.0.123/button2  
http://0.0.0.123/button1hold  
http://0.0.0.123/button2hold  
http://0.0.0.123/buttons1and2  
In order to prevent retriggering, the button state is reset when it is polled. When one of these urls is polled, it will reply with "on" if the corresponding button press has happened. It will then reply with "off" until it has happened again.

Example:  
![erminputexample](https://user-images.githubusercontent.com/31215073/30436135-71b26c72-9939-11e7-9200-c41bf2f566e5.png)

#### Input (JSON)
In cases where there are a lot of things that need to be polled, it is better to use JSON. The controller only needs to be polled once and it will output all of the information needed by ERM.

Go to "Automation" -> Select your room -> Click "Add Event" -> Select "Network Polling (Listen for Prop)" at the top.

Enter for the url:  
http://0.0.0.123/json

Choose "Run Script" from the Actions drop down menu -> Enter "Receive JSON" for the name -> Paste in the following script:
```
//Parse JSON string to object
var obj = JSON.parse(env.returnVal);

//status
if (obj.status == 1)
    trigger("solved");
else if (obj.status == 0)
    trigger("reset");

//buttonState
if (obj.buttonState == 1)
    trigger("button1");
else if (obj.buttonState == 2)
    trigger("button2");
else if (obj.buttonState == 3)
    trigger("button1hold");
else if (obj.buttonState == 4)
    trigger("button2hold");
else if (obj.buttonState == 5)
    trigger("buttons1and2");

//enabled
if (obj.enabled == 1)
    trigger("enabled");
else if (obj.enabled == 0)
    trigger("disabled");

//Inputs

//INPUT0
if (obj.INPUT0 == 1)
    trigger("INPUT0_ON");
else if (obj.INPUT0 == 0)
    trigger("INPUT0_OFF");
//INPUT1
if (obj.INPUT1 == 1)
    trigger("INPUT1_ON");
else if (obj.INPUT1 == 0)
    trigger("INPUT1_OFF");
//INPUT2
if (obj.INPUT2 == 1)
    trigger("INPUT2_ON");
else if (obj.INPUT2 == 0)
    trigger("INPUT2_OFF");
//INPUT3
if (obj.INPUT3 == 1)
    trigger("INPUT3_ON");
else if (obj.INPUT3 == 0)
    trigger("INPUT3_OFF");
//INPUT4
if (obj.INPUT4 == 1)
    trigger("INPUT4_ON");
else if (obj.INPUT4 == 0)
    trigger("INPUT4_OFF");
//INPUT5
if (obj.INPUT5 == 1)
    trigger("INPUT5_ON");
else if (obj.INPUT5 == 0)
    trigger("INPUT5_OFF");

//Outputs

//OUTPUT0
if (obj.OUTPUT0 == 1)
    trigger("OUTPUT0_ON");
else if (obj.OUTPUT0 == 0)
    trigger("OUPUT0_OFF");
//OUTPUT1
if (obj.OUTPUT1 == 1)
    trigger("OUTPUT1_ON");
else if (obj.OUTPUT1 == 0)
    trigger("OUPUT1_OFF");
//OUTPUT2
if (obj.OUTPUT2 == 1)
    trigger("OUTPUT2_ON");
else if (obj.OUTPUT2 == 0)
    trigger("OUPUT2_OFF");
//OUTPUT3
if (obj.OUTPUT3 == 1)
    trigger("OUTPUT3_ON");
else if (obj.OUTPUT3 == 0)
    trigger("OUPUT3_OFF");
//OUTPUT4
if (obj.OUTPUT4 == 1)
    trigger("OUTPUT4_ON");
else if (obj.OUTPUT4 == 0)
    trigger("OUPUT4_OFF");
//OUTPUT5
if (obj.OUTPUT5 == 1)
    trigger("OUTPUT5_ON");
else if (obj.OUTPUT5 == 0)
    trigger("OUPUT5_OFF");
```

Click save. Now ERM will poll the controller for information and trigger custom events based on it.

Example:  
![json example](https://user-images.githubusercontent.com/31215073/33152335-18001cb8-cfaa-11e7-9d3d-385be72a5957.png)

Here is the process to add an action based off a custom event:

Click "Add Event" in the automation editor -> Choose "Custom Event" from the Event drop down menu -> Type in the name of the event you want to listen for -> Choose the action you want to occur.

Here is a full list of the custom events:  
* solved
* reset
* button1
* button1hold  
* button2hold  
* buttons1and2  
* enabled  
* disabled  
* INPUT0_ON
* INPUT1_ON
* INPUT2_ON  
* INPUT3_ON  
* INPUT4_ON  
* INPUT5_ON  
* INPUT0_OFF
* INPUT1_OFF
* INPUT2_OFF  
* INPUT3_OFF  
* INPUT4_OFF  
* INPUT5_OFF
* OUTPUT0_ON
* OUTPUT1_ON
* OUTPUT2_ON  
* OUTPUT3_ON  
* OUTPUT4_ON  
* OUTPUT5_ON  
* OUTPUT0_OFF
* OUTPUT1_OFF
* OUTPUT2_OFF  
* OUTPUT3_OFF  
* OUTPUT4_OFF  
* OUTPUT5_OFF

#### Output
Go to "Automation" -> Select your room -> Click "Add Event" -> Select "Send Network Request (Trigger Prop)" at the bottom.

When outputs are set they will respond with "ok" if successful.

To trigger the puzzle use the url:  
http://0.0.0.123/trigger

To reset the puzzle use the url:  
http://0.0.0.123/reset

To enable or disable the puzzle use these urls:  
http://0.0.0.123/enable  
http://0.0.0.123/disable  

You can reset into a specific enable state by using these urls:  
http://0.0.0.123/reset_enable  
http://0.0.0.123/reset_disable  

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

The register that tells you if the game has been solved or not is the last 3 digits followed by a 0. So in this case the register "1230" will be triggered when the puzzle is solved.

The register that tells you if the game is enabled is the last 3 digits followed by a 1. So in this case the register "1231" will be triggered when the puzzle is solved.

The register that will be updated when a specific input is changed is the last 3 digits of the IP + a 0 + the number of the input. So the register "12300" will be updated when the status of input 0 on the prop with that IP address changes.

Outputs and relays are the same as inputs, except instead of a 0 it is a 1 for output and 2 for relay.

Example:  
![ccinputexample](https://user-images.githubusercontent.com/31215073/33387395-2215934e-d4fb-11e7-9eda-e61d8d5c74e5.png)

#### Output
Go to "System Setup" -> "ModBus Setup" -> Select the "Switches" tab.

The IP Address field in ClueControl must be set to the IP of the prop.

Here are a list of the commands for "# to send":
* Solve - 1
* Reset - 2
* Enable - 3
* Disable - 4
* Reset and enable - 5
* Reset and disable - 6

To set the value of a specific output a 3 digit combination is used.
* The first digit is the type of output. 1 for output and 2 for relay.
* The second digit is the number of the output you are targeting.
* The third digit is the status you want to update it to. 1 for on 0 for off.

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

All communication with the prop is done in the MQTT topic "/myChannel/myProp" where myChannel is the specified channel name and myProp is the specified prop name. As a naming convention you can append the last three digits of the IP address to the prop name to help with identification. I.e. if your prop has a static IP of 192.168.1.123, name your prop "prop123".
 
Inputs and outputs are sent through the topic as JSON objects.  

#### Input
When the prop detects a state change of an input, it sends the following json object:
```json
{
    "DIRECTION":"FROM",
    "TYPE":"INPUT",
    "INDEX":[index],
    "VALUE":[value]
}
```
"FROM" indicates that this is a message from the prop to the MQTT server. The [index] will be the input index (i.e. 0 is INPUT0, 1 is INPUT1…). The value will be the digital read of the pin (either 0 for LOW or 1 for HIGH).

Example
```json
{
    "DIRECTION":"FROM",
    "TYPE":"INPUT",
    "INDEX":0,
    "VALUE":1
}
```
This is a state change from the prop indicating INPUT0 is now HIGH. 


#### Output
To set an output, send a json object as follows:
```json
{
    "DIRECTION":"TO",
    "TYPE":"OUTPUT",
    "INDEX":[index],
    "VALUE":[value]
}
```
"TO" indicates that this is a message to the prop from the MQTT server. [value] should be 1 to turn an output on or 0 to turn an output off.

Example
```json
{
    "TYPE": "OUTPUT",
    "OUTPUT": 0,
    "VALUE": 1
}
```
This is a command to the prop that will set OUTPUT0 to HIGH.

#### All Commands
[Here is a comprehensive list of all commands and further documentation]( https://github.com/anidea/Support-Documentation/tree/master/FX%20Controller%20Networking/MQTT%20and%20Node%20Red)

#### Node-RED
An easy way to work with the MQTT interface is using Node-RED. To get started with our example, copy the contents of [this json file]( https://raw.githubusercontent.com/anidea/Support-Documentation/master/FX%20Controller%20Networking/MQTT%20and%20Node%20Red/NodeRED%20Example.json). Then in Node-RED, go to the menu, select Import -> Clipboard. Paste the file contents into the window, select "new flow" and click Ok. Click to position the nodes. In the From MQTT and To MQTT nodes, change the topic to match your prop settings. Finally press Deploy and you should be connected!
