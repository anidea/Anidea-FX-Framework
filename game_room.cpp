#include "game_room.h"

room::room() : Game()
{
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

  Serial.println(F("room initialized"));

  reset();
}

void room::loop(void)
{
  // put your main code here, to run repeatedly:
  
  // Do generic loop actions
  Game::loop();

  if (digitalRead(INPUT0) == HIGH && digitalRead(INPUT1) == LOW && button1Flag == false && buttonState == 0)
  {
    button1Flag = true;
    button1Timer = millis();
  }
  if (digitalRead(INPUT0) == LOW && digitalRead(INPUT1) == HIGH && button2Flag == false && buttonState == 0)
  {
    button2Flag = true;
    button2Timer = millis();
  }
  if (digitalRead(INPUT0) == HIGH && digitalRead(INPUT1) == HIGH)
  {
    buttonState = 5;
    button1Flag = false;
    button2Flag = false;
  }
  if (digitalRead(INPUT0) == LOW && digitalRead(INPUT1) == LOW && buttonState == 0)
  {
    if (millis() - button1Timer > 10 && button1Flag == true)
    {
//      Serial.println("Spot 1");
      if (millis() - button1Timer < 500)
      {
//        Serial.println("Spot 2");
        buttonState = 1;
      }
      else
      {
        buttonState = 3;
      }
      button1Flag = false;
    }
    if (millis() - button2Timer > 10 && button2Flag == true)
    {
      if (millis() - button2Timer < 500)
      {
        buttonState = 2;
      }
      else
      {
        buttonState = 4;
      }
      button2Flag = false;
    }
  }

  switch (_gameState)
  {
    case GAMESTATE_START:
      if (_enabled)
      {
        _gameState = GAMESTATE_RUN;
      }
      break;

    case GAMESTATE_RUN:
	 // Puzzle logic here
      
      break;

    case GAMESTATE_SOLVED:
      // Run solved and any other one time routine
      solved();
      _gameState = GAMESTATE_ENDLOOP;

      break;
 
    default:
    case GAMESTATE_ENDLOOP:
      // Wait for reset
      
      break;
  }
}

void room::solved(void)
{
  Serial.println(F("room solved"));

  //Call generic solve function
  Game::solved();
}

void room::reset(void)
{
  Serial.println(F("room reset"));

  //Reset global game variables
  Game::reset();
}
