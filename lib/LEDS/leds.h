#ifndef LEDS_H
#define LEDS_H

#include "mbed.h"
#include "pinDefinitions.h"

#define NR_OF_LEDS 4

class Leds {
    public:
        Leds();
        void toggleLed(uint8_t ledId);

    private:
       DigitalOut leds[4] = {LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN};
};

#endif