#ifndef UTIL_H
#define UTIL_H

#include "mbed.h"

class Util {
    public:
        static void printAsBinary(int number);
        static void scanForI2CDevices(PinName sda, PinName scl);
};

#endif