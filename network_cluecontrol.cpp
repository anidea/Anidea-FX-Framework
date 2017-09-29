#include "network.h"
#include "network_cluecontrol.h"

void cluecontrol::loop(void)
{
  Network::loop();
  CCMod.CCValue = 0;  //clear the value to prevent re-triggering
  CCMod.ReceiveModBus(); //thist must be callled to service the ModBus routine.
  listenForRequest();
  setTriggers();
}

void cluecontrol::setTriggers()
{
  if (sent != pMyGame->_puzzleSolved)
  {
    CCMod.SetTrigger(MyIP[3], pMyGame->_puzzleSolved);
    sent = pMyGame->_puzzleSolved;
  }

  for (int i = 0; i < NUM_INPUTS; i++)
  {
    if (INPUT_STATES[i] != INPUT_STATE_OLD[i] && pMyGame->INPUT_OVERRIDE_ENABLE[i] == 1) 
    {
      Serial.print(F("INPUT"));
      Serial.print(i);
      Serial.println(F(" status changed"));
      Serial.println(10 * MyIP[3] + i);
      CCMod.SetTrigger(10 * MyIP[3] + i, INPUT_STATES[i]);
      INPUT_STATE_OLD[i] = INPUT_STATES[i];
    }
  }
}

void cluecontrol::listenForRequest()
{
  if (CCMod.CCValue == ACTIVATE)
  {
    Serial.println(F("Activate command received"));  //for troubleshooting-monitoring
    pMyGame->forceSolved();
    CCMod.CCValue = 0;  //clear the value to prevent re-triggering
  }
  else if (CCMod.CCValue == DEACTIVATE)
  {
    Serial.println(F("deactivate command received"));
    pMyGame->reset();
    CCMod.CCValue = 0;  //clear the value to prevent re-triggering
  }
  else
  {
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
      if (CCMod.CCValue == 100 + (10 * i) && pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("OUTPUT"));
        Serial.print(i);
        Serial.println(F(" turned off"));
        OUTPUT_STATES[i] = 0;
        CCMod.CCValue = 0;  //clear the value to prevent re-triggering
      }
      else if (CCMod.CCValue == 100 + (10 * i) + 1 && pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("OUTPUT"));
        Serial.print(i);
        Serial.println(F(" turned on"));
        OUTPUT_STATES[i] = 1;
        CCMod.CCValue = 0;  //clear the value to prevent re-triggering
      }
    }
    for (int i = 0; i < NUM_RELAYS; i++)
    {
      if (CCMod.CCValue == 200 + (10 * i) && pMyGame->RELAY_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("RELAY"));
        Serial.print(i);
        Serial.println(F(" turned off"));
        RELAY_STATES[i] = 0;
        CCMod.CCValue = 0;  //clear the value to prevent re-triggering
      }
      else if (CCMod.CCValue == 200 + (10 * i) + 1 && pMyGame->RELAY_OVERRIDE_ENABLE[i] == 1) 
      {
        Serial.print(F("RELAY"));
        Serial.print(i);
        Serial.println(F(" turned on"));
        RELAY_STATES[i] = 1;
        CCMod.CCValue = 0;  //clear the value to prevent re-triggering
      }
    }
  }
}

