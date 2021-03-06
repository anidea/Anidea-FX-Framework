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
  if (_puzzleSolved_old != pMyGame->_puzzleSolved)
  {
    CCMod.SetTrigger(10 * MyIP[3] + 0, pMyGame->_puzzleSolved);
    _puzzleSolved_old = pMyGame->_puzzleSolved;
  }

  if (_enabled_old != pMyGame->_enabled)
  {
    CCMod.SetTrigger(10 * MyIP[3] + 1, pMyGame->_enabled);
    _enabled_old = pMyGame->_enabled;
  }

  for (int i = 0; i < NUM_INPUTS; i++)
  {
    if (pMyGame->INPUT_STATES[i] != INPUT_STATE_OLD[i] && pMyGame->INPUT_OVERRIDE_ENABLE[i] == 1) 
    {
//      Serial.print(F("INPUT"));
//      Serial.print(i);
//      Serial.println(F(" status changed"));
//      Serial.println(100 * MyIP[3] + i);
      CCMod.SetTrigger(100 * MyIP[3] + i, pMyGame->INPUT_STATES[i]);
      INPUT_STATE_OLD[i] = pMyGame->INPUT_STATES[i];
    }
  }

  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (pMyGame->OUTPUT_STATES[i] != OUTPUT_STATE_OLD[i] && pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == 1) 
    {
//      Serial.print(F("OUTPUT"));
//      Serial.print(i);
//      Serial.println(F(" status changed"));
//      Serial.println(100 * MyIP[3] + 10 + i);
      CCMod.SetTrigger(100 * MyIP[3] + 10 + i, pMyGame->OUTPUT_STATES[i]);
      OUTPUT_STATE_OLD[i] = pMyGame->OUTPUT_STATES[i];
    }
  }

  for (int i = 0; i < NUM_RELAYS; i++)
  {
    if (pMyGame->RELAY_STATES[i] != RELAY_STATE_OLD[i] && pMyGame->RELAY_OVERRIDE_ENABLE[i] == 1) 
    {
//      Serial.print(F("RELAY"));
//      Serial.print(i);
//      Serial.println(F(" status changed"));
//      Serial.println(100 * MyIP[3] + 20 + i);
      CCMod.SetTrigger(100 * MyIP[3] + 20 + i, pMyGame->RELAY_STATES[i]);
      RELAY_STATE_OLD[i] = pMyGame->RELAY_STATES[i];
    }
  }
}

void cluecontrol::listenForRequest()
{
  if (CCMod.CCValue == ACTIVATE)
  {
//    Serial.println(F("Activate command received"));  //for troubleshooting-monitoring
    pMyGame->forceSolved();
  }
  else if (CCMod.CCValue == DEACTIVATE)
  {
//    Serial.println(F("deactivate command received"));
    pMyGame->reset();
  }
  else if (CCMod.CCValue == ENABLE)
  {
//    Serial.println(F("enable command received"));
    pMyGame->enable();
  }
  else if (CCMod.CCValue == CC_DISABLE)
  {
//    Serial.println(F("disable command received"));
    pMyGame->disable();
  }
  else if (CCMod.CCValue == RESET_ENABLE)
  {
//    Serial.println(F("reset_enable command received"));
    pMyGame->reset();
    pMyGame->enable();
  }
  else if (CCMod.CCValue == RESET_DISABLE)
  {
//    Serial.println(F("reset_disable command received"));
    pMyGame->reset();
    pMyGame->disable();
  }
  else
  {
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
      if (CCMod.CCValue == 100 + (10 * i) && pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == 1) 
      {
//        Serial.print(F("OUTPUT"));
//        Serial.print(i);
//        Serial.println(F(" turned off"));
        pMyGame->OUTPUT_STATES[i] = 0;
        pMyGame->OUTPUT_STATES_FLAG[i] = true;
      }
      else if (CCMod.CCValue == 100 + (10 * i) + 1 && pMyGame->OUTPUT_OVERRIDE_ENABLE[i] == 1) 
      {
//        Serial.print(F("OUTPUT"));
//        Serial.print(i);
//        Serial.println(F(" turned on"));
        pMyGame->OUTPUT_STATES[i] = 1;
        pMyGame->OUTPUT_STATES_FLAG[i] = true;
      }
    }
    for (int i = 0; i < NUM_RELAYS; i++)
    {
      if (CCMod.CCValue == 200 + (10 * i) && pMyGame->RELAY_OVERRIDE_ENABLE[i] == 1) 
      {
//        Serial.print(F("RELAY"));
//        Serial.print(i);
//        Serial.println(F(" turned off"));
        pMyGame->RELAY_STATES[i] = 0;
        pMyGame->RELAY_STATES_FLAG[i] = true;
      }
      else if (CCMod.CCValue == 200 + (10 * i) + 1 && pMyGame->RELAY_OVERRIDE_ENABLE[i] == 1) 
      {
//        Serial.print(F("RELAY"));
//        Serial.print(i);
//        Serial.println(F(" turned on"));
        pMyGame->RELAY_STATES[i] = 1;
        pMyGame->RELAY_STATES_FLAG[i] = true;
      }
    }
  }
  CCMod.CCValue = 0;  //clear the value to prevent re-triggering
}


