#include "fx300.h"
#include "game_senet.h"

///////////////////////////////////
// Constructor for the game
//
senetgame::senetgame() : Game()
{
  pinMode(REED_PIN_1, INPUT);
  pinMode(REED_PIN_2, INPUT);
  pinMode(REED_PIN_3, INPUT);
  pinMode(REED_PIN_4, INPUT);
  pinMode(REED_PIN_5, INPUT);
  pinMode(REED_PIN_6, INPUT);
  pinMode(RELAY_PIN, OUTPUT);   // Control mag lock or lights

  Serial.println("PR Senet Game");

  reset();
}

void senetgame::loop(void)
{
  // Do generic loop actions
  Game::loop();

  // Read the switch value into a variable
  int reed1 = digitalRead(REED_PIN_1);
  int reed2 = digitalRead(REED_PIN_2);
  int reed3 = digitalRead(REED_PIN_3);
  int reed4 = digitalRead(REED_PIN_4);
  int reed5 = digitalRead(REED_PIN_5);
  int reed6 = digitalRead(REED_PIN_6);

  // Print out the value of the switches
  Serial.print(reed1);
  Serial.print(reed2);
  Serial.print(reed3);
  Serial.print(reed4);
  Serial.print(reed5);
  Serial.print(reed6);
  Serial.println();

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
      if (reed1 == LOW && reed2 == LOW && reed3 == LOW && reed4 == LOW && reed5 == LOW && reed6 == LOW) {
        if (! relayTriggered) {
          _gameState = GAMESTATE_SOLVED;  // Win
          break;
        }
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

void senetgame::solved(void)
{
  Serial.println("senetgame solved");

  //Call generic solve function
  Game::solved();

  //Do game specific solved state
  relayTriggered = true;
  digitalWrite(RELAY_PIN, HIGH);
}

void senetgame::reset(void)
{
  Serial.println("senetgame reset");

  //Reset global game variables
  Game::reset();

  //Reset game specific variables
  relayTriggered = false;
  digitalWrite(RELAY_PIN, LOW);
}
