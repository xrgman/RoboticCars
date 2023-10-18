#ifndef AK8963_H
#define AK8963_H

#include "mbed.h"
#include "edevice.h"
#include "sensor.h"

//***** Address of MPU9250 *****
#define AK8963_ADDRESS 0x0C<<1 //Shifter left by one because mbed uses 8 bit addresses :)

typedef struct {
    float mx, my, mz;
} MagnetometerData;

class AK8963 : ISensor, IEDevice {
    public: 
        enum Mscale {
            MFS_14BITS = 0, // 0.6 mG per LSB
            MFS_16BITS      // 0.15 mG per LSB
        };

        enum Mmode {
            MOP_POWER_DOWN = 0,
            MOP_SINGLE = 1,
            MOP_CONINUES_1 = 2,
            MOP_CONINUES_2 = 6,
            MOP_EXTERNAL = 4,
            MOP_SELF_TEST = 8,
            MOP_FUSE_ROM = 15
        };

        AK8963(PinName scl, PinName sda);
        bool checkDeviceOperation(Communication *communication_protocol);
        void printSensorReadings();

        void initialize(Mscale magnetometer_scale, Mmode magnetometer_mode);
        void reset();
        void calibrate();

        bool isDataReady();
        bool readMagnetometerData(MagnetometerData *data);

    private:
        I2C i2c;
        Mscale magnetometerScale;
        Mmode magnetometerMode;
        float magnetometer_sensitivity_adjustment[3];

        char readByte(uint8_t address);
        void readBytes(uint8_t address, uint8_t count, uint8_t *dest);
        void writeByte(uint8_t address, uint8_t data);

};

#endif