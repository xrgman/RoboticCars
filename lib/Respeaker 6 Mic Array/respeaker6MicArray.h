#ifndef RESPEAKER6MIC_H
#define RESPEAKER6MIC_H

#include "mbed.h"
#include "edevice.h"
#include "enums.h"
#include "I2S.h"
#include "ac101.h"
#include "ac108.h"
#include "communication.h"

class Respeaker6MicArray : IEDevice
{
public:
    Respeaker6MicArray(PinName buttonPin, I2C *i2c, Communication *comm);

    void initialize();

    bool checkDeviceOperation(Communication *communication_protocol);
    void setOnButtonClickListener(void (*onButtonClick)());
    void setOnI2SWriteDoneListener(Callback<void()> onI2SWriteDone);
    void setOnI2SReadDoneListener(Callback<void()> onI2SReadDone);

    bool setVolumeSpeaker(uint8_t volume);
    bool setVolumeMicrophones(uint8_t volume);

    bool configureSpeaker(SampleRate sampleRate, WordSize wordSize);
    
    void writeSpeaker8(uint8_t *data, uint16_t size);
    void writeSpeaker16(uint16_t *data, uint16_t size);
    void writeSpeaker32(uint32_t *data, uint16_t size);

    uint32_t *getPointerToReadBuffer();

    void run();
    void test();

private:
    void (*onButtonClick)();
    Callback<void()> onI2SWriteDone;
    Callback<void()> onI2SReadDone;
    InterruptIn button;

    I2S i2s;
    AC101 ac101;
    AC108 ac108_1, ac108_2;

    Communication *communication;

    void onButtonInterruptRise();
    void onI2STxCpltCallback();
    void onI2SRxCpltCallback();

    //Fields used for playback of larger files:


    Timer debounceTimer;
};

#endif