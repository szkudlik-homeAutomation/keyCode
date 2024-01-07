#pragma once

#include "../global.h"
#include "TLE8457_serial_lib_defs.h"

// a double click - two short clicks occur before max time
#define DOUBLE_CLICK_TICKS_MAX   8   // 400ms 

// maximum number of nodes, should not be mode than 32 because of bitmaps
#define MAX_NUM_OF_NODES 32

#define NUM_OF_OUTPUTS 8

/**
 * Sent by the central node - arbitrary set state of a single output
 * this is a broadcast frame
 */
#define MESSAGE_BUTTON_PRESS 0x06
typedef struct 
{
  uint16_t ShortClick;   // bitmap of new buttons clicked
  uint16_t LongClick;    // bitmap of new long clicks (note! it will always be preceded by "short click"
  uint16_t DoubleClick;  // bitmap of new double clicks
  uint8_t  ForceSrcId;   // if != 0, the reciever will take this as a sender ID. If == 0, the real sender will be taken (for compatibility)
} tMessageTypeButtonPress;
C_ASSERT(sizeof(tMessageTypeButtonPress) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/**
 * Set a reaction for a button pressed 
 * send by the central node to setup
 */
#define MESSAGE_TYPE_SET_ACTION 0x07

#define BUTTON_TRIGGER_TYPE_CLICK            0
#define BUTTON_TRIGGER_TYPE_LONG_CLICK       1
#define BUTTON_TRIGGER_TYPE_DOUBLE_CLICK     2
#define BUTTON_TRIGGER_TYPE_ANY_CLICK        3

#define BUTTON_ACTION_TYPE_TOGGLE     0
#define BUTTON_ACTION_TYPE_ON         1
#define BUTTON_ACTION_TYPE_OFF        2

typedef struct 
{
  uint8_t  OutId;         // the output id to be set
  uint8_t  SenderDevID;   // the ID of the sender of a MESSAGE_BUTTON_PRESS message
  uint8_t  ButtonId    : 4, // the ID of the button triggering the action
           TriggerType : 2, // type of button action -  BUTTON_TRIGGER_TYPE*
           ActionType  : 2; // type of action - BUTTON_ACTION_TYPE*
  uint16_t Timer;         // in case of button is set to "1", by on or toggle, number of seconds after it should be turned off
  uint8_t  OutputsMask;   // bitmap of other outputs to be checked before action
  uint8_t  OutputsStates; // required states of outputs. All masked bits MUST be zero
} tMessageTypeSetAction;
C_ASSERT(sizeof(tMessageTypeSetAction) <= COMMUNICATION_PAYLOAD_DATA_SIZE);


/**
 * Erase all actions - clear Eeprom actions map, all actions need to be re-programmed
 */
#define MESSAGE_TYPE_CLEAR_ACTIONS 0x08

/**
 * get a CRC of all eeprom
 * sent by the central node, the node must respond with MESSAGE_TYPE_EEPROM_CRC_RESPONSE
 */
#define MESSAGE_TYPE_EEPROM_CRC_REQUEST 0x0B
#define MESSAGE_TYPE_EEPROM_CRC_RESPONSE 0x0C
typedef struct 
{
  uint8_t  NumOfActions;
  uint16_t EepromCRC;
} tMessageTypeEepromCRCResponse;
C_ASSERT(sizeof(tMessageTypeEepromCRCResponse) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

#define MESSAGE_TYPE_SET_DEFAULT_TIMER 0x0F
typedef struct 
{
  uint8_t  OutputID;              // output ID 
  uint16_t DefaultTimerValue;     // timer value
} tMessageTypeSetDefaultTimer;
C_ASSERT(sizeof(tMessageTypeSetDefaultTimer) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

#define MESSAGE_TYPE_DEFAULT_TIMER_REQUEST 0x10
typedef struct 
{
  uint8_t  OutputID;      // id of an output 
} tMessageTypeDefaultTimerRequest;
C_ASSERT(sizeof(tMessageTypeDefaultTimerRequest) <= COMMUNICATION_PAYLOAD_DATA_SIZE);


#define MESSAGE_TYPE_DEFAULT_TIMER_RESPONSE 0x11
typedef struct 
{
  uint8_t  OutputID;      // id of an output
  uint16_t DefTimerValue; // default timer. 0 means there's no timer (set forever)
} tMessageTypeDefaultTimerResponse;
C_ASSERT(sizeof(tMessageTypeDefaultTimerResponse) <= COMMUNICATION_PAYLOAD_DATA_SIZE);

/**
 * Erase all codes - clear Eeprom codes  map, all codes need to be re-programmed
 * for keyCode
 */
#define MESSAGE_TYPE_CLEAR_CODES 0x80

/**
 * add a code to eeprom
 */
typedef struct
{
	uint8_t size;		// code size, 0 - a dongle, otherwise - number of digits
	uint32_t code;		// a code in binary format

	uint8_t ValidStart;	    // valid only between start-end. If 0 - valid always.
	uint8_t ValidEnd;		// start-end is a time code from the central node. Only older byte from 16-bit timestamp
    uint8_t ButtonBitmap;   // bitmap of buttons shortClick in MESSAGE_BUTTON_PRESS triggered by this key
    						// incorrect code triggers all buttons long pressed
                            // NOTE - for backward compatibility with LightControl, where messages are 8 bytes long, bitmap must be shortened to 8 bits
} tMessageTypeAddCode;
C_ASSERT(sizeof(tMessageTypeAddCode) <= COMMUNICATION_PAYLOAD_DATA_SIZE);
#define MESSAGE_TYPE_ADD_CODE 0x81

/**
 * trigger a code - for debug, inject code as if entered on keypad
 */
typedef struct
{
	uint8_t size;		// code size, 0 - a dongle, otherwise - number of digits
	uint32_t code;		// a code in binary format
} tMessageTypeTriggerCode;
C_ASSERT(sizeof(tMessageTypeTriggerCode) <= COMMUNICATION_PAYLOAD_DATA_SIZE);
#define MESSAGE_TYPE_TRIGGER_CODE 0x82

/**
 * code recieved - an event, to retrieve all codes. Sent as a broadcast
 */
typedef struct
{
	uint8_t size;		// code size, 0 - a dongle, otherwise - number of digits
	uint32_t code;		// a code in binary format
} tMessageTypeCodeRecieved;
C_ASSERT(sizeof(tMessageTypeCodeRecieved) <= COMMUNICATION_PAYLOAD_DATA_SIZE);
#define MESSAGE_TYPE_CODE_RECIEVED 0x83
