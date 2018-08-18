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

	FX300/FX350/FX450
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
  

  // Only enable the inputs/outputs that are not going to be used by this game
  INPUT_OVERRIDE_ENABLE[0] = 1;
  INPUT_OVERRIDE_ENABLE[1] = 1;
  INPUT_OVERRIDE_ENABLE[2] = 1;
  INPUT_OVERRIDE_ENABLE[3] = 1;
  INPUT_OVERRIDE_ENABLE[4] = 1;
  INPUT_OVERRIDE_ENABLE[5] = 0;

  OUTPUT_OVERRIDE_ENABLE[0] = 1;
  OUTPUT_OVERRIDE_ENABLE[1] = 1;
  OUTPUT_OVERRIDE_ENABLE[2] = 1;
  OUTPUT_OVERRIDE_ENABLE[3] = 1;
  OUTPUT_OVERRIDE_ENABLE[4] = 0;
  OUTPUT_OVERRIDE_ENABLE[5] = 1;

  RELAY_OVERRIDE_ENABLE[0] = 1;
  RELAY_OVERRIDE_ENABLE[1] = 1;

  Serial.println(F("simplegame initialized"));

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
        Serial.println("Button pressed");
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
  // Only define this function if something specific needs to be done when the prop is solved manually

  //Call generic forceSolve function
  Game::forceSolved();
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

