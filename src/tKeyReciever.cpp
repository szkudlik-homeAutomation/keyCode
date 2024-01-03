/*
 * tKeyReciever.cpp
 *
 *  Created on: 20 gru 2023
 *      Author: szkud
 */


#include "tKeyReciever.h"
#include "Common_code/tTimestamp.h"
#include "Common_code/sensors/tWiegandSensor.h"
#include "Common_code/TLE8457_serial/TLE8457_serial_lib.h"

void tKeyReciever::onMessage(uint8_t type, uint16_t data, void *pData)
{
   uint8_t SensorID = data;
   if (type != MessageType_SensorEvent)
	   return;
   if (SensorID != WIEGAND_SENSOR_ID)
	   return;

   tSensorEvent *pSensorEvent = (tSensorEvent *)pData;
   if (pSensorEvent->EventType != EV_TYPE_MEASUREMENT_COMPLETED)
	   return;

   tWiegandSensor::tResult *pResult = (tWiegandSensor::tResult *)pSensorEvent->pDataBlob;
   DEBUG_PRINT_3("Key reciever event, key type: ");
   DEBUG_3(print(pResult->type, DEC));
   DEBUG_PRINT_3(" value: ");
   DEBUG_3(println(pResult->code, HEX));

   switch (pResult->type)
   {
   case key_type_dongle:
	   handleCode(pResult->code, key_type_dongle);	// independent from keys - may be in the middle of key sequence
  	   deletePendingKeyCode();
	   break;

   case key_type_digit:
	   setTimeout();
	   handleDigit(pResult->code);
	   break;

   default:
	   deletePendingKeyCode();	// error
   }
}

void tKeyReciever::setTimeout()
{
	// set a timeout, clear prev one (if any)
	deleteTimeout();
	setPeriod(CONFIG_KEYCODE_TIMEOUT);
	setIterations(RUNTIME_ONCE);
	enable();
}

void tKeyReciever::service()
{
	// a timeout
	sendIncorrectCodeEvent();
	deletePendingKeyCode();
}

void tKeyReciever::deletePendingKeyCode()
{
	deleteTimeout();
	mDigitsCollected = 0;
	mDigitsCode = 0;
}

void tKeyReciever::sendIncorrectCodeEvent()
{
	DEBUG_PRINTLN_3("INCORRECT CODE - event sent");

	tMessageTypeButtonPress Msg;
    Msg.DoubleClick = 0xFFFF;
    Msg.ForceSrcId = 0;
    Msg.LongClick = 0;
    Msg.ShortClick = 0;
    CommSenderProcess::Instance->Enqueue(DEVICE_ID_BROADCAST,MESSAGE_BUTTON_PRESS,sizeof(Msg),&Msg);
}

void tKeyReciever::sendMatchCodeEvent(tMessageTypeAddCode *pValidCode)
{
	DEBUG_PRINTLN_3("CODE ACCEPTED - event sent");

    tMessageTypeButtonPress Msg;
    Msg.DoubleClick = 0;
    Msg.ForceSrcId = 0;
    Msg.LongClick = 0;
    Msg.ShortClick = pValidCode->ButtonBitmap;
    CommSenderProcess::Instance->Enqueue(DEVICE_ID_BROADCAST,MESSAGE_BUTTON_PRESS,sizeof(Msg),&Msg);
}


void tKeyReciever::handleDigit(uint32_t code)
{
	if (code == 0x1B)
	{
		deletePendingKeyCode();
		return;
	}
	if (code == 0xD)
	{
	    handleCode(mDigitsCode, key_type_digit);
		deletePendingKeyCode();
		return;
	}
	if (mDigitsCollected >= KEY_MAX_DIGITS)
	{
		sendIncorrectCodeEvent();
		deletePendingKeyCode();
		return;
	}

	mDigitsCode += code * 10^mDigitsCollected;
}

void tKeyReciever::handleCode(uint32_t code, uint8_t type)
{
    DEBUG_PRINT_3("Processing code type");
    DEBUG_3(print(type, DEC));
    DEBUG_PRINT_3(" value: ");
    DEBUG_3(print(code, DEC));
    DEBUG_PRINT_3(" 0x");
    DEBUG_3(println(code, HEX));

    uint8_t NumOfEnties = EEPROM.read(KEY_CODE_TABLE_USAGE_OFFSET);
	for (uint8_t i = 0; i < NumOfEnties; i++)
	{
		tMessageTypeAddCode ValidCode;
		EEPROM.get(KEY_CODE_TABLE_OFFSET+(KEY_CODE_TABLE_SIZE*i),ValidCode);
		if (ValidCode.type != type)
			continue;
		if (ValidCode.code != code)
			continue;

		if (ValidCode.ValidStart && ValidCode.ValidEnd)
		{
		    uint16_t ts = tTimestamp::get();
		    ts = ts >> 8;   // eldest 8 bits

		    if (ValidCode.ValidStart > ts)
                continue;
            if (ValidCode.ValidEnd < ts)
                continue;
		}

		sendMatchCodeEvent(&ValidCode);
		return;
	}

	sendIncorrectCodeEvent();
}
