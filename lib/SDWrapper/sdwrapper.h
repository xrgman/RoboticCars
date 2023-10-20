#ifndef SDWRAPPER_H
#define SDWRAPPER_H

#include "mbed.h"
#include "enums.h"
#include "communication.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"



class SDWrapper {
    public:
        SDWrapper(PinName mosi, PinName miso, PinName clk, PinName nss, Communication *comm);

        void initialize();
        bool playWavFile(const char *filename, Callback<void(SampleRate sampleRate, WordSize wordSize, uint8_t channels)> configureCallback, Callback<void(uint16_t *data, uint16_t size)> writeCallback);
        void signalBatchDone();

        void run();

    private:
        Communication *communication_protocol;
        SDBlockDevice sd;
        FATFileSystem fs;

        Callback<void(uint16_t *data, uint16_t size)> writeCallback;

        FILE *file;
        bool fileStarted = false;
        bool playing = false;

        bool mountFileSystem();
        bool openFile(const char *filename);
        void printWAVHeader(const uint8_t header[44]);
};

#endif