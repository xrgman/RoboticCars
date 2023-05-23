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
        enum DeviceId {
            drv8912 = 0,
            drv8910,
            drv8908,
            drv8906,
            drv8904
        };

        DRV8908(PinName mosi, PinName miso, PinName sck, PinName nss, PinName sleep, PinName fault);
        void initialize();
        void checkDeviceOperation(Communication *communication_protocol);
        void printSensorReadings();
        void reset();
        void printErrorStatus();
        void test();
    private:
        SPI spi;
        DigitalOut chip_select;
        DigitalOut sleep_pin;
        DigitalIn fault_pin;

        void writeByte(uint8_t address, uint8_t data);
        uint16_t readByte(uint8_t address);
};

#endif