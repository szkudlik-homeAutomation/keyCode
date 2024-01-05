/*
 * telnetServerCommands.cpp
 *
 *  Created on: 6 pa� 2022
 *      Author: szkud
 */

#include "../global.h"
#if CONFIG_TELNET_SERVER

#include "lib/Commander/src/Commander.h"
#include "Common_code/Network/telnetServer.h"
#include "Common_code/TLE8457_serial/TLE8457_serial_lib.h"

bool send_ClearCodes(Commander &Cmdr);
bool send_addCode(Commander &Cmdr);
bool send_GetEepromCrc(Commander &Cmdr);
bool send_triggerCode(Commander &Cmdr);

const commandList_t TelnetCommands[] = {
  {"enableLogs",      TelnetEnableLogs,             "enable logs on telnet console"},
  {"disableLogs",     TelnetDisableLogs,            "enable logs on telnet console"},
#if CONFIG_TLE8457_COMM_LIB
  {"GetVersion",      send_GetVersion,              "show version"},
  {"Reset",           send_Reset,                   "reset the system"},
  {"GetEepromCrc",    send_GetEepromCrc,            "GetEepromCrc dev_id"},
  {"ClearCodes",      send_ClearCodes,              "ClearCodes dev_id"},
  {"AddCode",		send_addCode, "AddCode dev_id type code ButtonBitmap [validStart validEnd]"},
  {"TriggerCode",	send_triggerCode, "dev_id type code"},
#endif //CONFIG_TLE8457_COMM_LIB
};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));

bool send_ClearCodes(Commander &Cmdr)
{
  int Dst;
  if(Cmdr.getInt(Dst))
  {
	  CommSenderProcess::Instance->Enqueue(Dst,MESSAGE_TYPE_CLEAR_CODES,0,NULL);
  }
  else
  {
    Cmdr.println(F("Usage: ClearCodes dst_dev_id"));
    return false;
  }

  return true;
}

bool send_addCode(Commander &Cmdr)
{
    int Dst;
    int type;
    uint32_t code;
    int ButtonBitmap;
    int validStart = 0;
    int validEnd = 0;
    if(! Cmdr.getInt(Dst))
        goto error;
    if(! Cmdr.getInt(type))
        goto error;
    if(! Cmdr.getLong(code))
        goto error;
    if(! Cmdr.getInt(ButtonBitmap))
        goto error;

    if(! Cmdr.getInt(validStart))
        goto finish;
    if(! Cmdr.getInt(validEnd))
        goto error;

  finish:
    tMessageTypeAddCode Msg;
    Msg.code = code;
    Msg.type = type;
    Msg.ValidEnd = validEnd;
    Msg.ValidStart = validStart;
    Msg.ButtonBitmap = ButtonBitmap;

    CommSenderProcess::Instance->Enqueue(Dst,MESSAGE_TYPE_ADD_CODE,sizeof(Msg),&Msg);

    return true;

  error:
        Cmdr.println(F("Usage: AddCode dev_id type code ButtonBitmap [validStart validEnd]"));
        Cmdr.println(F("   type 0 - a dongle, 1 - keySequence"));
        Cmdr.println(F("   code a code in binary format"));
        Cmdr.println(F("   ButtonBitmap bitmap of simulated key Press when code is accepted"));
        Cmdr.println(F("   [validStart validEnd] timestamps from-to the code is valid"));
        return false;
}

bool send_triggerCode(Commander &Cmdr)
{
	  int Dst;
	  int type;
	  int code;
	  if(! Cmdr.getInt(Dst))
		  goto error;
	  if(! Cmdr.getInt(type))
		  goto error;
	  if(! Cmdr.getInt(code))
		  goto error;

	  tMessageTypeTriggerCode Msg;
	  Msg.code = code;
	  Msg.type = type;
	  CommSenderProcess::Instance->Enqueue(Dst,MESSAGE_TYPE_TRIGGER_CODE,sizeof(Msg),&Msg);
	  return true;

	error:
	    Cmdr.println(F("Usage: TriggerCode dev_id type code"));
	    Cmdr.println(F("   type 0 - a dongle, 1 - keySequence"));
	    Cmdr.println(F("   code a code in binary format"));
	    return false;
}

bool send_GetEepromCrc(Commander &Cmdr)
{
  int Dst;
  if(Cmdr.getInt(Dst))
  {
	  CommSenderProcess::Instance->Enqueue(Dst,MESSAGE_TYPE_EEPROM_CRC_REQUEST,0,NULL);
  }
  else
  {
    Cmdr.println(F("Usage: StateOverview dst_dev_id"));
    return false;
  }

  return true;
}

#endif CONFIG_TELNET_SERVER
