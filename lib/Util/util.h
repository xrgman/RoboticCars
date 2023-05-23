#ifndef UTIL_H
#define UTIL_H

#include "mbed.h"

class Util {
    public:
        static void printAsBinary(int number);
        static void scanForI2CDevices(PinName sda, PinName scl);
        static uint8_t getBitsFromData(uint8_t data, uint8_t start, uint8_t end);
        static uint16_t getBitsFromData(uint16_t data, uint8_t start, uint8_t end);
        static uint32_t getBitsFromData(uint32_t data, uint8_t start, uint8_t end);
};

#endif