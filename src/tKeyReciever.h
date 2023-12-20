/*
 * tKeyReciever.h
 *
 *  Created on: 20 gru 2023
 *      Author: szkud
 */

#pragma once

#include "../global.h"
#include "Common_code/tMessageReciever.h"

class tKeyReciever : public tMessageReciever
{
public:
	tKeyReciever() : tMessageReciever()
   	   { RegisterMessageType(MessageType_SensorEvent); }

   virtual void onMessage(uint8_t type, uint16_t data, void *pData);
};
