#include "game.h"

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

  for(int i = 0; i < NUM_INPUTS; ++i) //Or loop the array and init them.
      INPUT_OVERRIDE_ENABLE[i] = true;

  for(int i = 0; i < NUM_OUTPUTS; ++i) //Or loop the array and init them.
      OUTPUT_OVERRIDE_ENABLE[i] = true;

  for(int i = 0; i < NUM_RELAYS; ++i) //Or loop the array and init them.
      RELAY_OVERRIDE_ENABLE[i] = true;

  Serial.println(F("Game object initialized"));

  reset();
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

  // If enabled at all, detect input state
  if (GAME_INPUT_RESET)
  {
    if (digitalRead(GAME_INPUT_RESET) == 1)   // Check for reset and kill game if active
    {
      reset();
      return;
    }
  }
 
  // If enabled at all, detect input state
  if (GAME_INPUT_ENABLE)
  {
    if (digitalRead(GAME_INPUT_ENABLE) == 1)   // Don't run the game if enable is high, must be low
    {
      return;
    }
  }
}

///////////////////////////////////
// void forceSolved(void)
// Called to force the game solved
//
void Game::forceSolved(void)
{
  // Routine run when the puzzle is solved
  Serial.println(F("Puzzle Force Solved!"));
  _puzzleSolved = 1;
  _gameState = GAMESTATE_ENDLOOP;

  // Call host call back
  if (solvedCallback) solvedCallback();
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

  // Call host call back
  if (solvedCallback) solvedCallback();
}

///////////////////////////////////
// void reset(void)
// Called to reset the game, called internally and externally
//
void Game::reset(void)
{
  // Routine run to reset the puzzle.  Run at start or by other means
  
  Serial.println(F("Game Reset"));
  
  // Reset game vars
  _puzzleSolved = 0;
  _gameState = GAMESTATE_START;
}
