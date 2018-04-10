#include "game.h"

bool Game::INPUT_STATES[NUM_INPUTS] = {false};
bool Game::OUTPUT_STATES[NUM_OUTPUTS] = {false};
bool Game::OUTPUT_STATES_FLAG[NUM_OUTPUTS] = {false};
bool Game::RELAY_STATES[NUM_RELAYS] = {false};
bool Game::RELAY_STATES_FLAG[NUM_OUTPUTS] = {false};

///////////////////////////////////
// Constructor for the game
//
Game::Game()
{
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

  // Turn all overrides on
  for(int i = 0; i < NUM_INPUTS; ++i) //Or loop the array and init them.
      INPUT_OVERRIDE_ENABLE[i] = true;

  for(int i = 0; i < NUM_OUTPUTS; ++i) //Or loop the array and init them.
      OUTPUT_OVERRIDE_ENABLE[i] = true;

  for(int i = 0; i < NUM_RELAYS; ++i) //Or loop the array and init them.
      RELAY_OVERRIDE_ENABLE[i] = true;

  Serial.println(F("Game object initialized"));
}

///////////////////////////////////
// void tick(void)
// Call periodically, nominally every 100ms with a real hardware counter
//
void Game::tick(void)
{
  static int freeRunningTimer;

  freeRunningTimer++;
}

///////////////////////////////////
// void loop(void)
// Called in a continious loop from the arduino loop
// This function should exit and not be held up for long periods of time
//
void Game::loop(void)
{
  // put your main code here, to run repeatedly:

  delay(10); // Keep game running at a reasonable rate, not micro speed. 100x a second, ehternet adds significant delays

  // Read input states
  for (int i = 0; i < NUM_INPUTS; i++)
  {
      INPUT_STATES[i] = digitalRead(INPUTS[i]);
  }

  // Write output states
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (OUTPUT_OVERRIDE_ENABLE[i] == true)
    {
      if (OUTPUT_STATES_FLAG[i] == true)
      {
        digitalWrite(OUTPUTS[i], OUTPUT_STATES[i]);
        OUTPUT_STATES_FLAG[i] = false;
      }
    }
  }

  // Write relay states
  for (int i = 0; i < NUM_RELAYS; i++)
  {
    if (RELAY_OVERRIDE_ENABLE[i] == true)
    {
      if (RELAY_STATES_FLAG[i] == true)
      {
        digitalWrite(RELAYS[i], RELAY_STATES[i]);
        RELAY_STATES_FLAG[i] = false;
      }
    }
  }
}

///////////////////////////////////
// void forceSolved(void)
// Called to force the game solved
//
void Game::forceSolved(void)
{
  Serial.println(F("Puzzle Force Solved!"));
  solved();
}

///////////////////////////////////
// void solved(void)
// Called when the game is solved
//
void Game::solved(void)
{
  // Routine run when the puzzle is solved
  Serial.println(F("Puzzle Solved!"));
  _puzzleSolved = 1;
  _gameState = GAMESTATE_ENDLOOP;
}

///////////////////////////////////
// void reset(void)
// Called to reset the game, called internally and externally
//
void Game::reset(void)
{
  // Routine run to reset the puzzle.  Run at start or by other means
    // Set outputs to all low to start
	for (int i = 0; i < NUM_OUTPUTS; ++i) //Or loop the array and init them.
	{
		OUTPUT_STATES[i] = 0;
		OUTPUT_STATES_FLAG[i] = false;
		digitalWrite(OUTPUTS[i], LOW);
	}

	for (int i = 0; i < NUM_RELAYS; ++i) //Or loop the array and init them.
	{
		RELAY_STATES[i] = 0;
		RELAY_STATES_FLAG[i] = false;
		digitalWrite(RELAYS[i], LOW);
	}
	  
  Serial.println(F("Game Reset"));
  
  // Reset game vars
  _puzzleSolved = 0;
  _gameState = GAMESTATE_START;
}

///////////////////////////////////
// void enable(void)
// Called to enable the game
//
void Game::enable(void)
{
  Serial.println(F("Game enabled"));
  
  // Set game vars
  _enabled = true;
}

///////////////////////////////////
// void disable(void)
// Called to disable the game
//
void Game::disable(void)
{
  Serial.println(F("Game disabled"));
  
  // Set game vars
  _enabled = false;
}
