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
}
