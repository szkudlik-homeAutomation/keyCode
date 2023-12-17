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

#define CONFIG_NETWORK 0
#define CONFIG_HTTP_SERVER 0
#define CONFIG_TELNET_SERVER 0
#define CONFIG_WATCHDOG 1

#define CONFIG_TLE8457_COMM_LIB 0

#include <Arduino.h>
#include "src/Common_code/helpers.h"
#include "GlobalDefs/version.h"
#include "GlobalDefs/Eeprom.h"
#include "GlobalDefs/CommDefs.h"
#include "src/Common_code/debug.h"
#include "src/Common_code/status.h"
