#ifndef SENSOR_H
#define SENSOR_H

class ISensor {
    virtual void CheckDeviceOperation() = 0;
    virtual void PrintSensorReadings() = 0;
    virtual void Reset() = 0;
};

#endif