#ifndef SDWRAPPER_H
#define SDWRAPPER_H

#include "mbed.h"
#include "enums.h"
#include "communication.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

#define DATA_WRITE_SIZE 3 //2 for 16 bit, 3 for 24 bit, etc.


// WAV header structure
#pragma pack(push, 1) // Ensure correct packing of the struct
struct WAVHeader
{
    // RIFF Chunk
    char chunkID[4];    // "RIFF" chunk descriptor
    uint32_t chunkSize; // File size excluding ChunkID and ChunkSize
    char format[4];     // "WAVE" format descriptor

    // Format Subchunk
    char subchunk1ID[4];    // "fmt " subchunk
    uint32_t subchunk1Size; // Size of the format data (16 bytes for PCM)
    uint16_t audioFormat;   // Audio format (1 for PCM)
    uint16_t numChannels;   // Number of channels (1 for mono, 2 for stereo, etc.)
    uint32_t sampleRate;    // Sample rate (e.g., 44100 Hz)
    uint32_t byteRate;      // Byte rate (SampleRate * NumChannels * BitsPerSample / 8)
    uint16_t blockAlign;    // Block align (NumChannels * BitsPerSample / 8)
    uint16_t bitsPerSample; // Bits per sample (e.g., 16 bits)

    // Data Subchunk
    char subchunk2ID[4];    // "data" subchunk
    uint32_t subchunk2Size; // Subchunk2Size (Size of the data)
};
#pragma pack(pop) // Restore default packing

class SDWrapper
{
public:
    SDWrapper(PinName mosi, PinName miso, PinName clk, PinName nss, Communication *comm);

    void initialize();
    bool playWavFile(const char *filename, Callback<void(SampleRate sampleRate, WordSize wordSize, uint8_t channels)> configureCallback, Callback<void(uint16_t *data, uint16_t size)> writeCallback);
    bool recordToWavFile(const char *filename, uint8_t secondsToWrite, Callback<uint16_t*()> getReadBufferCallback);

    void signalWriteDone();
    void signalReadDone();

    void run();

private:
    Communication *communication_protocol;
    SDBlockDevice sd;
    FATFileSystem fs;

    Callback<void(uint16_t *data, uint16_t size)> writeCallback;
    Callback<uint16_t *()> getReadBufferCallback;

    FILE *fileRead;
    FILE *fileWrite;

    bool fileReadStarted = false;
    bool fileWriteStarted = false;
    bool writeDone = true;
    bool readDone = true;
    int samples = 0;

    uint8_t secondsToWrite;
    uint32_t bytesWritten;

    uint32_t sampleCount = 44100;

    bool mountFileSystem();
    bool openFile(const char *filename, FILE **file, const char *mode);

    void finishWavFileRecording();

    WAVHeader createWavHeaderFromFile(const uint8_t header[44]);
    WAVHeader createWavHeader(uint32_t sampleRate, uint16_t bitsPerSample, uint16_t numChannels);
    void printWAVHeader(WAVHeader WAVHeader);
};

#endif