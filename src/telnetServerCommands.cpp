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
bool send_GetEepromCrc(Commander &Cmdr);
const commandList_t TelnetCommands[] = {
  {"enableLogs",      TelnetEnableLogs,             "enable logs on telnet console"},
  {"disableLogs",     TelnetDisableLogs,            "enable logs on telnet console"},
#if CONFIG_TLE8457_COMM_LIB
  {"GetVersion",      send_GetVersion,              "show version"},
  {"Reset",           send_Reset,                   "reset the system"},
  {"GetEepromCrc",    send_GetEepromCrc,            "MESSAGE_TYPE_EEPROM_CRC_REQUEST dev_id"},
#endif //CONFIG_TLE8457_COMM_LIB
};

tTelnetServer TelnetServer(TelnetCommands,sizeof(TelnetCommands));


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
