/*
 * IncomingFrameHandler.cpp
 *
 *  Created on: 10 sie 2022
 *      Author: szkud
 */

#include "../global.h"

#include "lib/AceCRC/src/AceCRC.h"
using namespace ace_crc::crc16ccitt_nibble;


#include "tKeyCodeIncomingFrameHanlder.h"
#include "Common_code/TLE8457_serial/TLE8457_serial_lib.h"


void tKeyCodeIncomingFrameHanlder::onMessage(uint8_t type, uint16_t data, void *pData)
{
    uint8_t ret;

    tCommunicationFrame *pFrame = (tCommunicationFrame *)pData;
    uint8_t SenderDevId = pFrame->SenderDevId;

    switch (data)	// messageType
      {

       case MESSAGE_TYPE_EEPROM_CRC_REQUEST:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_EEPROM_CRC_REQUEST");
             HandleMsgEepromCrcRequest(SenderDevId);
           break;

       case MESSAGE_TYPE_EEPROM_CRC_RESPONSE:
             DEBUG_PRINTLN_3("===================>MESSAGE_TYPE_EEPROM_CRC_RESPONSE");
             HandleMsgEepromCrcResponse(SenderDevId,(tMessageTypeEepromCRCResponse*)(pFrame->Data));
           break;
      }
}

void tKeyCodeIncomingFrameHanlder::HandleMsgEepromCrcRequest(uint8_t SenderID)
{

  int NumOfCodes= EEPROM.read(KEY_CODE_TABLE_USAGE_OFFSET);
  tMessageTypeEepromCRCResponse Msg;
  Msg.NumOfActions = NumOfCodes;
  Msg.EepromCRC = 0;	// that was pointless
  CommSenderProcess::Instance->Enqueue(SenderID,MESSAGE_TYPE_EEPROM_CRC_RESPONSE,sizeof(Msg), &Msg);
}


void tKeyCodeIncomingFrameHanlder::HandleMsgEepromCrcResponse(uint8_t SenderID, tMessageTypeEepromCRCResponse* Message)
{
	LOG_PRINT("Eeprom CRC for device ");
	LOG(print(SenderID,HEX));
	LOG_PRINT(" num of actions=");
	LOG(print(Message->NumOfActions,DEC));
	LOG_PRINT(" CRC=");
	LOG(println(Message->EepromCRC,DEC));
}

