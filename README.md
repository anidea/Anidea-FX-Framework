# Anidea FX Controller

This application is targeted for the AEI FX series of boards, an Arduino compatible series of controllers from Anidea Engineering.  While this program is free (MIT LECENSE), please consider purchasing and FX300 to support us making more free code.

You can purchase a FX300 at http://get.anidea-engineering.com/fx300_order_request

Generic framework for running different games on escape room props and connecting them with different management software.

## General operation:

Each game and each network have both a header and .cpp file that define their operation. Any new game or network added must have its header (.h) file included in the main .ino file.

At the top of the includes there are several boards included. Only one can be uncommented to tell the program which pin definitions and functions to use.

Below where the games are included there are several declarations of "myGame". Only one can be uncommented to tell the program which game to run.

The next section is the network variables. These need to be configured for the specific device as well as for the management software.

Lastly is the network section Only one can be uncommented to tell the program which management software to communicate with.

```
// Uncomment only one of these lines for the board you want
#include "fx300.h"
//#include fx###.h

#include "game.h"
#include "network.h"

// Networks
#include "escaperoommaster.h"
#include "cluecontrol.h"

// Include game headers here
#include "game_simplegame.h"
#include "game_lightsout.h"
#include "game_senet.h"
#include "game_sequencedetect.h"
#include "game_sixwire.h"
```

```
// Uncomment only one of these lines for the game you want
//  myGame = new simplegame(); //Simple game provided as an example
//  myGame = new lightsout(); //Lights out game description
//  myGame = new senetgame(); //PR Senet game description
//  myGame = new sequencedetect(); //Sequencedetect description
//  myGame = new sixwire(); //Sixwire description

byte MyMac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5 };   // This must be unique for each device
IPAddress MyIP(10, 0, 1, 210);                          // This must be unique for each device
IPAddress gateway(10, 0, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress HostIP(10,0,1,115);                           // This should be the IP of the device running the management software
int CCRegister = 5; // Register for Clue Control

// Uncomment only one of these lines for the network you want
//  myNetwork = new escaperoommaster(MyMac, MyIP, gateway, subnet, HostIP);
//  myNetwork = new cluecontrol(MyMac, MyIP, gateway, subnet, HostIP, CCRegister);
//  myNetwork = new nodered(MyMac, MyIP, gateway, subnet, HostIP);
```
