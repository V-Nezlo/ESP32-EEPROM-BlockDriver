# ESP32 EEPROM Block Driver

EEPROM block access driver for ESP32
The main task of the driver is to organize a convenient interface for working with non-volatile memory from anywhere in the program.
The driver is based on a "block" - an area of dedicated non-volatile memory for storing information.

## Capabilities

* Create "blocks" of arbitrary size, read and write them
* Eliminated erroneous reading, block crossing, block re-creation
* The interface provides an access point to blocks anywhere in the program
* Block record integrity control

## Peculiarities

This driver does not create a "block" structure inside non-volatile memory, it creates a mask that is superimposed on this memory. The block data looks like this:

```sh
char name - Block name
size_t address - Block start address
size_t size - Size of block
uint16_t crc - Crc
```
Of this data, only crc gets into non-volatile memory, immediately after the block storage object. The mask consists of the block address and its size.

# Usage

```sh
EepromBlock <size_t BlockCount, size_t EepromSize = 512>
```
Constructor without arguments, the template specifies the number of blocks and the size of the EEPROM area

```sh
bool createBlock(const char *aName, size_t aSize)
```
Creates a block with the desired name and size. Returns false if a block with the same name already exists,
the length of the block name is greater than the maximum possible or if the block pool is full.
Otherwise returns true and creates a block

```sh
bool writeBlock(const char *aName, const void* aData) const
```
Writes data to the block if the block exists. Otherwise returns false. The size of the data must match the size of the created block

```sh
bool readBlock(const char *aName, void* aData) const
```
Reads data from a block. Returns false if the block was not found or the CRC did not match.

## Using example

See SimpleExample.cpp in examples folder

## Usage features

The key point is that the user does not change the order in which the block creation functions are called. There is no block re-allocation in the driver, so mixing block creation functions with each other is unacceptable. The driver does not have an EEPROM cleanup function, because if a block is not found in memory, it is not read.

----------------------------------------------------------------------------------------------------

# ESP32 EEPROM Block Driver

Драйвер, организующий блоковый доступ к памяти EEPROM для ESP32
Главная задача драйвера - организовать удобный интерфейс работы с энергонезависимой памятью из любой точки программы.
Основу драйвера составляет "блок" - область выделенной энергонезависимой памяти для хранения там произвольной информации.

## Возможности

* Создание "блоков" произвольного размера, их чтение и запись
* Исключено чтение мусора, пересечение блоков, повторное создание блоков
* Интерфейс обеспечивает точку доступа к блокам в любом месте программы
* Контроль целостности записи блоков

## Особенности

Данный драйвер не создает "блоковую" структуру внутри энергонезависимой памяти, он создает маску, которая накладывается на эту память. Данные о блоке выглядят следующим образом:
```sh
char name - Имя блока
size_t address - Адрес начала блока
size_t size - Размер блока
uint16_t crc - Контрольная сумма блока
```
Из этих данных в энергонезависимую память попадает только crc, сразу после обьекта хранения блока. Маска состоит из адреса блока и его размера.

## Использование

```sh
EepromBlock <size_t BlockCount, size_t EepromSize = 512>
```
Конструктор без аргументов, в шаблоне задается число блоков и размер области EEPROM

```sh
bool createBlock(const char *aName, size_t aSize)
```
Создает блок с нужным именем и размером. Возвращает false если блок с таким именем уже существует,
длина имени блока больше максимально возможной или если пул блоков заполнен.
Иначе возвращает true и создает блок

```sh
bool writeBlock(const char *aName, const void* aData) const
```
Записывает данные в блок если блок существует. Иначе возвращает false. Размер данных должен совпадать с размером созданного блока

```sh
bool readBlock(const char *aName, void* aData) const
```
Читает данные из блока. Возвращает false если блок не найден или CRC не совпал.

## Пример использования

```sh
struct Patient // Структура, которая будет храниться в блоке
{
    char name[32];
    uint32_t money;
    uint8_t age;
    float psyHealth;
};

struct SomeData
{
    uint32_t data1;
    float data2;
    size_t data3;
};

EepromBlock<2> eepromBlock; // Создали обьект драйвера, указали число блоков

eepromBlock.createBlock("Patient", sizeof(Patient)); // Создали блок размером структуры Patient
eepromBlock.createBlock("SomeData", sizeof(SomeData)); // Создали блок для структуры SomeData
// Замена местами функций createBlock вызовет полную потерю всех данных в памяти, поэтому добавлять данные в память без
// их потери следует последовательно

// Проверим, есть ли этот блок в памяти, и если есть - загрузим из него значения
Patient patient;
uint32_t patientMoney;
if (eepromBlock.readBlock("Patient", &patient)) {
    patientMoney = patient.money; // Значение будет прочитано из EEPROM
} else {
    // Тут мы точно знаем что данных в EEPROM нет, поэтому можем дать какое то дефолтное число
    patientMoney = 1000;
}

Patient newPatient {
    "Valera",
    5000,
    54,
    75.0
};

eepromBlock.writeBlock("Patient", &newPatient); // Записали данные 
```

## Особенности использования

Ключевым моментом является то чтобы пользователь не менял порядок вызова функций создания блоков. В драйвере отсутствует переаллокация блоков, поэтому перемешивание функций создания блоков между собой недопустимо. В драйвере отсутствует функция очистки EEPROM, поскольку если блок не найден в памяти - он не читается.
