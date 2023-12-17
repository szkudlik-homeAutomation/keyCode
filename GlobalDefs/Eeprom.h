#pragma once
#include "../global.h"
#include <EEPROM.h>


#define EEPROM_CANNARY 0xA0     // change if update changes EEPROM structure

#define KEY_CODE_BYTE_SIZE ((KEY_CODE_BIT_SIZE + 1) / 8)
#define KEY_CODE_TABLE_SIZE 50	// TODO - calculate

typedef struct tKeyCode
{
	uint8_t Id;			// id of the code
	uint8_t TypeLength; // if 0 - a dongle bitmap. if > 0 - number of key digits
	union {
		uint8_t DongleBitmap[KEY_CODE_BYTE_SIZE];
		uint8_t CodeDigits[KEY_MAX_DIGITS];
	};

	uint16_t ValidStart;	// valid onlt between start-end. If 0 - valid always.
	uint16_t ValidEnd;		// start-end is a time code from the central node
	uint8_t ValidWithId;	// key is valid only if code with this ID has been entered lately.
    uint16_t ShortClick;    // bitmap of buttons clicked in MESSAGE_BUTTON_PRESS triggered by this key
};

typedef struct __tEeprom
{
  uint8_t DeviceID;  // MUST be at the 1st position
  uint8_t CANNARY;   // a cannary, if is not set to EEPROM_CANNARY the eeprom should be set to default
#if CONFIG_NETWORK
  uint8_t IP[4];
  uint8_t IPMask[4];
  uint8_t Gateway[4];
  uint8_t DNS[4];
  uint8_t MAC[6];
#endif CONFIG_NETWORK
  uint8_t KeyCodeUsage;
  tKeyCode KeyCode[KEY_CODE_TABLE_SIZE];
};

#define EEPROM_DEVICE_ID_OFFSET offsetof(__tEeprom,DeviceID)

#define EEPROM_CANNARY_OFFSET offsetof(__tEeprom,CANNARY)

#if CONFIG_NETWORK
#define EEPROM_IP offsetof(__tEeprom,IP[0])
#define EEPROM_IPMASK offsetof(__tEeprom,IPMask[0])
#define EEPROM_GATEWAY offsetof(__tEeprom,Gateway[0])
#define EEPROM_DNS offsetof(__tEeprom,DNS[0])
#define EEPROM_MAC offsetof(__tEeprom,MAC[0])
#endif //CONFIG_NETWORK

#define KEY_CODE_TABLE_USAGE_OFFSET offsetof(__tEeprom,KeyCodeUsage)
#define KEY_CODE_TABLE_OFFSET offsetof(__tEeprom,KeyCode[0])

void SetDefaultEEPromValues();
