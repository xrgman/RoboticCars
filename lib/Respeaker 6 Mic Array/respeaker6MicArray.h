#ifndef RESPEAKER6MIC_H
#define RESPEAKER6MIC_H

#include "mbed.h"
#include "communication.h"

class Respeaker6MicArray {
    public:
        Respeaker6MicArray(PinName buttonPin, Communication *comm);

        void setOnButtonClickListener(void (*onButtonClick)());

    private:
        void (*onButtonClick)();
        InterruptIn button;

        Communication *communication;

        void onButtonInterruptRise();
        void onButtonInterruptFall();

        Timer debounceTimer;
};

#endif