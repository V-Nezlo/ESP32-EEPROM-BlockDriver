//-----------------------------------------------------------------------
//  File        : EepromBlock.hpp
//  Created     : 04.04.2022
//  Modified    : 08.04.2022
//  Author      : V-Nezlo (vlladimirka@gmail.com)
//  Description : Block driver for ESP32 Eeprom
//-----------------------------------------------------------------------

#ifndef EEPROM_BLOCK_DRIVER_HPP_
#define EEPROM_BLOCK_DRIVER_HPP_

#include <EEPROM.h>

template <size_t BlockCount, size_t EepromSize = 512>
class EepromBlock {

	static constexpr size_t maxNameLength{20};
	static constexpr uint16_t blockNotFoundMark{0xFFFF};
	static constexpr char tag[]{"EepromBlock"};

	struct BlockParams {
		char name[maxNameLength];
		size_t address;
		size_t size;
		uint16_t crc;
	};

public:
	EepromBlock() :
		blockCounter{0},
		bytesCount{0},
		blocks{}
	{
		EEPROM.begin(EepromSize);
	}

	bool createBlock(const char *aName, size_t aSize)
	{
		size_t nameLength = strlen(aName) + 1; // Не забываем про нуль-терминатор

		if (nameLength > maxNameLength) {
			ESP_LOGE(tag, "Creating block %s return error: block name too long", aName);
			return false;
		}

		if (blockCounter >= BlockCount) {
			ESP_LOGE(tag, "Creating block %s return error: block pool is full", aName);
			return false;
		}

		if (findBlockByName(aName) != blockNotFoundMark) {
			ESP_LOGE(tag, "Creating block %s return error: block already exist", aName);
			return false;
		}

		memcpy(blocks[blockCounter].name, aName, nameLength);
		blocks[blockCounter].address = bytesCount;
		blocks[blockCounter].size = aSize;
		blocks[blockCounter].crc = crc16(aName, nameLength);
		
		blockCounter += 1;
		bytesCount += aSize + 2; // 2 дополнительных байта CRC
		return true;
	}

	bool writeBlock(const char *aName, const void* aData) const
	{
		// Найдем номер нашего блока
		uint16_t blockNum = findBlockByName(aName);

		if (blockNum == blockNotFoundMark) {
			ESP_LOGE(tag, "Writing block %s return error: block not found", aName);
			return false;
		}

		size_t blockSize = blocks[blockNum].size;
		size_t desiredAddress = blocks[blockNum].address;

		// Если блок есть - записываем в него данные и crc
		EEPROM.writeBytes(desiredAddress, aData, blockSize);
		EEPROM.writeUShort(desiredAddress + blockSize, blocks[blockNum].crc);
		EEPROM.commit();
		ESP_LOGI(tag, "Block %s write successful", aName);
		return true;
	}

	bool readBlock(const char *aName, void* aData) const
	{

		uint16_t blockNum = findBlockByName(aName);

		if (blockNum == blockNotFoundMark) {
			ESP_LOGE(tag, "Reading block %s return error: block not found", aName);
			return false;
		}

		size_t blockSize = blocks[blockNum].size;
		size_t desiredAddress = blocks[blockNum].address;
		uint16_t receivedCrc;
		EEPROM.get(desiredAddress + blockSize, receivedCrc);
		// Если crc не сошелся - возвращаем false
		if (blocks[blockNum].crc != receivedCrc) {
			ESP_LOGE(tag, "Reading block %s return error: CRC does not match", aName);
			return false;
		} 

		// Если crc сошелся - забираем из блока данные
		EEPROM.readBytes(desiredAddress, aData, blockSize);
		ESP_LOGI(tag, "Block %s read successful", aName);
		return true;
	}

private:

	uint16_t findBlockByName(const char *aName) const 
	{
		for (size_t i = 0; i < BlockCount; ++i) {
			if (!strcmp(blocks[i].name, aName)) {
				return i;
			}
		}
		return blockNotFoundMark;
	}

	uint16_t crc16(const char* data_p, unsigned char length) {
		unsigned char x;
		uint16_t crc = 0xFFFF;

		while (length--){
			x = crc >> 8 ^ *data_p++;
			x ^= x>>4;
			crc = (crc << 8) ^ (static_cast<uint16_t>(x << 12)) ^ (static_cast<uint16_t>(x <<5)) 
			^ (static_cast<uint16_t>(x));
		}
		return crc;
	}

	size_t blockCounter;
	size_t bytesCount;
	BlockParams blocks[BlockCount];
};

#endif