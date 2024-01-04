/*
 * tKeyReciever.h
 *
 *  Created on: 20 gru 2023
 *      Author: szkud
 */

#pragma once

#include "../global.h"
#include "Common_code/tMessageReciever.h"
#include "lib/ArduinoProcessScheduler/src/ProcessScheduler.h"

class tKeyReciever : public tMessageReciever, public Process
{
public:
	tKeyReciever(Scheduler &manager) : Process(manager,LOW_PRIORITY,CONFIG_KEYCODE_TIMEOUT),
	    tMessageReciever()
   	   {
	    	RegisterMessageType(MessageType_SensorEvent);
            RegisterMessageType(MessageType_SerialFrameRecieved);
	    	deletePendingKeyCode();
   	   }

   virtual void onMessage(uint8_t type, uint16_t data, void *pData);
   virtual void service();


private:
   uint8_t  mDigitsCollected;        // number of collected digits. Goes back to 0 if a dongle is read or when timeout
   uint32_t mDigitsCode;             // a code, in 32bit format

   void handleSensorEvent(uint16_t data, void *pData);
   void handleFrameRecieved(uint16_t data, void *pData);

   void HandleMsgEepromCrcRequest(uint8_t SenderID);
   void HandleMsgEepromCrcResponse(uint8_t SenderID, tMessageTypeEepromCRCResponse* Message);
   void HandleMsgEepromClearCodes();

   static const uint8_t key_type_digit = 4;
   static const uint8_t key_type_dongle = 34;
   void setTimeout();
   void deletePendingKeyCode();
   void deleteTimeout() { disable(); }

   void handleCode(uint32_t code, uint8_t type);
   void handleDigit(uint32_t code);

   void sendIncorrectCodeEvent();
   void sendMatchCodeEvent(tMessageTypeAddCode *pValidCode);
};

