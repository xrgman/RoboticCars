#ifndef UTIL_H
#define UTIL_H

#include "mbed.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))

class Util {
    public:
        static void printAsBinary(int number);
        static void scanForI2CDevices(PinName sda, PinName scl);
        static uint8_t getBitsFromData(uint8_t data, uint8_t start, uint8_t end);
        static uint16_t getBitsFromData(uint16_t data, uint8_t start, uint8_t end);
        static uint32_t getBitsFromData(uint32_t data, uint8_t start, uint8_t end);
        static bool setBits(uint16_t *data, uint8_t start, uint8_t nrOfBits, uint8_t value);
        static uint16_t to_ui16(char *pData);
        static void delay(uint32_t duration);
};

#endif