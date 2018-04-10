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
	  {
		#ifdef ONEBUTTON
		int startIndex = 1;
		#elif defined(TWOBUTTON)
		int startIndex = 2;
		#endif
		for (uint8_t i = startIndex; i < NUM_INPUTS; i++)
		{
			if (digitalRead(INPUTS[i]) == HIGH) // If Solved
			{
				// Signal Solved
				OUTPUT_STATES[i] = true;
				OUTPUT_STATES_FLAG[i] = true;

				if (i == 4 || i == 5)
				{
					RELAY_STATES[i] = false;
					RELAY_STATES_FLAG[i] = true;
				}
			}
		}
	  }
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
  
  digitalWrite(RELAY0, HIGH);
  digitalWrite(RELAY1, HIGH);

  //Reset global game variables
  Game::reset();
}
