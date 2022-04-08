//-----------------------------------------------------------------------
//  File        : SimpleExample.ino
//  Created     : 08.04.2022
//  Modified    : 08.04.2022
//  Author      : V-Nezlo (vlladimirka@gmail.com)
//  Description : Block Driver Example
//-----------------------------------------------------------------------
#include <Arduino.h>
#include "EepromBlock.hpp"

struct Data
{
    char name[32];
    uint32_t id;
    float level;
};

void setup()
{
    Serial.begin(115200);
    String outMessage;
    Data data
    {
        "Giorno Giovanna",
        123456,
        75.2
    };

    EepromBlock<1> eepromBlock;
    eepromBlock.createBlock("Data", sizeof(Data));

    Data receivedData;
    if (eepromBlock.readBlock("Data", &receivedData)) {
        outMessage = "Data founded in EEPROM, and name of the character is: " + String(receivedData.name);
        Serial.println(outMessage);
        while(1) {} // infinite loop
    } else {
        outMessage = "Data not founded in EEPROM";
        Serial.println(outMessage);
    }

    eepromBlock.writeBlock("Data", &data);
    Serial.println("Write test Data in EEPROM");

    if (eepromBlock.readBlock("Data", &receivedData)) {
        Serial.println("Data was successfully written");
    } else {
        Serial.println("Critical failure");
    }

}

void loop()
{

}