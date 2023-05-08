#ifndef MPU9250_H
#define MPU9250_H

#include "mbed.h"
#include "sensor.h"

//***** Pin definitions *****
#define MPU9250_SCL PF_1
#define MPU9250_SDA PF_0
#define MPU9250_INT PF_2

//***** Address of MPU9250 *****
#define MPU9250_ADDRESS 0x68 << 1 //Shifter left by one because mbed uses 8 bit addresses :)

typedef struct {
    float ax, ay, az;
} AccelerometerData;

typedef struct {
    float gx, gy, gz;
} GyroscopeData;



class MPU9250 : ISensor {
    public:
        enum Gscale {
            GFS_250DPS = 0,
            GFS_500DPS,
            GFS_1000DPS,
            GFS_2000DPS
        };

        enum Ascale {
            AFS_2G = 0,
            AFS_4G,
            AFS_8G,
            AFS_16G
        };

        

        MPU9250(PinName scl, PinName sda, PinName interupt);
        void CheckDeviceOperation();
        void PrintSensorReadings();

        void Initialize(Gscale gyroscope_scale, Ascale accelerometer_scale);
        void Reset();
        void Calibrate();
        

        AccelerometerData ReadAccelerometerData();
        GyroscopeData ReadGyroscopeData();
        float ReadTemperatureData();

    private:
        I2C i2c;
        char ReadByte(uint8_t address);
        void ReadBytes(uint8_t address, uint8_t count, uint8_t *dest);
        void WriteByte(uint8_t address, uint8_t data);
        void SelfTest();
};

#endif