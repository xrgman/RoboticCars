#ifndef SENSOR_H
#define SENSOR_H

class ISensor {
    virtual void printSensorReadings() = 0;
    virtual void reset() = 0;
};

#endif