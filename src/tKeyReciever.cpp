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
#include "Common_code/TLE8457_serial/tOutgoingFrames.h"

void tKeyReciever::onMessage(uint8_t type, uint16_t data, void *pData)
{
    switch (type)
    {
    case MessageType_SensorEvent:
        handleSensorEvent(data, pData);
        break;

    case MessageType_SerialFrameRecieved:
        handleFrameRecieved(data, pData);
        break;
    }
}

void tKeyReciever::handleSensorEvent(uint16_t data, void *pData)
{
    uint8_t SensorID = data;
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
    case wiegrand_key_type_dongle:
        handleCode(pResult->code, key_type_dongle);  // independent from keys - may be in the middle of key sequence
        deletePendingKeyCode();
        break;

    case wiegrand_key_type_digit:
        setTimeout();
        handleDigit(pResult->code);
        break;

    default:
        deletePendingKeyCode();  // error
    }
}

void tKeyReciever::handleFrameRecieved(uint16_t data, void *pData)
{
    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;

    switch (data)   // messageType
    {
    case MESSAGE_TYPE_EEPROM_CRC_REQUEST:
          HandleMsgEepromCrcRequest(SenderDevId);
          break;
#if CONFIG_DEBUG_NODE
    case MESSAGE_TYPE_EEPROM_CRC_RESPONSE:
          HandleMsgEepromCrcResponse(SenderDevId,(tMessageTypeEepromCRCResponse*)(pFrame->Data));
          break;

    case MESSAGE_BUTTON_PRESS:
        HandleMsgButtonPress((tMessageTypeButtonPress*)(pFrame->Data));
        break;

    case MESSAGE_TYPE_CODE_RECIEVED:
        HandleMsgCodeRecieved((tMessageTypeCodeRecieved*)(pFrame->Data));
        break;

#endif // CONFIG_DEBUG_NODE

    case MESSAGE_TYPE_CLEAR_CODES:
          DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_CLEAR_CODES");
          HandleMsgEepromClearCodes();
          break;

    case MESSAGE_TYPE_ADD_CODE:
        HandleMsgAddCode(SenderDevId, (tMessageTypeAddCode*)(pFrame->Data));
        break;
    case MESSAGE_TYPE_TRIGGER_CODE:
        HandleMsgTriggerCode((tMessageTypeTriggerCode *)(pFrame->Data));
        break;

    }
}

void tKeyReciever::HandleMsgEepromCrcRequest(uint8_t SenderID)
{

  int NumOfCodes= EEPROM.read(KEY_CODE_TABLE_USAGE_OFFSET);
  tMessageTypeEepromCRCResponse Msg;
  Msg.NumOfActions = NumOfCodes;
  Msg.EepromCRC = 0;    // that was pointless
  CommSenderProcess::Instance->Enqueue(SenderID,MESSAGE_TYPE_EEPROM_CRC_RESPONSE,sizeof(Msg), &Msg);
}

#if CONFIG_DEBUG_NODE
void tKeyReciever::HandleMsgEepromCrcResponse(uint8_t SenderID, tMessageTypeEepromCRCResponse* Message)
{
    LOG_PRINT("Eeprom CRC for device ");
    LOG(print(SenderID,HEX));
    LOG_PRINT(" num of actions=");
    LOG(print(Message->NumOfActions,DEC));
    LOG_PRINT(" CRC=");
    LOG(println(Message->EepromCRC,DEC));
}

void tKeyReciever::HandleMsgButtonPress(tMessageTypeButtonPress* Msg)
{
    LOG_PRINT(" BUTTON PRESS short: ");
    LOG(print(Msg->ShortClick, BIN));
    LOG_PRINT(" long: ");
    LOG(println(Msg->DoubleClick, BIN));
}

void tKeyReciever::HandleMsgCodeRecieved(tMessageTypeCodeRecieved *Msg)
{
    LOG_PRINT(" Code recieved, type: ");
    LOG(print(Msg->type, DEC));
    LOG_PRINT(" value: ");
    LOG(print(Msg->code, DEC));
    LOG_PRINT(" 0x");
    LOG(println(Msg->code, HEX));
}
#endif CONFIG_DEBUG_NODE

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
	LOG_PRINTLN("INCORRECT CODE - event sent");

	tMessageTypeButtonPress Msg;
    Msg.DoubleClick = 0xFFFF;
    Msg.ForceSrcId = 0;
    Msg.LongClick = 0;
    Msg.ShortClick = 0;
    CommSenderProcess::Instance->Enqueue(DEVICE_ID_BROADCAST,MESSAGE_BUTTON_PRESS,sizeof(Msg),&Msg);
}

void tKeyReciever::sendMatchCodeEvent(tMessageTypeAddCode *pValidCode)
{
	LOG_PRINTLN("CODE ACCEPTED - event sent");

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
	mDigitsCode *= 10;
	mDigitsCode += code;
	mDigitsCollected++;
}

void tKeyReciever::handleCode(uint32_t code, uint8_t type)
{
    DEBUG_PRINT_3("Processing code type: ");
    DEBUG_3(print(type, DEC));
    DEBUG_PRINT_3(" value: ");
    DEBUG_3(print(code, DEC));
    DEBUG_PRINT_3(" 0x");
    DEBUG_3(println(code, HEX));

    tMessageTypeCodeRecieved Msg;
    Msg.code = code;
    Msg.type = type;

    CommSenderProcess::Instance->Enqueue(DEVICE_ID_BROADCAST,MESSAGE_TYPE_CODE_RECIEVED,sizeof(Msg),&Msg);

    uint8_t NumOfEnties = EEPROM.read(KEY_CODE_TABLE_USAGE_OFFSET);

    if (NumOfEnties > 0)
    {
        for (uint8_t i = 0; i < NumOfEnties; i++)
        {
            DEBUG_PRINT_2("Check entry ");
            DEBUG_2(println(i, DEC));

            tMessageTypeAddCode ValidCode;
            EEPROM.get(KEY_CODE_TABLE_OFFSET+(KEY_CODE_TABLE_SIZE*i),ValidCode);

            DEBUG_PRINT_2("Entry type ");
            DEBUG_2(print(ValidCode.type, DEC));
            DEBUG_PRINT_2(" value: ");
            DEBUG_2(print(ValidCode.code, DEC));
            DEBUG_PRINT_2(" 0x");
            DEBUG_2(println(ValidCode.code, HEX));

            if (ValidCode.type != type)
                continue;
            if (ValidCode.code != code)
                continue;

//            if (ValidCode.ValidStart && ValidCode.ValidEnd)
//            {
//                uint16_t ts = tTimestamp::get();
//                ts = ts >> 8;   // eldest 8 bits
//
//                if (ValidCode.ValidStart > ts)
//                    continue;
//                if (ValidCode.ValidEnd < ts)
//                    continue;
//            }
//
            sendMatchCodeEvent(&ValidCode);
            return;
        }
	}

	sendIncorrectCodeEvent();
}

void tKeyReciever::HandleMsgAddCode(uint8_t SenderDevId, tMessageTypeAddCode *Msg)
{
    uint8_t NumOfEnties = EEPROM.read(KEY_CODE_TABLE_USAGE_OFFSET);

    if (NumOfEnties >= KEY_CODE_TABLE_SIZE)
    {
        // out of eeprom
        DEBUG_PRINT_3("Out of eeprom: ");
        DEBUG_3(println(NumOfEnties, DEC));
        tOutgoingFrames::SendMsgStatus(SenderDevId, STATUS_OUT_OF_MEMORY);
        return;
    }

    DEBUG_PRINT_3("Adding code at slot ");
    DEBUG_3(println(NumOfEnties, DEC));
    DEBUG_PRINT_3("   type");
    DEBUG_3(print(Msg->type, DEC));
    DEBUG_PRINT_3("   value: ");
    DEBUG_3(print(Msg->code, DEC));
    DEBUG_PRINT_3(" 0x");
    DEBUG_3(println(Msg->code, HEX));
//    DEBUG_PRINT_3("  Valid start: ");
//    DEBUG_3(print(Msg->ValidStart, DEC));
//    DEBUG_PRINT_3(" valid end: ");
//    DEBUG_3(println(Msg->ValidEnd, DEC));
    DEBUG_PRINT_3("  MessageKeyMap: ");
    DEBUG_3(println(Msg->ButtonBitmap, BIN));


    EEPROM.put(KEY_CODE_TABLE_OFFSET+(KEY_CODE_TABLE_SIZE*NumOfEnties),*Msg);

    NumOfEnties++;
    EEPROM.write(KEY_CODE_TABLE_USAGE_OFFSET, NumOfEnties);
}

void tKeyReciever::HandleMsgTriggerCode(tMessageTypeTriggerCode *Msg)
{
    handleCode(Msg->code, Msg->type);
}

void tKeyReciever::HandleMsgEepromClearCodes()
{
      EEPROM.write(KEY_CODE_TABLE_USAGE_OFFSET, 0);
}

