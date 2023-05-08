#ifndef DRV8908_H
#define DRV8908_H

#include "mbed.h"
#include "sensor.h"

//***** Pin definitions *****
#define DRV8908_MOSI_PIN PA_7
#define DRV8908_MISO_PIN PB_4
#define DRV8908_SCK_PIN PG_11 //Serial clock pin
#define DRV8908_NSS_PIN PG_10 //Chip select pin
#define DRV8908_SLEEP_PIN PG_13 
#define DRV8908_FAULT_PIN PG_12

//***** SPI settings *****
#define DRV8908_SPI_FREQ 4000000
#define DRV8908_SPI_BITS 16
#define DRV8908_SPI_MODE 1

//***** General definitions *****
#define DRV8908_READ_ADDRESS 0x4000

class DRV8908 : ISensor {
    public:
        DRV8908(PinName mosi, PinName miso, PinName sck, PinName nss, PinName sleep, PinName fault);
        void Initialize();
        void CheckDeviceOperation();
        void PrintSensorReadings();
        void Reset();
        void PrintErrorStatus();
        void Test();
    private:
        SPI spi;
        DigitalOut chip_select;
        DigitalOut sleep_pin;
        DigitalIn fault_pin;

        void WriteByte(uint8_t address, uint8_t data);
        uint16_t ReadByte(uint8_t address);
};

#endif