#ifndef DRV8908_H
#define DRV8908_H

#include "mbed.h"
#include "edevice.h"
#include "drv89xxmotor.h"
#include "drv89xxenums.h"

//***** SPI settings *****
#define DRV8908_SPI_FREQ 5000000
#define DRV8908_SPI_BITS 16
#define DRV8908_SPI_MODE 1

//***** General definitions *****
#define DRV8908_READ_ADDRESS 0x4000
#define DRV89XX_MAX_MOTORS 4
#define DRV89XX_CONFIG_BYTES 0x25
#define DRV89XX_UPDATE_START 0x08
#define DRV89XX_UPDATE_END 0x16

class DRV8908 : IEDevice {
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
        void configureMotor(uint8_t id, HalfBridge half_bridg1, HalfBridge half_bridge2, PWMChannel pwm_channel, uint8_t reverse_delay);
        void setMotor(uint8_t id, uint8_t speed, Direction direction);
        void disableMotor(uint8_t id);
        void writeChanges();

        void checkDeviceOperation(Communication *communication_protocol);
        void printErrorStatus();
        void test();

        //Set, disable motor functions
    private:
        SPI spi;
        DigitalOut chip_select;
        DigitalOut sleep_pin;
        DigitalIn fault_pin;

        uint8_t config_cache[DRV89XX_CONFIG_BYTES] = {0};
        DRV89xxMotor motors[DRV89XX_MAX_MOTORS];

        void writeByte(uint8_t address, uint8_t data);
        uint16_t readByte(uint8_t address);
};

#endif
