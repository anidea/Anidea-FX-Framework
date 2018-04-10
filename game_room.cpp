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
		
		// Turn on respective OUTPUTS for each detected INPUT.
		for (uint8_t i = startIndex; i < NUM_INPUTS; i++)
		{
			if (digitalRead(INPUTS[i]) == HIGH)
			{
				if (!OUTPUT_STATES[i]) OUTPUT_STATES_FLAG[i] = true;
				OUTPUT_STATES[i] = true;
			}
		}
		
		// Tie RELAY0 to OUTPUT4
		if (OUTPUT_STATES[4])
		{
			if (RELAY_STATES[4]) RELAY_STATES_FLAG[4] = true;
			RELAY_STATES[4] = false;
		}
		
		//Tie RELAY1 to OUTPUT5
		if (OUTPUT_STATES[5])
		{
			if (RELAY_STATES[5]) RELAY_STATES_FLAG[5] = true;
			RELAY_STATES[5] = false;
		}
	  }
      break;
  }
}

void room::reset(void)
{
  //Reset global game variables
  Game::reset();
  
  Serial.println(F("room reset"));
  
  digitalWrite(RELAY0, HIGH);
  digitalWrite(RELAY1, HIGH);
}
