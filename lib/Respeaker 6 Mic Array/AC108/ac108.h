#ifndef AC108_H
#define AC108_H

#include "mbed.h"
#include "edevice.h"





class AC108 : IEDevice {
    public:
        AC108(I2C *i2c, uint8_t device_address);

        bool checkDeviceOperation(Communication *communication_protocol);
        bool initialize();
        bool captureStarted();
        bool setVolume(uint8_t vol);
        bool startCapture44();
        bool stopCapture();

    private:
        I2C *i2c;
        uint8_t device_address;
        char readByte(uint8_t address);
        void readBytes(uint8_t address, uint8_t count, uint8_t *dest);
        bool writeByte(uint8_t address, uint8_t data);
};

#endif