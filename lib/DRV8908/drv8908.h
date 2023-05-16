#ifndef DRV8908_H
#define DRV8908_H

#include "mbed.h"
#include "sensor.h"

//***** SPI settings *****
#define DRV8908_SPI_FREQ 5000000
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