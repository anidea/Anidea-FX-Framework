#ifndef game_rfid__h
#define game_rfid__h

#include "arduino.h"
#include "network.h"
#include "game.h"

class Network;

class rfid : public Game
{
  public:
    rfid();

    virtual void loop();

    void solved();

    byte learn();

    virtual void reset(); // Call to reset game

    byte getLen();
    void getTagStates(byte tagStates[], bool&);

    // Presets
    #define RFID_GAME_DEFAULT 1
    #define RFID_GAME_SMALL4 2
    #define RFID_GAME_LARGE24 3

    // Configuration
    #define RFID_iGame RFID_GAME_DEFAULT

    #if RFID_iGame == RFID_GAME_DEFAULT
      static const bool learnWithMissingTag = true;
      static const byte iTotalScanLength = 4;
      static const byte iTagOffset = 1;
      static const bool require_enable = false;
    #elif RFID_iGame == RFID_GAME_SMALL4
      static const bool learnWithMissingTag = false;
      static const byte iTotalScanLength = 4;
      static const byte iTagOffset = 0;
      static const bool require_enable = false;
    #elif RFID_iGame == RFID_GAME_LARGE24
      static const bool learnWithMissingTag = true;
      static const byte iTotalScanLength = 24;
      static const byte iTagOffset = 2;
      static const bool require_enable = false;
    #endif

    static const byte RFID_STR_LEN_MAX = 20;

    void playVideo(byte);
    void playAudio(byte);
    void setLedState(byte, bool);
    void setLock(bool);
    void setLight(bool);
    void setScrollingLight(bool);
    void setDisplay(bool, const char[]);

    char tagValues[iTotalScanLength][RFID_STR_LEN_MAX + 1];
    bool tagValuesFlag = true;
    static const byte numButtons = 6;
    bool buttonStates[numButtons];
    bool buttonFlag = false;

  private:
    static const byte LEARN_BUTTON = INPUT0;
    static const byte RESET_BUTTON = INPUT1;
    static const byte EMERGENCY_BUTTON = INPUT2;
    static const byte REPLAY_BUTTON = INPUT3;
    
    static const byte ACTIVITY_LIGHT_1 = OUTPUT0;
    static const byte ACTIVITY_LIGHT_2 = OUTPUT1;
    static const byte ERROR_LIGHT = OUTPUT2;
    static const byte LEARN_LIGHT = OUTPUT3;
    static const byte SOUND_CUE = OUTPUT4;
    
    static const byte MAG_LOCK = RELAY0;

    bool iPuzzleSolved = false;
    bool learnPress = false;
    bool resetPress = false;
    bool iEmergencyExit;
    uint32_t uReceiveLen;
    int iLoop = 0;
    bool iCommFailure = 0;
    bool iAllTagsMatched = 0;
    byte iTagMatchCount = 0;
    
    byte tagFails[iTotalScanLength];
    bool tagFound[iTotalScanLength];
    
    byte tagStatesOld[iTotalScanLength];

    void RS485_SendMessage(char *pMessage, char *pResponse, uint32_t *puLength);
    byte RfidSetGetTagIds(byte iGet);
    void buttons();
    void EEPROMReadString(byte, byte, char*);
    void EEPROMWriteString(byte, char*);

    Uart *mySerialRfid;
};

#endif
