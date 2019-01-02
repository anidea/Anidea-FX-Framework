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
	// Setup hardware
	pinMode(RELAY0, OUTPUT);
  pinMode(RELAY1, OUTPUT);

  pinMode(OUTPUT0, OUTPUT);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);
  pinMode(OUTPUT3, OUTPUT);
  pinMode(OUTPUT4, OUTPUT);
  pinMode(OUTPUT5, OUTPUT);

  pinMode(INPUT0, INPUT);
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(INPUT3, INPUT);
  pinMode(INPUT4, INPUT);
  pinMode(INPUT5, INPUT);

  pinMode(RS485_ENABLE, OUTPUT);
  pinMode(STAT_LED, OUTPUT);

  pinMode(WIZ_CS, OUTPUT);
  digitalWrite(WIZ_CS, HIGH);

#ifdef DIGITAL_HALL
	int z;
	if (readDigitalHall(z) == false)
	{
		// One time (should madebe at least) configure of new hall sensor from default address.
		Serial.println("About to configure digital hall.  Cycle power completely when complete.");
		programDigitalHall();
	}
#endif

  // Turn all overrides on
  for(int i = 0; i < NUM_INPUTS; ++i) //Or loop the array and init them.
      INPUT_OVERRIDE_ENABLE[i] = true;

  for(int i = 0; i < NUM_OUTPUTS; ++i) //Or loop the array and init them.
      OUTPUT_OVERRIDE_ENABLE[i] = true;

  for(int i = 0; i < NUM_RELAYS; ++i) //Or loop the array and init them.
      RELAY_OVERRIDE_ENABLE[i] = true;


	#if defined(FX60_0_ENABLE)
	  FX60_0_I2C.writeAll(0, 0, 0); // Clear all
	  FX60_0_I2C.setAllDirection(0xFF, 0, 0); // Input, output, output

	  Serial.print("FX60_0 Connection: ");
	  Serial.println(FX60_0_I2C.testConnection());
	#endif

	#if defined(FX60_1_ENABLE)
	  FX60_1_I2C.writeAll(0, 0, 0); // Clear all
	  FX60_1_I2C.setAllDirection(0xFF, 0, 0); // Input, output, output

	  Serial.print("FX60_1 Connection: ");
	  Serial.println(FX60_1_I2C.testConnection());
	#endif

  Serial.println(F("Game object initialized"));
}

///////////////////////////////////
// void tick(void)
// Call periodically, nominally every 100ms with a real hardware counter
//
void Game::tick(void)
{
	freeRunningTimer += TIMER_INTERVAL;
}

///////////////////////////////////
// void loop(void)
// Called in a continious loop from the arduino loop
// This function should exit and not be held up for long periods of time
//
void Game::loop(void)
{
  // put your main code here, to run repeatedly:

  //delay(10); // Keep game running at a reasonable rate, not micro speed. 100x a second, ehternet adds significant delays

			 // Read input states
  for (int i = 0; i < NUM_INPUTS; i++)
  {
	  if (INPUT_OVERRIDE_ENABLE[i])
	  {
		  bool high = digitalRead(INPUTS[i]);
		  if (INPUT_STATES[i] != high)
		  {
			  INPUT_STATE_OLD[i] = INPUT_STATES[i];
		  }

		  INPUT_STATES[i] = high;
	  }
  }

  // Write output states
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
	  if (OUTPUT_OVERRIDE_ENABLE[i])
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
	  uint8_t bitIndex = 1 << i;

	  if (RELAY_OVERRIDE_ENABLE[i])
	  {
		  if (RELAY_STATES_FLAG[i] == true)
		  {
			  digitalWrite(RELAYS[i], RELAY_STATES[i]);

			  RELAY_STATES_FLAG[i] = false;
		  }
	  }
  }
#if defined(FX60_0_ENABLE)
  for (int i = 0; i < FX60_NUM_INPUTS; i++)
  {
	  bool read = FX60_0_I2C.readPin(FX60_INPUTS[i]);
	  uint8_t bitIndex = 1 << i;
	  FX60_0_INPUT_STATES &= ~bitIndex;
	  FX60_0_INPUT_STATES |= bitIndex * read;
  }

  // Write output states
  for (int i = 0; i < FX60_NUM_OUTPUTS; i++)
  {
	  uint8_t bitIndex = 1 << i;

	  if (FX60_0_OUTPUT_ENABLE & bitIndex) //If override enabled
	  {
		  if (FX60_0_OUTPUT_STATES_FLAG & bitIndex) //If state changed
		  {
			  FX60_0_I2C.writePin(FX60_OUTPUTS[i], (FX60_0_OUTPUT_STATES & bitIndex) ? HIGH : LOW); //write change
			  FX60_0_OUTPUT_STATES_FLAG &= ~bitIndex; //zero out flag
		  }
		  /*else
		  {
		  bool read = FX60_0_I2C.readPin(FX60_OUTPUTS[i]);
		  FX60_0_OUTPUT_STATES &= ~bitIndex;
		  FX60_0_OUTPUT_STATES |= bitIndex * read;
		  }*/
	  }
  }

  // Write relay states
  for (int i = 0; i < FX60_NUM_RELAYS; i++)
  {
	  uint8_t bitIndex = 1 << i;

	  if (FX60_0_RELAY_ENABLE & bitIndex)
	  {
		  if (FX60_0_RELAY_STATES_FLAG & bitIndex) //If state changed
		  {
			  FX60_0_I2C.writePin(FX60_RELAYS[i], (FX60_0_RELAY_STATES & bitIndex) ? HIGH : LOW); //write change
			  FX60_0_RELAY_STATES_FLAG &= ~bitIndex; //zero out flag
		  }
		  /*else
		  {
		  bool read = FX60_0_I2C.readPin(FX60_RELAYS[i]);
		  FX60_0_RELAY_STATES &= ~bitIndex;
		  FX60_0_RELAY_STATES |= bitIndex * read;
		  }*/
	  }
  }
#endif
#if defined(FX60_1_ENABLE)
  for (int i = 0; i < FX60_NUM_INPUTS; i++)
  {
	  bool read = FX60_1_I2C.readPin(FX60_INPUTS[i]);
	  uint8_t bitIndex = 1 << i;
	  FX60_1_INPUT_STATES &= ~bitIndex;
	  FX60_1_INPUT_STATES |= bitIndex * read;
  }

  // Write output states
  for (int i = 0; i < FX60_NUM_OUTPUTS; i++)
  {
	  uint8_t bitIndex = 1 << i;

	  if (FX60_1_OUTPUT_ENABLE & bitIndex) //If override enabled
	  {
		  if (FX60_1_OUTPUT_STATES_FLAG & bitIndex) //If state changed
		  {
			  FX60_1_I2C.writePin(FX60_OUTPUTS[i], (FX60_1_OUTPUT_STATES & bitIndex) ? HIGH : LOW); //write change
			  FX60_1_OUTPUT_STATES_FLAG &= ~bitIndex; //zero out flag
		  }
		  /*else
		  {
		  bool read = FX60_1_I2C.readPin(FX60_OUTPUTS[i]);
		  FX60_1_OUTPUT_STATES &= ~bitIndex;
		  FX60_1_OUTPUT_STATES |= bitIndex * read;
		  }*/
	  }
  }

  // Write relay states
  for (int i = 0; i < FX60_NUM_RELAYS; i++)
  {
	  uint8_t bitIndex = 1 << i;

	  if (FX60_1_RELAY_ENABLE & bitIndex)
	  {
		  if (FX60_1_RELAY_STATES_FLAG & bitIndex) //If state changed
		  {
			  FX60_1_I2C.writePin(FX60_RELAYS[i], (FX60_1_RELAY_STATES & bitIndex) ? HIGH : LOW); //write change
			  FX60_1_RELAY_STATES_FLAG &= ~bitIndex; //zero out flag
		  }
		  /*else
		  {
		  bool read = FX60_1_I2C.readPin(FX60_RELAYS[i]);
		  FX60_1_RELAY_STATES &= ~bitIndex;
		  FX60_1_RELAY_STATES |= bitIndex * read;
		  }*/
	  }
  }
#endif
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
  solvedFlag = false;
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
		OUTPUT_STATES_FLAG[i] = true;
		digitalWrite(OUTPUTS[i], LOW);
	}

	for (int i = 0; i < NUM_RELAYS; ++i) //Or loop the array and init them.
	{
		RELAY_STATES[i] = 1;
		RELAY_STATES_FLAG[i] = true;
		digitalWrite(RELAYS[i], HIGH);
	}
	  
  Serial.println(F("Game Reset"));
  
  // Reset game vars
  _puzzleSolved = 0;
  _gameState = GAMESTATE_START;
  
  solvedFlag = false;
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

void Game::EEPROMReadString(size_t pos, byte len, char* data)
{
	byte i;
	for (i = 0; i < len; i++)
	{
		data[i] = EEPROM.read(pos + i);
	}
	data[i] = 0;

	//Serial.print("Read: ");
	//Serial.println(data);
}

void Game::EEPROMWriteString(size_t pos, char* data)
{
	byte i;
	for (i = 0; i < strlen(data); i++)
	{
		EEPROM.write(pos + i, data[i]);
		//Serial.println(data[i]);
	}

	EEPROM.write(pos + i, 0);

	//Serial.print("Wrote: ");
	//Serial.println(data);
}

bool Game::hallLearnCommand(bool exit)
{
#ifdef DIGITAL_HALL
	static const int HALL_NORTH_THRESH = -20;
	static const int HALL_SOUTH_THRESH = 20;
#else
	static const int HALL_NORTH_THRESH = (.4 * 0x3FF);
	static const int HALL_SOUTH_THRESH = (.6 * 0x3FF);
#endif

	static bool releaseNorth;
	static bool learning = false;
	static bool released = true;

	if (exit)
	{
		learning = false;
		released = true;
		return true;
	}
#ifdef DIGITAL_HALL
	int read;
	readDigitalHall(read);
#else
	int read = analogRead(HALL);
#endif

	bool inNorth = read < HALL_NORTH_THRESH;
	bool inSouth = read > HALL_SOUTH_THRESH;

	if (!inNorth && !inSouth) released = true;

	if (!released) return false;

	if (learning)
	{
		if (inNorth && releaseNorth)
		{
			released = false;
			learning = false;
			return true;
		}
		else if (inSouth && !releaseNorth)
		{
			released = false;
			learning = false;
			return true;
		}
	}
	else
	{
		if (inNorth)
		{
			releaseNorth = false;
			learning = true;
			released = false;

			return true;
		}
		else if (inSouth)
		{
			releaseNorth = true;
			learning = true;
			released = false;

			return true;
		}
	}

	return false;
}


bool Game::readDigitalHall(int& zHallValue)
{
  auto signExtendBitfield = [](uint32_t data, int width) -> long
  {
    long x = (long) data;
    long mask = 1L << (width - 1);
  
    if (width < 32)
    {
        x = x & ((1 << width) - 1); // make sure the upper bits are zero
    }
  
    return (long)((x ^ mask) - mask);
  };
  
	uint8_t address = 113;

	Wire.beginTransmission((uint8_t)address);
	Wire.write(0x28);
	bool error = Wire.endTransmission();
	
	if (error)
	{
		Serial.println("Failed to find I2C Digital Hall Device");
		
		return false;
	}

	Wire.requestFrom((uint8_t)address, (uint8_t)8);
      
  uint32_t value0x28 = Wire.read() << 24;
  value0x28 += Wire.read() << 16;
  value0x28 += Wire.read() << 8;
  value0x28 += Wire.read();

  uint32_t value0x29 = Wire.read() << 24;
  value0x29 += Wire.read() << 16;
  value0x29 += Wire.read() << 8;
  value0x29 += Wire.read();
  
	zHallValue = signExtendBitfield(((value0x28 >> 4) & 0x0FF0) | ((value0x29 >> 8) & 0x0F), 12);

  error = Wire.endTransmission();

	if (error) return false;

	//Serial.print("Hall Read ");
	//Serial.println(zHallValue);

	return true;
}

bool Game::programDigitalHall()
{
	uint16_t error;

	auto write = [&](uint32_t value, uint8_t reg = 0x02) -> uint16_t
	{
		Wire.beginTransmission(0x0);
		Wire.write(reg);
		Wire.write((byte)(value >> 24));
		Wire.write((byte)(value >> 16));
		Wire.write((byte)(value >> 8));
		Wire.write((byte)(value));
		return Wire.endTransmission();
	};

	auto read = [&](uint32_t& value) -> uint16_t
	{
		Wire.beginTransmission(0x0);
		Wire.write(0x02);
		error = Wire.endTransmission(false);

		if (!error)
		{
			Wire.requestFrom(0x0, 4);
			value = (uint32_t)(Wire.read()) << 24;
			value += (uint32_t)(Wire.read()) << 16;
			value += (uint32_t)(Wire.read()) << 8;
			value += (uint32_t)(Wire.read());
		}

		return error;
	};

	Serial.println("Configuring digital hall");

	error = write(0x2C413534, 0x35);

	if (error)
	{
		Serial.print("Error while trying to enter customer access mode. error = ");
		Serial.println(error);

		return false;
	}

	uint32_t value0x02 = -1;
	error = read(value0x02);

	if (error)
	{
		Serial.print("Unable to read the ALS31300. error = ");
		Serial.println(error);
		return false;
	}

	value0x02 &= 0xFFFE03FF; //zero out address bits
	value0x02 |= (113 << 10); //write address bits

	error = write(value0x02);

	if (error)
	{
		Serial.print("Unable to write the ALS31300. error = ");
		Serial.println(error);

		return false;
	}

	delay(100);

	Serial.println(F("Hall programmed for address 113"));
	return true;
}
