#include "fx300.h"
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
      Serial.println("Spot 1");
      if (millis() - button1Timer < 500)
      {
        Serial.println("Spot 2");
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

  if (digitalRead(OUTPUT4) == HIGH)
  {
    digitalWrite(RELAY0, HIGH);
  }
  if (digitalRead(OUTPUT5) == HIGH)
  {
    digitalWrite(RELAY1, HIGH);
  }

  switch (_gameState)
  {
    case GAMESTATE_START:
      digitalWrite(OUTPUT0, LOW);
      digitalWrite(OUTPUT1, LOW);
      if (_enabled)
      {
        _gameState = GAMESTATE_RUN;
      }
      break;

    case GAMESTATE_RUN:
      digitalWrite(OUTPUT0, HIGH);
      digitalWrite(OUTPUT1, LOW);
      for (int i = 2; i < 6; i++)
      {
        if (digitalRead(INPUTS[i]) == HIGH)
        {
          digitalWrite(OUTPUTS[i], HIGH);
        }
      }
    
      solvedFlag = true;
      for (int i = 2; i < 6; i++)
      {
        if (digitalRead(OUTPUTS[i]) == LOW)
        {
          solvedFlag = false;
        }
      }
      if (solvedFlag == true && _puzzleSolved == 0)
      {
        _gameState = GAMESTATE_SOLVED;
      }
      break;

    case GAMESTATE_SOLVED:
      // Run solved and any other one time routine
      digitalWrite(OUTPUT0, HIGH);
      digitalWrite(OUTPUT1, HIGH);
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

  //Do game specific solved state
  for (int i = 0; i < 6; i++)
  {
    digitalWrite(OUTPUTS[i], HIGH);
  }
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(RELAYS[i], LOW);
  }
}

void room::reset(void)
{
  Serial.println(F("room reset"));

  //Reset global game variables
  Game::reset();

  //Reset game specific variables
  for (int i = 0; i < 6; i++)
  {
    digitalWrite(OUTPUTS[i], LOW);
  }
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(RELAYS[i], LOW);
  }
}
