#include <MsTimer2.h>

#include "fx300.h"
#include "game_sequencedetect.h"

/*

  FX300 6 input sequence detector
  Tests for sequence of up to 6 inputs and controls output lights accordingly

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
    Light outputs either turn on in sequence when the correct input is received or they can illustrate the pattern in a simon says mode

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

   Once enabled, the light sequence will start.  If simon says, then the pattern will play.  If sequence mode, all lights will be off until a a correct input is received.  Simon Mode is not progressive.  It shows a pattern that must be replicated.

   As correct inputs are received, the outputs will progress giving positive feedback to the player

   Once the entire sequence is entered, the solved routine is run and the solved relay will disengage.
   
   
*/

///////////////////////////////////
// Constructor for the game
//
sequencedetect::sequencedetect() : Game()
{
  // Only enable the inputs/outputs that are not going to be used by this game
  INPUT_OVERRIDE_ENABLE[0] = 0;
  INPUT_OVERRIDE_ENABLE[1] = 0;
  INPUT_OVERRIDE_ENABLE[2] = 0;
  INPUT_OVERRIDE_ENABLE[3] = 0;
  INPUT_OVERRIDE_ENABLE[4] = 0;
  INPUT_OVERRIDE_ENABLE[5] = 0;

  OUTPUT_OVERRIDE_ENABLE[0] = 0;
  OUTPUT_OVERRIDE_ENABLE[1] = 0;
  OUTPUT_OVERRIDE_ENABLE[2] = 0;
  OUTPUT_OVERRIDE_ENABLE[3] = 0;
  OUTPUT_OVERRIDE_ENABLE[4] = 0;
  OUTPUT_OVERRIDE_ENABLE[5] = 0;

  RELAY_OVERRIDE_ENABLE[0] = 1;
  RELAY_OVERRIDE_ENABLE[1] = 0;

  Serial.println(F("FX300 6 input sequence detector"));

  reset();
}
///////////////////////////////////
// void tick(void)
// Call periodically, nominally every 100ms with a real hardware counter
//
void sequencedetect::tick(void)
{
  static int freeRunningTimer;
  static int terminalCountLightIncrement = GAME_LIGHT_OUTPUT_SIMONSAYS_ON_TIME;
  static int currentLightSequence;
  static int currentLightOn;

  freeRunningTimer++;

  if (GAME_MAX_SOLVE_TIME)
  {
    if (_gameResetDownTimer) _gameResetDownTimer--;
  }

  if (GAME_LIGHT_OUTPUT_SIMONSAYS)
  {
    // Display sequnce in the background while puzzle is not trying to be solved
  
    if (_runLightSequence == 1) 
    {
      // No buttons pressed yet in sequence
  
      if (terminalCountLightIncrement)
      {
        terminalCountLightIncrement--;
      }else{
  
        // Increment ligths
  
        if (currentLightOn == 1)
        {
          // Turn lights off
          currentLightOn = 0;
          
          terminalCountLightIncrement = GAME_LIGHT_OUTPUT_SIMONSAYS_OFF_TIME;
  
          if (currentLightSequence >= 0)
          {
            // Turn off light only if we aren't at the beginning of the sequence
            digitalWrite(_outputLightsPinList[EEPROM.read(EEPROM_START + currentLightSequence)], LOW); // Turn off current light
          }
  
        }else{
          // Turn lights on
          currentLightOn = 1;
  
          terminalCountLightIncrement = GAME_LIGHT_OUTPUT_SIMONSAYS_ON_TIME;
  
          currentLightSequence++;
    
          int nextLight = EEPROM.read(EEPROM_START + currentLightSequence);
    
          if (nextLight == 0xFF)
          {
            // Last light - delay and start over
            currentLightSequence = -1; // Back at beginning, but make negative so we logic above can flow.
  
            terminalCountLightIncrement = GAME_LIGHT_OUTPUT_SIMONSAYS_GAP_TIME; // Set to alternate value since we reset
          }else{
            // Next light
            
            digitalWrite(_outputLightsPinList[EEPROM.read(EEPROM_START + currentLightSequence)], HIGH); // Turn on next light
            
          }
        }
      }
    }
  }
  
}

///////////////////////////////////
// void loop(void)
// Called in a continious loop from the arduino loop
// This function should exit and not be held up for long periods of time
//
void sequencedetect::loop(void)
{
  // put your main code here, to run repeatedly:

  // Do generic loop actions
  Game::loop();

    // Check for anything in the eeprom
  if (EEPROM.read(EEPROM_START + 0) == 0xFF)
  {
    // value out of range, go right into record sequence mode
    recordInputButtonSequence();
  }

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

      _runLightSequence = 1;       // Run light show (if enabled)

      _gameState = 10;             // Run puzzle

      break;

    case GAMESTATE_RUN:
      // Check for time experation

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

      if (0xFF == EEPROM.read(EEPROM_START + _inputSequencePosition))
      {
        // Sequence end detected

        _gameState = GAMESTATE_SOLVED;
        
      }else{

        if (_waitForNoInput == 1)
        {
          if (scanCurrentInputButton == NO_INPUT_DETECTED)
          {
            _waitForNoInput = 0;

            if (GAME_LIGHT_OUTPUT_SIMONSAYS)
            {
              // Turn off all lights when button released
              allLightsOnOff(LOW);
            }
          }
        }

        if (_waitForNoInput == 0)
        {
          if (scanCurrentInputButton == EEPROM.read(EEPROM_START + _inputSequencePosition))
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
  
              if (GAME_LIGHT_OUTPUT_SIMONSAYS)
              {
                // Turn all the lights off
    
                _runLightSequence = 0;
  
                allLightsOnOff(LOW);
              }
  
            }
  
  
  
            if (GAME_LIGHT_OUTPUT_SEQUENCE_ONEATATIME)
            {
              // Turn off previous led
              if (_inputSequencePosition > 0)
              {
                // can't do this if no lights are on, filter this
                digitalWrite(_outputLightsPinList[_inputSequencePosition - 1], LOW); // Turn off the current lamp to show things are progessing
              }
            }
            else if (GAME_LIGHT_OUTPUT_SIMONSAYS && 0) // This mode disabled in favor of a light while pushed mode
            {
              // Turn off previous led
              if (_scanPreviousInputButton > 0)
              {
                // can't do this if no lights are on, filter this
                digitalWrite(_outputLightsPinList[_scanPreviousInputButton], LOW); // Turn off the current lamp to show things are progessing
              }
            }
  
            Serial.print(F("Game Sequence "));
            Serial.print(_inputSequencePosition);
            Serial.print(F(" input "));
            Serial.print(scanCurrentInputButton);
            Serial.println(F(" read"));
          
  
            // Next in sequence
            _inputSequencePosition++;
  
            if (GAME_LIGHT_OUTPUT_SEQUENCE) 
            {
              if (_inputSequencePosition < SEQUENCE_GAME_LIGHT_OUTPUT)
              {
                // Make sure we don't try to turn on a light that doesn't exist, would be at least one at this point from the previous inc operation
                digitalWrite(_outputLightsPinList[_inputSequencePosition - 1], HIGH); // Turn on the next lamp to show things are progessing
              
              }
            }
            else if (GAME_LIGHT_OUTPUT_SIMONSAYS)
            {
              if (scanCurrentInputButton < SEQUENCE_GAME_LIGHT_OUTPUT)
              {
                // Make sure we don't try to turn on a light that doesn't exist, would be at least one at this point from the previous inc operation
                digitalWrite(_outputLightsPinList[scanCurrentInputButton], HIGH); // Turn on the next lamp to show things are progessing
              
              }
            }
    
            
           
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

///////////////////////////////////
// void solved(void)
// Called when the game is solved
//
void sequencedetect::solved(void)
{
  // Routine run when the puzzle is solved
  Serial.println(F("sequencedetect Solved!"));

  //Call generic solve function
  Game::solved();

  //Do game specific solved state
  _runLightSequence = 0;
  digitalWrite(SOLVED, LOW);        // Turn Off maglock
  digitalWrite(LED, HIGH);          // Mimick LED for solved 

  // Turn all the lights on - Victory!
  allLightsOnOff(HIGH);  //steve testing bullsh*t - because Steve helped so much in testing
}

///////////////////////////////////
// void reset(void)
// Called to reset the game, called internally and externally
//
void sequencedetect::reset(void)
{
  // Routine run to reset the puzzle.  Run at start or by other means
  
  Serial.println(F("sequencedetect Reset"));
  
  //Reset global game variables
  Game::reset();

  //Reset game specific variables
  _gameState = 0;
  _inputSequencePosition = 0;
  _runLightSequence = 1;

  // Set outputs
  digitalWrite(SOLVED, HIGH);    // Turn on maglock
  digitalWrite(LED, LOW);           // Mimick LED for solved 

  if (GAME_LIGHT_OUTPUT_NONE || GAME_LIGHT_OUTPUT_SEQUENCE || GAME_LIGHT_OUTPUT_SIMONSAYS)
  {
    // Turn all the lights off
    allLightsOnOff(LOW);
  }
  
}

///////////////////////////////////
// void loop(void)
// Called in a continious loop from the arduino loop
// This function should exit and not be held up for long periods of time
//
void sequencedetect::recordInputButtonSequence(void)
{
  // Will record upto GAME_MAX_SEQUENCE from the inputs using selected input filter

  int inputSequencePosition = 0;
  int exitLoopEarly = 0;
  
  Serial.println(F("Programming mode"));

  // Clear the lights
  allLightsOnOff(LOW);
  
  // Stop scanning lights
  _runLightSequence = 0;

  while(inputSequencePosition < GAME_MAX_SEQUENCE)
  {
    // Look for exit
    if (analogRead(HALL) > HALL_SOUTH_THRESH) // Detect a south pole of a magnet
    {
      exitLoopEarly = 1;
      
      break;
    }

    int inputButton = scanInputsSteady();

    if (inputButton == NO_INPUT_DETECTED)
    {
      Serial.println(F("No input detected..."));
      
    }else{

      // Use first light to acknolwege input
      digitalWrite(_outputLightsPinList[0], HIGH);

      // Save the input
      EEPROM.write(EEPROM_START + inputSequencePosition, inputButton);

      Serial.print(F("Input "));
      Serial.print(inputButton);
      Serial.println(F(" recorded."));

      inputSequencePosition++;  //Record next button

      // Wait for no input before continuing
      while(scanInputsSteady() != NO_INPUT_DETECTED){};

      // Use first light to acknolwege input, turn off
      digitalWrite(_outputLightsPinList[0], LOW);
    }
  }

  
  // Programming mode ended
  EEPROM.write(EEPROM_START + inputSequencePosition, 0xFF);    // End of the sequence

  if (exitLoopEarly)
  {
    Serial.println(F("Programming mode complete"));
  }else{
    Serial.println(F("Programming mode complete, max entries"));
  }

  
  int i = 0;
  uint8_t input;
  
  Serial.println(F("Sequence "));

  while((input = EEPROM.read(EEPROM_START + i)) != 0xFF)
  {
    Serial.println(input, DEC);
    i++;
  }
    
  Serial.println(F("Programmed"));

  // Run scanning lights
  _runLightSequence = 1;

 
}

int sequencedetect::scanInputsSteady(void)
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

void sequencedetect::allLightsOnOff(int state)
{
  // Turn all the lights off
  for (int i = 0; i < SEQUENCE_GAME_LIGHT_OUTPUT; i++)
  {
    // Turn all lights off
    digitalWrite(_outputLightsPinList[i], state);
  }

}
