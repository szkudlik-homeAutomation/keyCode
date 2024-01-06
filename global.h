/*
 * global.h
 *
 * a common file that MUST be included in every .h and .c**
 *
 */

#pragma once


//#define CONFIG_DEBUG_NODE 1     //!!!!!!


#ifdef __AVR_ATmega2560__
#define CONFIG_DEBUG_NODE 1
#elif __AVR_ATmega328P__
#else
#error unknown board
#endif

#if CONFIG_DEBUG_NODE
#define COMM_SERIAL Serial1
#define COMM_SERIAL_EVENT serialEvent1
#else // CONFIG_DEBUG_NODE
#define COMM_SERIAL Serial
#define COMM_SERIAL_EVENT serialEvent
#endif // CONFIG_DEBUG_NODE


#if CONFIG_DEBUG_NODE

#define CONFIG_LOGGER 1

#define DEBUG_3_ENABLE
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_EVENT serialEvent

#define CONFIG_NETWORK 1
#define CONFIG_TELNET_SERVER 1
#define CONFIG_HTTP_SERVER 1
#define CONFIG_TIMESTAMP_SERVLET 1

#define CONFIG_SENSOR_HUB 0
#define CONFIG_SENSORS_JSON_OUTPUT 0

#else // CONFIG_DEBUG_NODE
#define CONFIG_LOGGER 0
#endif // CONFIG_DEBUG_NODE

#define CONFIG_WATCHDOG 1
#define CONFIG_TLE8457_COMM_LIB 1

#define KEY_CODE_BIT_SIZE 36
#define KEY_MAX_DIGITS 9

#define CONFIG_SENSORS 1
#define CONFIG_SENSOR_GENERATE_EVENTS 1
#define CONFIG_WIEGAND_SENSOR 1

#define WIEGAND_SENSOR_ID 1
#define CONFIG_WIEGAND_D0_PIN 2		// for both Arduino Nano/Uno.
#define CONFIG_WIEGAND_D1_PIN 3

#define CONFIG_KEYCODE_TIMEOUT 2500

#define CONFIG_TIMESTAMP 0

#include <Arduino.h>
#include "src/Common_code/helpers.h"
#include "GlobalDefs/version.h"
#include "GlobalDefs/Eeprom.h"
#include "GlobalDefs/CommDefs.h"
#include "GlobalDefs/TLE8457_serial_lib_defs.h"
#include "src/Common_code/debug.h"
#include "src/Common_code/status.h"
