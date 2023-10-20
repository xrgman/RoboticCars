#include "localizationControl.h"
#include "main.h"
#include "enums.h"
#include "I2S.h"
#include "pinDefinitions.h"

void onI2SWriteDone()
{
    sdWrapper.signalBatchDone();
}

void configure(SampleRate sampleRate, WordSize wordSize, uint8_t channels)
{
    respeaker.configureSpeaker(sampleRate, wordSize); //TODO channels


    //SErial clock (Mhz) should be: samplerate (Hz) * wordsize * channels
    //In our case:  22050*16*1  = 352.8 kHz
    //Or            22050*16*2  = 705.6 kHz


}

void initializeLocalization()
{
    respeaker.setOnI2SWriteDoneListener(&onI2SWriteDone);

    respeaker.setVolumeSpeaker(45);

    //sdWrapper.playWavFile("/sd/test2.wav", &configure, callback(&respeaker, &Respeaker6MicArray::writeSpeaker16));
}

void runLocalization()
{
}
