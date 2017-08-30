#include "network.h"
#include "cluecontrol.h"

void cluecontrol::sendGameSolved(void)
{
  CCMod.SetTrigger(CCCoil, Register);
}

void cluecontrol::sendGameReset(void)
{
//  CCMod.SetTrigger(CCCoil, Register);
}

void cluecontrol::loop(void)
{
 CCMod.ReceiveModBus(); //thist must be callled to service the ModBus routine.
 listenForRequest();

 if (pMyGame->_puzzleSolved == 1 && sent == 0)
 {
  sendGameSolved();
  sent = 1;
 }
 else if (pMyGame->_puzzleSolved == 0 && sent == 1)
 {
  sendGameReset();
  sent = 0;
 }

 
}

void cluecontrol::listenForRequest()
{
  if (CCMod.CCValue == ACTIVATE)
   {
        //active=true;   //set the active flag
        Serial.println(F("Activate command received"));  //for troubleshooting-monitoring
        pMyGame->forceSolved();
        CCMod.CCValue = 0;  //clear the value to prevent re-triggering
        
   }

   if (CCMod.CCValue == DEACTIVATE)
   {
        //active=false;           //clear the active flag
        Serial.println(F("deactivate command received"));
        pMyGame->reset();
        CCMod.CCValue = 0;  //clear the value to prevent re-triggering
   }
}

