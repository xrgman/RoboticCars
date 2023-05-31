#ifndef LEDS_H
#define LEDS_H

#include "mbed.h"
#include "pinDefinitions.h"

#define NR_OF_LEDS 4

enum LedsColors
{
    BLUE = 0,
    YELLOW,
    GREEN,
    RED
};

class Leds {
    public:
        enum Effect
        {
            NONE,
            VISOR
        };


        void enableLed(uint8_t ledId);
        void disableLed(uint8_t ledId);
        void toggleLed(uint8_t ledId);
        bool getLed(uint8_t ledId);
        void setEffect(Effect effect);
        Effect getCurrentEffect();
        void processLedEffect(uint32_t systemCounter);

    private:
       DigitalOut leds[4] = {LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN};
       Effect currentLedEffect = NONE;
       bool reverse = false;

       void processVisorEffect();
};

#endif