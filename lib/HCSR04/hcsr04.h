#ifndef HCSR04_H
#define HCSR04_H

#include "mbed.h"

class HCSR04 {
    public:
        HCSR04(PinName triggerPin, PinName echoPin);
        void startMeasurement();
        bool isNewDataReady();
        int getDistanceCm();
        int getDistanceInch();
    private:
        DigitalOut trigger;
        InterruptIn echo;
        Timer timer;
        Timeout triggerTimeOut, echoTimeOut;
        bool timerStarted, newDataReady;
        int pulseDuration, distance;

        void disableTrigger();
        void onEchoRise();
        void onEchoFall();
};

#endif
