#include "localizationControl.h"
#include "main.h"
#include "enums.h"
#include "I2S.h"
#include "pinDefinitions.h"

void onI2SWriteDone()
{
    sdWrapper.signalWriteDone();
}

void onI2SReadDone()
{
    sdWrapper.signalReadDone();
}

void configure(SampleRate sampleRate, WordSize wordSize, uint8_t channels)
{
    bool t = respeaker.configureSpeaker(sampleRate, wordSize); //TODO channels

    if(!t) {
        printf("Failed to configure!\n");
    }
    else {
        printf("BANANANA\n");
    }

    //SErial clock (Mhz) should be: samplerate (Hz) * wordsize * channels
    //In our case:  22050*16*1  = 352.8 kHz
    //Or            22050*16*2  = 705.6 kHz


}

void initializeLocalization()
{
    respeaker.setOnI2SWriteDoneListener(&onI2SWriteDone);
    respeaker.setOnI2SReadDoneListener(&onI2SReadDone);

    respeaker.setVolumeSpeaker(45);
    

    sdWrapper.recordToWavFile("sd/record.wav", 5, callback(&respeaker, &Respeaker6MicArray::getPointerToReadBuffer));



    //Change 32 -> 16 bit block A!
   //sdWrapper.playWavFile("/sd/record.wav", &configure, callback(&respeaker, &Respeaker6MicArray::writeSpeaker16));
// respeaker.test();
    //HAL_Delay(1000);
    //respeaker.test();
}

void runLocalization()
{
}
