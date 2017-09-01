#include <EEPROM.h>

#include "fx300.h"
#include "game_inputsequence.h"

/*

  FX300 input sequence detector
  Tests for sequence of inputs and solves the puzzle if they are in the correct order

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
    Button or hall inputs are specified in a list below.  High is active

    Enable, when low, the game is enabled

    Reset, when high, the game is reset

 * *OUTPUTS*
    Outout 0 will turn on when an input is activated or when the puzzle is solved

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

   If enbaled below, RESET high will hold the game in reset and clear the game play

   If enabled below, ENABLE low will allow the game to be played

   Once the entire sequence is entered, the solved routine is run and the solved relay will disengage.
   
*/

inputsequence::inputsequence() : Game()
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
  INPUT_OVERRIDE_ENABLE[0] = 0;
  INPUT_OVERRIDE_ENABLE[1] = 0;
  INPUT_OVERRIDE_ENABLE[2] = 0;
  INPUT_OVERRIDE_ENABLE[3] = 0;
  INPUT_OVERRIDE_ENABLE[4] = 0;
  INPUT_OVERRIDE_ENABLE[5] = 0;

  OUTPUT_OVERRIDE_ENABLE[0] = 0;
  OUTPUT_OVERRIDE_ENABLE[1] = 1;
  OUTPUT_OVERRIDE_ENABLE[2] = 1;
  OUTPUT_OVERRIDE_ENABLE[3] = 1;
  OUTPUT_OVERRIDE_ENABLE[4] = 1;
  OUTPUT_OVERRIDE_ENABLE[5] = 1;

  RELAY_OVERRIDE_ENABLE[0] = 0;
  RELAY_OVERRIDE_ENABLE[1] = 1;

  Serial.println(F("inputsequence initialized"));
  gameName = F("inputsequence");

  reset();

  // Check for anything in the eeprom
  if (EEPROM.read(0) == 0xFF)
  {
    // value out of range, go right into record sequence mode
    recordInputButtonSequence();
  }
  
}

void inputsequence::loop(void)
{
  // put your main code here, to run repeatedly:
  
  // Do generic loop actions
  Game::loop();

  // Detect programming mode
  if (analogRead(HALL) < HALL_NORTH_THRESH) // Detect a north pole of a magnet
  {
    // Programming mode entered
    recordInputButtonSequence();
  }

  int scanCurrentInputButton;

  switch (_gameState)
  {
    case GAMESTATE_START:
      // Setup state

      // run reset
      reset();

      _gameState++;
      break;

    case GAMESTATE_START + 1:
      _gameState = GAMESTATE_RUN;             // Run puzzle
      break;

    case GAMESTATE_RUN:

      if (GAME_MAX_SOLVE_TIME)
      {
        if ((_gameResetDownTimer == 0) && (_inputSequencePosition > 0))
        {
          Serial.println(F("Game Timer Expired"));
          
          // Fail, reset
          _gameState = 0; // Reset puzzle
          
        }
      }
  
      scanCurrentInputButton = scanInputsSteady();

      if (0xFF == EEPROM.read(_inputSequencePosition))
      {
        // Sequence end detected

        _gameState = GAMESTATE_SOLVED;
        
      }else{

        if (_waitForNoInput == 1)
        {
          if (scanCurrentInputButton == NO_INPUT_DETECTED)
          {
            _waitForNoInput = 0;          
          }
        }

        if (_waitForNoInput == 0)
        {
          if (scanCurrentInputButton == EEPROM.read(_inputSequencePosition))
          {
            // Current button read is the same as the recorded sequence
  
            _waitForNoInput = 1;
  
            // First good button
            if (_inputSequencePosition == 0)
            {  
              Serial.println(F("First good button"));
  
              if (GAME_MAX_SOLVE_TIME)
              {
                _gameResetDownTimer = GAME_MAX_SOLVE_TIME; // Reset max time
              } 
            }
  
            Serial.print(F("Game Sequence "));
            Serial.print(_inputSequencePosition);
            Serial.print(F(" input "));
            Serial.print(scanCurrentInputButton);
            Serial.println(F(" read"));
  
            // Next in sequence
            _inputSequencePosition++;
           
          }else{
            // No button or wrong button
            if ((scanCurrentInputButton != NO_INPUT_DETECTED) && (scanCurrentInputButton != _scanPreviousInputButton))
            {
              // Wrong button
  
              // Reset sequence
  
              Serial.println(F("Wrong button, sequence reset"));
    
              _gameState = 0;  // Reset the game sequence
             
            }
          }
        }
      }

      // Store last so we can look for the change
      _scanPreviousInputButton = scanCurrentInputButton;
      
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

void inputsequence::forceSolved(void)
{
  Serial.println(F("inputsequence forceSolved"));

  //Call generic forceSolve function
  Game::forceSolved();
  
  //Do game specific solved state
  digitalWrite(feedbackLight, HIGH);        // Turn Off maglock
  digitalWrite(solvedOutput, LOW);          // Mimick LED for solved 
}

void inputsequence::solved(void)
{
  Serial.println(F("inputsequence solved"));

  //Call generic solve function
  Game::solved();

  //Do game specific solved state
  digitalWrite(feedbackLight, HIGH);    // Turn on maglock
  digitalWrite(solvedOutput, LOW);           // Mimick LED for solved
}

void inputsequence::reset(void)
{
  Serial.println(F("inputsequence reset"));

  //Reset global game variables
  Game::reset();

  //Reset game specific variables
  _inputSequencePosition = 0;

  // Set outputs
  digitalWrite(feedbackLight, LOW);    // Turn on maglock
  digitalWrite(solvedOutput, HIGH);           // Mimick LED for solved 
}

void inputsequence::recordInputButtonSequence(void)
{
  // Will record upto GAME_MAX_SEQUENCE from the inputs using selected input filter

  int inputSequencePosition = 0;
  
  Serial.println(F("Programming mode"));

  while(true)
  {

    // Look for exit
    if (analogRead(HALL) > HALL_SOUTH_THRESH) // Detect a south pole of a magnet
    {
      break;
    }

    int inputButton = scanInputsSteady();

    if (inputButton == NO_INPUT_DETECTED)
    {
      Serial.println(F("No input detected..."));
      
    }
    else
    {
      // Use first light to acknolwege input
      digitalWrite(feedbackLight, HIGH);

      // Save the input
      EEPROM.write(inputSequencePosition, inputButton);

      Serial.print(F("Input "));
      Serial.print(inputButton);
      Serial.println(F(" recorded."));

      inputSequencePosition++;  //Record next button

      // Wait for no input before continuing
      while(scanInputsSteady() != NO_INPUT_DETECTED){};

      // Use first light to acknolwedge input, turn off
      digitalWrite(feedbackLight, LOW);
    }
    
  }

  // Programming mode ended
  EEPROM.write(inputSequencePosition, 0xFF);    // End of the sequence

  Serial.println(F("Programming mode complete"));
  
  int i = 0;
  uint8_t input;
  
  Serial.println(F("Sequence "));

  while((input = EEPROM.read(i)) != 0xFF)
  {
    Serial.println(input, DEC);
    i++;
  }
    
  Serial.println(F("Programmed"));
}

int inputsequence::scanInputsSteady(void)
{
  /*
  Scan all inputs looking for a steady state for a period of time. While this isn't
  important for buttons, when you have many hall sensors that may get triggered as
  magnets pass over them, we need to do some heavy filtering and look for a steady
  input for a period of time
  
  
  One caveat, if you have a really crappy input or the users don't settle on an input, the routine will stay here indefinitely

  TODO:  Add a max out timer
  
  */

  if (GAME_INPUT_COUNT > 8)
  {
    Serial.println(F("error Too many inputs for the number of bits"));
  }
  
  // Increase size types if ever on a machine with more inputs
  uint8_t allInputs, allPreviousInputs;
  uint8_t activeInputCount;
  int activeInput;
  
  uint8_t steadyStateCounter = STEADY_STATE_INPUTS_COUNT;
  
  
  while(1)
  {
    allInputs = 0;    // Clear all bits
    activeInputCount = 0;
    activeInput = NO_INPUT_DETECTED;
    
    // Load all inputs ports into a single byte for easier comparison
    for (int i = 0; i < GAME_INPUT_COUNT; i++)
    {
      if (digitalRead(_inputButtonPinList[i]) == 1) // Bit active
      {
        allInputs |= 1;     // Set bit
        
        allInputs <<= 1;    // Shift up

        activeInput = i;   // Save the current button, this is only used if a single input 

        activeInputCount++;
      }
    }

    // Test for steady, single input
    if ((allInputs != allPreviousInputs) || (activeInputCount > 1))
    {
      // Reset counter is more than one input is active or they are changing
      steadyStateCounter = STEADY_STATE_INPUTS_COUNT; 

      /*
      Serial.print("Counter reset: inputs ");
      Serial.print(allInputs, HEX);
      Serial.print(", Count ");
      Serial.print(activeInputCount, DEC);
      Serial.println();
      */
    }


    // Check to see if we are done
    if (steadyStateCounter == 0)
    {
      // We have been steady with a single input for some time now

      /*
      Serial.print("Active input returned ");
      Serial.print(activeInput, DEC);
      Serial.println();
      */
      return (activeInput);

    }

    // Store last scane
    allPreviousInputs = allInputs;

    steadyStateCounter--;   // Wait for steady state


    delay(STEADY_STATE_INPUTS_TIME);  // Wait a scan time

    
  }
  
}
