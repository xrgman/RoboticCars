#ifndef SENSOR_H
#define SENSOR_H

#include "communication.h"

class ISensor {
    virtual void checkDeviceOperation(Communication *communication_protocol) = 0;
    virtual void printSensorReadings() = 0;
    virtual void reset() = 0;
};

#endif