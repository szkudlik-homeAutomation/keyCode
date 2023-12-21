/*
 * tKeyReciever.cpp
 *
 *  Created on: 20 gru 2023
 *      Author: szkud
 */


#include "tKeyReciever.h"
#include "Common_code/sensors/tWiegandSensor.h"

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
	   hanldeDongleCode(pResult->code);	// independent from keys - may be in the middle of key sequence
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
}

void tKeyReciever::sendIncorrectCodeEvent()
{
	DEBUG_PRINTLN_3("INCORRECT CODE - event sent");
}

void tKeyReciever::sendMatchCodeEvent()
{
	DEBUG_PRINTLN_3("CODE ACCEPTED - event sent");
}


void tKeyReciever::hanldeDongleCode(uint32_t code)
{
	DEBUG_PRINTLN_3("DONGLE CODE");
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
		handleCompletedDigitCode();
		return;
	}
	if (mDigitsCollected >= KEY_MAX_DIGITS)
	{
		sendIncorrectCodeEvent();
		deletePendingKeyCode();
		return;
	}

	mDigits[mDigitsCollected++] = code;
}

void tKeyReciever::handleCompletedDigitCode()
{
	DEBUG_PRINTLN_3("DIGIT CODE");
	deletePendingKeyCode();
}
