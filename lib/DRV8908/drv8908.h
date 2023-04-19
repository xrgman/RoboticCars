#ifndef DRV8908_H
#define DRV8908_H

#include "mbed.h"

//***** Pin definitions *****
#define DRV8908_MOSI_PIN PA_7
#define DRV8908_MISO_PIN PB_4
#define DRV8908_SCK_PIN PG_11 //Serial clock pin
#define DRV8908_NSS_PIN PG_10 //Chip select pin
#define DRV8908_SLEEP_PIN PG_13
#define DRV8908_FAULT_PIN PG_12

//***** SPI settings *****
#define DRV8908_SPI_FREQ 5000000
#define DRV8908_SPI_BITS 16
#define DRV8908_SPI_MODE 1

class DRV8908 {
    public:
        DRV8908(PinName mosi, PinName miso, PinName sck, PinName nss, PinName sleep, PinName fault);
        void Initialize();
        void PrintErrorStatus();
    private:
        SPI spi;
        DigitalOut chip_select;
        DigitalOut sleep_pin;
        DigitalIn fault_pin;

        void writeByte(uint8_t address, uint8_t data);
        uint8_t readByte(uint8_t address);
};

#endif