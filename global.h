/*
 * global.h
 *
 * a common file that MUST be included in every .h and .c**
 *
 */

#pragma once

#define COMM_SERIAL Serial1
#define COMM_SERIAL_EVENT serialEvent1

#define CONFIG_LOGGER 1

#define DEBUG_3_ENABLE
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_EVENT serialEvent

#define CONFIG_NETWORK 1
#define CONFIG_HTTP_SERVER 0
#define CONFIG_TELNET_SERVER 1
#define CONFIG_WATCHDOG 1

#define CONFIG_TLE8457_COMM_LIB 1

#define KEY_CODE_BIT_SIZE 36
#define KEY_MAX_DIGITS 5


#include <Arduino.h>
#include "src/Common_code/helpers.h"
#include "GlobalDefs/version.h"
#include "GlobalDefs/Eeprom.h"
#include "GlobalDefs/CommDefs.h"
#include "GlobalDefs/TLE8457_serial_lib_defs.h"
#include "src/Common_code/debug.h"
#include "src/Common_code/status.h"
