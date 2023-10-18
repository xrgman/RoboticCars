#ifndef RESPEAKER6MIC_H
#define RESPEAKER6MIC_H

#include "mbed.h"
#include "edevice.h"
#include "I2S.h"
#include "ac101.h"
#include "ac108.h"
#include "communication.h"

class Respeaker6MicArray : IEDevice {
    public:
        Respeaker6MicArray(PinName buttonPin, I2C *i2c, Communication *comm);

        void initialize();

        bool checkDeviceOperation(Communication *communication_protocol);
        void setOnButtonClickListener(void (*onButtonClick)());

        void loop();

    private:
        void (*onButtonClick)();
        InterruptIn button;

        I2S i2s;
        AC101 ac101;
        AC108 ac108_1, ac108_2;

        Communication *communication;

        void onButtonInterruptRise();
        void onButtonInterruptFall();

        Timer debounceTimer;

};

#endif