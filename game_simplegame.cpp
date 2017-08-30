#include "fx300.h"
#include "game_simplegame.h"

/*
  Simple test game
  Provides an example for how to configure a game. The game is solved once a button is pressed 3 times. A light turns on when it is solved.

  This application is targeted for the FX300, an Arduino compatible controller from Anidea Engineering.  
  While this program is free, (MIT LECENSE) please consider purchasing and FX300 to support me making more free code.

  2017 Gabriel Goldstein
  Anidea Engineering
  http://www.anidea-engineering.com

  written and tested with Arduino 1.8.1

   -------------------
*/


/*
  // pin assignments

   AEI FX300
   ------------------------
 * *INPUTS*
    1 input defined by buttonPin that is connected to VIN through a button

 * *OUTPUTS*
    1 output defined by lightPin that is connected to the ground terminal of an LED that is connected through a resistor to VIN.


  General operation:

  STARTUP
    Turn power on

  RESET
    Called at startup, after programming and when the reset input is activated
    Turn on the solved output
    Clear any in progress game play.
    Go to game play.

  GAME PLAY
   Once the button is pressed 3 times, the game is won and solved is called
*/

simplegame::simplegame() : Game()
{
  // Things that need to be set up when the game is first created
  pinMode(RELAY0, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(OUTPUT0, OUTPUT);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);
  pinMode(OUTPUT3, OUTPUT);
  pinMode(OUTPUT4, OUTPUT);
  pinMode(OUTPUT5, OUTPUT);
  pinMode(LED, OUTPUT);

  pinMode(INPUT0, INPUT);
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(INPUT3, INPUT);
  pinMode(INPUT4, INPUT);
  pinMode(INPUT5, INPUT);

  // Only enable the inputs/outputs that are not going to be used by this game
  INPUT0_OVERRIDE_ENABLE = 1;
  INPUT1_OVERRIDE_ENABLE = 1;
  INPUT2_OVERRIDE_ENABLE = 1;
  INPUT3_OVERRIDE_ENABLE = 1;
  INPUT4_OVERRIDE_ENABLE = 1;
  INPUT5_OVERRIDE_ENABLE = 0;

  OUTPUT0_OVERRIDE_ENABLE = 1;
  OUTPUT1_OVERRIDE_ENABLE = 1;
  OUTPUT2_OVERRIDE_ENABLE = 1;
  OUTPUT3_OVERRIDE_ENABLE = 1;
  OUTPUT4_OVERRIDE_ENABLE = 0;
  OUTPUT5_OVERRIDE_ENABLE = 1;

  RELAY0_OVERRIDE_ENABLE = 1;
  RELAY1_OVERRIDE_ENABLE = 1;

  Serial.println(F("simplegame initialized"));
  gameName = F("simplegame");

  reset();
}

void simplegame::loop(void)
{
  // put your main code here, to run repeatedly:
  
  // Do generic loop actions
  Game::loop();
  
  int buttonstate = digitalRead(buttonPin);

  switch (_gameState)
  {
    case GAMESTATE_START:
      // Setup state

      // run reset
      reset();

      _gameState++;             // Run show
      break;

    case GAMESTATE_START + 1:
      _gameState = GAMESTATE_RUN;             // Run puzzle
      break;

    case GAMESTATE_RUN:
      if (buttonstate == 1 && pressflag == 0)
      {
        presscounter++;
        pressflag = 1;
      }
      else if (buttonstate == 0)
      {
        pressflag = 0;
      }
      
      if (presscounter >= 3)
      {
        Serial.print(F("presscounter > 3"));
        _gameState = GAMESTATE_SOLVED;  // Win
        break;
      }
      break;

    case GAMESTATE_SOLVED:
      // Run solved and any other one time routine
      solved();

      break;
 
    default:
    case GAMESTATE_ENDLOOP:
      // Wait for reset

      break;
  }
}

void simplegame::forceSolved(void)
{
  Serial.println(F("simplegame forceSolved"));

  //Call generic forceSolve function
  Game::forceSolved();
  
  //Do game specific solved state
  digitalWrite(lightPin, HIGH);
}

void simplegame::solved(void)
{
  Serial.println(F("simplegame solved"));

  //Call generic solve function
  Game::solved();

  //Do game specific solved state
  digitalWrite(lightPin, HIGH);
}

void simplegame::reset(void)
{
  Serial.println(F("simplegame reset"));

  //Reset global game variables
  Game::reset();

  //Reset game specific variables
  presscounter = 0;
  pressflag = 0;
  digitalWrite(lightPin, LOW);
}
