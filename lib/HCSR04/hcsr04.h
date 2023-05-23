#ifndef HCSR04_H
#define HCSR04_H

#include "mbed.h"
#include "communication.h"

class HCSR04 {
    public:
        HCSR04(PinName triggerPin, PinName echoPin);
        HCSR04(PinName triggerPin, PinName echoPin, Communication *communication_protocol);
        void startMeasurement();
        bool isNewDataReady();
        int getDistanceCm();
        float getDistanceCmBlocking();
        int getDistanceInch();

    private:
        DigitalOut trigger;
        InterruptIn echo;
        Timer timer;
        Timeout triggerTimeOut, echoTimeOut;
        bool timerStarted, newDataReady, useCommunication;
        int pulseDuration, distance;
        Communication *communication;

        void initialize();
        void disableTrigger();
        void onEchoRise();
        void onEchoFall();
};

#endif
