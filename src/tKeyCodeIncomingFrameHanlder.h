/*
 * IncomingFrameHandler.h
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */

#pragma once

#include "../global.h"
#include "Common_code/tMessageReciever.h"

class tKeyCodeIncomingFrameHanlder : public tMessageReciever {
public:
   tKeyCodeIncomingFrameHanlder() : tMessageReciever() { RegisterMessageType(MessageType_SerialFrameRecieved); }

   virtual void onMessage(uint8_t type, uint16_t data, void *pData);

private:
   // handlers of all incoming frames

   void HandleMsgEepromCrcRequest(uint8_t SenderID);
   void HandleMsgEepromCrcResponse(uint8_t SenderID, tMessageTypeEepromCRCResponse* Message);

};
