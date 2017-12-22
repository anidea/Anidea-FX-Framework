#ifndef game_rfid__h
#define game_rfid__h

#include "arduino.h"
#include "network.h"
#include "game.h"
#include <EEPROM.h>

class Network;

class rfid : public Game
{
  public:
    rfid();

    virtual void loop();

    void solved();

    virtual void reset(); // Call to reset game

  private:
    // Presets
    #define RFID_GAME_DEFAULT 1
    #define RFID_GAME_SMALL4 2
    #define RFID_GAME_LARGE24 3

    // Configuration
    #define RFID_iGame RFID_GAME_DEFAULT

    #if RFID_iGame == RFID_GAME_DEFAULT
      static const bool learnWithMissingTag = true;
      static const int iTotalScanLength = 4;
      static const int iTagOffset = 1;
      static const bool require_enable = false;
    #elif RFID_iGame == RFID_GAME_SMALL4
      static const bool learnWithMissingTag = false;
      static const int iTotalScanLength = 4;
      static const int iTagOffset = 0;
      static const bool require_enable = false;
    #elif RFID_iGame == RFID_GAME_LARGE24
      static const bool learnWithMissingTag = true;
      static const int iTotalScanLength = 24;
      static const int iTagOffset = 2;
      static const bool require_enable = false;
    #endif
    
    static const int LEARN_BUTTON = INPUT0;
    static const int RESET_BUTTON = INPUT1;
    static const int EMERGENCY_BUTTON = INPUT2;
    
    static const int ACTIVITY_LIGHT_1 = OUTPUT0;
    static const int ACTIVITY_LIGHT_2 = OUTPUT1;
    static const int ERROR_LIGHT = OUTPUT2;
    static const int LEARN_LIGHT = OUTPUT3;
    static const int SOUND_CUE = OUTPUT4;
    static const int RS485_ENABLE = OUTPUT5;
    
    static const int MAG_LOCK = RELAY0;
    
    static const int RFID_STR_LEN_MAX = 20;

    bool iPuzzleSolved = false;
    bool learnPress = false;
    bool resetPress = false;
    int iEmergencyExit;
    uint32_t uReceiveLen;
    int iLoop = 0;
    bool iCommFailure = 0;
    char cBufOut[255], cBufIn[32];
    int iAllTagsMatched = 0;
    int iTagMatchCount = 0;
    
    int tagFails[iTotalScanLength];
    bool tagFound[iTotalScanLength];

    void RS485_SendMessage(char *pMessage, char *pResponse, uint32_t *puLength);
    int RfidSetGetTagIds(int iGet);
    void buttons();
    void EEPROMReadString(int, int, char*);
    void EEPROMWriteString(int, char*);
};

#endif
