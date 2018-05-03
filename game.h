#ifndef game__h
#define game__h

#include "arduino.h"
#include "network.h"

// Boards
#if defined(FX300) || defined(FX350)
#include "fx300.h"
#endif

#if defined(FX450)
#include "fx450.h"
#endif

#define TIMER_INTERVAL 100

#define HALL_NORTH_THRESH 410
#define HALL_SOUTH_THRESH 614

class Network;

class Game
{
  public:
    // Game states
    static const int GAMESTATE_START = 0;
    static const int GAMESTATE_RUN = 10;
    static const int GAMESTATE_SOLVED = 100;
    static const int GAMESTATE_ENDLOOP = 255;
    static const int EEPROM_START = 10; // Start all EEPROM storage from here because anything before is used by the framework
  
	int freeRunningTimer = 0;

    Game();

    virtual void tick();  // Called periodically to run hard timing

    virtual void loop();  // Called in loop

    virtual void reset(); // Call to reset game

    virtual void solved(); // Called when the game is solved

    virtual void forceSolved(); // Called when the game needs to be force solved

    virtual void enable(); // Call to enable game

    virtual void disable(); // Call to disable game

    void setNetwork(Network *pn) { this->_pMyNetwork = pn; };

    bool   isSolved(void)  {return _puzzleSolved; }

    bool   isEnabled(void)  {return _enabled; }

	virtual void getTagData(char * data, size_t size) {}
	virtual bool isRFIDChanged() { return false; }
    virtual byte learn() {}

    Network *_pMyNetwork = NULL;

    // Global Game Variables
    bool _puzzleSolved = 0;
    byte _gameState = 0;
    bool _enabled = 0;
    
    // Input override enables
    bool INPUT_OVERRIDE_ENABLE[NUM_INPUTS] = {};

    // Output override enables
    bool OUTPUT_OVERRIDE_ENABLE[NUM_OUTPUTS] = {};

    // Relay override enables
    bool RELAY_OVERRIDE_ENABLE[NUM_RELAYS] = {};

	static bool INPUT_STATES[NUM_INPUTS];

	static bool OUTPUT_STATES[NUM_OUTPUTS];
	static bool OUTPUT_STATES_FLAG[NUM_OUTPUTS];

	static bool RELAY_STATES[NUM_RELAYS];
	static bool RELAY_STATES_FLAG[NUM_OUTPUTS];

	bool INPUT_STATE_OLD[NUM_INPUTS]{};
	bool OUTPUT_STATE_OLD[NUM_INPUTS]{};
	bool RELAY_STATE_OLD[NUM_INPUTS]{};

#ifdef FX60_0_ENABLE
	uint8_t FX60_0_INPUT_STATES = 0;
	uint8_t FX60_0_OUTPUT_STATES = 0;
	uint8_t FX60_0_OUTPUT_STATES_FLAG = 0;
	uint8_t FX60_0_RELAY_STATES = 0;
	uint8_t FX60_0_RELAY_STATES_FLAG = 0;

	static const uint8_t FX60_0_INPUT_ENABLE = FX60_0_INPUT_OVERRIDES;
	static const uint8_t FX60_0_OUTPUT_ENABLE = FX60_0_OUTPUT_OVERRIDES;
	static const uint8_t FX60_0_RELAY_ENABLE = FX60_0_RELAY_OVERRIDES;

	TCA6424A FX60_0_I2C{ TCA6424A_ADDRESS_ADDR_LOW };

	uint8_t FX60_0_ISTATE_OLD = 0;
	uint8_t FX60_0_OSTATE_OLD = 0;
	uint8_t FX60_0_RSTATE_OLD = 0;
#endif

#ifdef FX60_1_ENABLE
	uint8_t FX60_1_INPUT_STATES = 0;
	uint8_t FX60_1_OUTPUT_STATES = 0;
	uint8_t FX60_1_OUTPUT_STATES_FLAG = 0;
	uint8_t FX60_1_RELAY_STATES = 0;
	uint8_t FX60_1_RELAY_STATES_FLAG = 0;

	static const uint8_t FX60_1_INPUT_ENABLE = FX60_1_INPUT_OVERRIDES;
	static const uint8_t FX60_1_OUTPUT_ENABLE = FX60_1_OUTPUT_OVERRIDES;
	static const uint8_t FX60_1_RELAY_ENABLE = FX60_1_RELAY_OVERRIDES;

	TCA6424A FX60_1_I2C{ TCA6424A_ADDRESS_ADDR_HIGH };

	uint8_t FX60_1_ISTATE_OLD = 0;
	uint8_t FX60_1_OSTATE_OLD = 0;
	uint8_t FX60_1_RSTATE_OLD = 0;
#endif

	void EEPROMReadString(byte pos, byte len, char* data);
	void EEPROMWriteString(byte pos, char* data);

  private:

};

#endif
