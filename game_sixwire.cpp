#include "fx300.h"
#include "game_sixwire.h"

/*
  FX300 6 wires prop controller
  Tests for up to 6 connections for patch panels or general connections
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
    Inputs are specified in a list below.  This game interates through up to 6 input/output lines and makes sure they are connected properly.
    Each input needs a 1K pullup to VIN (12V or 24V)
 * *OUTPUTS*
    Outputs are correlated with inputs.  
 * *RELAY OUTPUTS*
   SOLVED       (Connect to relay1 COM and NO or NC, depending on requirements, the SOLVED state will connect COM to NO)
   
 * ***  SOLVED is run backward to provide safe operation such that if the prop dies or isn't powered, the maglock opens
  General operation:
  STARTUP
    Turn power on
  RESET
    Called at startup, after programming and when the reset input is activated
    Turn on the solved output
    Clear any in progress game play.
    Go to game play.
  GAME PLAY
   Outputs are driven insequence and monitored by the inputs.
   When each input reads the same as each output, we know the patch cables are properly placed
   Jumper a 1K between VIN and each input.  Each input/output pair is looking for connectivity, meaning a connection between the input and output.
   When all outputs are connected to the respective inputs, the puzzle is solved
   
*/

sixwire::sixwire() : Game()
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
  INPUT0_OVERRIDE_ENABLE = 0;
  INPUT1_OVERRIDE_ENABLE = 0;
  INPUT2_OVERRIDE_ENABLE = 0;
  INPUT3_OVERRIDE_ENABLE = 0;
  INPUT4_OVERRIDE_ENABLE = 0;
  INPUT5_OVERRIDE_ENABLE = 0;

  OUTPUT0_OVERRIDE_ENABLE = 0;
  OUTPUT1_OVERRIDE_ENABLE = 0;
  OUTPUT2_OVERRIDE_ENABLE = 0;
  OUTPUT3_OVERRIDE_ENABLE = 0;
  OUTPUT4_OVERRIDE_ENABLE = 0;
  OUTPUT5_OVERRIDE_ENABLE = 0;

  RELAY0_OVERRIDE_ENABLE = 1;
  RELAY1_OVERRIDE_ENABLE = 0;

  Serial.println(F("sixwire initialized"));
  gameName = F("sixwire");

  reset();
}

void sixwire::loop(void)
{
  // put your main code here, to run repeatedly:
  
  // Do generic loop actions
  Game::loop();
  
  int allMatched = 1;

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
      // check for proper wiring

      // Iterate through all the patch cables
      for (int i = 0; i < PATCH_COUNT; i++)
      {
        // Set outputs
        for (int j = 0; j < PATCH_COUNT; j++)
        {
          if (i == j)
          {
            digitalWrite(outputPinList[j], HIGH); // Turn on one high at a time
          }else{
            digitalWrite(outputPinList[j], LOW); // Turn off the rest
          }
        }

        delay(10);  // Let all the pins settle

        // Read inputs
        for (int j = 0; j < PATCH_COUNT; j++)
        {
          if (i == j)
          {
            if (digitalRead(inputPinList[j]) == HIGH)  // Should be low when wired correctly
            {
              allMatched = 0;
              
              Serial.print(F("Failed Match Low "));
              Serial.print(i);
              Serial.print(F(", "));
              Serial.print(j);
              Serial.println();

              break;
            }
          }else{
            if (digitalRead(inputPinList[j]) == LOW)  // Should be high when wired correctly
            {
              allMatched = 0;

              Serial.print(F("Failed Match High "));
              Serial.print(i);
              Serial.print(F(", "));
              Serial.print(j);
              Serial.println("");

              break;
            }
          }
        }

        if (allMatched == 0)
        {
          // We only need one to fail

          #if 1     // Disable (0) if you want all cables always tested
          break;
          #endif

        }
      
      }

      if (allMatched == 1)
      {
      
        _gameState = GAMESTATE_SOLVED;  // Win

        Serial.println(F("Solved, state 0 to state 1"));

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

void sixwire::forceSolved(void)
{
  Serial.println(F("sixwire forceSolved"));

  //Call generic forceSolve function
  Game::forceSolved();
  
  //Do game specific solved state
  digitalWrite(SOLVED, LOW);        // Turn Off maglock
  digitalWrite(LED, HIGH);          // Mimick LED for solved
}

void sixwire::solved(void)
{
  Serial.println(F("sixwire solved"));

  //Call generic solve function
  Game::solved();

  //Do game specific solved state
  digitalWrite(SOLVED, LOW);        // Turn Off maglock
  digitalWrite(LED, HIGH);          // Mimick LED for solved
}

void sixwire::reset(void)
{
  Serial.println(F("sixwire reset"));

  //Reset global game variables
  Game::reset();

  //Reset game specific variables
  // Set outputs
  digitalWrite(SOLVED, HIGH);    // Turn on maglock
  digitalWrite(LED, LOW);           // Mimick LED for solved 
}

int sixwire::getDebouncedInput(int input)
{
  int inputCount = 0;
  
  for (int i = 0; i < DEBOUNCE_COUNT; i++)
  {
    if (digitalRead(input) == 1)
    {
      inputCount++;
    }else{
      inputCount--;
    }

    delay(DEBOUNCE_TIME);
  }

  if (inputCount >= DEBOUNCE_COUNT / 2)
  {
    return HIGH;
  }

  return LOW;
}
