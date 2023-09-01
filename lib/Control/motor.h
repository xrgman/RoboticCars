#ifndef MOTOR_H
#define MOTOR_H

#include "mbed.h"

#define NR_OF_RPM_SAMPLES 10

class Motor {
    public:
        Motor(PinName motorHall1Pin, Timer* timer);
        void initialize();
        float getRPM();

    private:
        Timer *timer;
        InterruptIn motorHallSensor;

        uint32_t lastPulseTime;
        uint8_t rpmIndex;
        uint32_t rpmSum;
        uint32_t rpmValues[NR_OF_RPM_SAMPLES];

        float rpm;


        void hallISR();
};

#endif