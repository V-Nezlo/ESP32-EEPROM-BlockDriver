//-----------------------------------------------------------------------
//  File        : SimpleExample.ino
//  Created     : 08.04.2022
//  Modified    : 21.04.2022
//  Author      : V-Nezlo (vlladimirka@gmail.com)
//  Description : Block Driver Example
//-----------------------------------------------------------------------

#include "EepromBlock.hpp"
#include <Arduino.h>

struct WiFiCred {
	char ssid[32];
	char pass[64];
};

struct Internal {
	uint16_t counter;
	uint32_t seconds;
};

struct BrokenData {
	uint8_t nevermind;
};

void setup()
{	
	delay(3000); // Delay for serialmonitor init
	EepromBlock<2> eepromBlock;

	// Shuffle the order of these functions to see how they work.
	eepromBlock.createBlock("internal", sizeof(Internal));
	eepromBlock.createBlock("wifi", sizeof(WiFiCred));
	eepromBlock.createBlock("broken", sizeof(BrokenData)); // Return error in log, pool overhead
	// Shuffling will result in loss of data in memory

	// Structures
	WiFiCred wifiParams;
	Internal internalParams;
	BrokenData brokenDataParams;

	if (eepromBlock.readBlock("wifi", &wifiParams)) { // If read successfully
		ESP_LOGI("wifi", "wifi successfully readed, password is %s and ssid is %s", wifiParams.pass, wifiParams.ssid);
	} else { // Writing data
		strcpy(wifiParams.ssid, "THIS IS SEED");
		strcpy(wifiParams.pass, "THIS IS PASSWORD");
		eepromBlock.writeBlock("wifi", &wifiParams);
	}

	if (eepromBlock.readBlock("internal", &internalParams)) {
		ESP_LOGI("internal", "Internal params successfully readed and counter = %d , seconds = %d ", internalParams.counter, internalParams.seconds);
	} else {
		internalParams.counter = 1337;
		internalParams.seconds = 2882;
		eepromBlock.writeBlock("internal", &internalParams);
	}

	// this returns errors
	eepromBlock.readBlock("broken", &brokenDataParams);
	eepromBlock.writeBlock("broken", &brokenDataParams);

}

void loop()
{

}