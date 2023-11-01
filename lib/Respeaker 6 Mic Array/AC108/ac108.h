#ifndef AC108_H
#define AC108_H

#include "mbed.h"
#include "edevice.h"
#include "enums.h"

#define _FREQ_24_576K 24576000
#define _FREQ_22_579K 22579200

struct pll_div
{
    unsigned int freq_in;
    unsigned int freq_out;
    unsigned int m1;
    unsigned int m2;
    unsigned int n;
    unsigned int k1;
    unsigned int k2;
};

static const struct pll_div ac108_pll_div_list[] = {
    {400000, _FREQ_24_576K, 0, 0, 614, 4, 1},
    {512000, _FREQ_24_576K, 0, 0, 960, 9, 1}, //_FREQ_24_576K/48
    {768000, _FREQ_24_576K, 0, 0, 640, 9, 1}, //_FREQ_24_576K/32
    {800000, _FREQ_24_576K, 0, 0, 614, 9, 1},
    {1024000, _FREQ_24_576K, 0, 0, 480, 9, 1}, //_FREQ_24_576K/24
    {1600000, _FREQ_24_576K, 0, 0, 307, 9, 1},
    {2048000, _FREQ_24_576K, 0, 0, 240, 9, 1}, /* accurate,  8000 * 256 */
    {3072000, _FREQ_24_576K, 0, 0, 160, 9, 1}, /* accurate, 12000 * 256 */
    {4096000, _FREQ_24_576K, 2, 0, 360, 9, 1}, /* accurate, 16000 * 256 */
    {6000000, _FREQ_24_576K, 4, 0, 410, 9, 1},
    {12000000, _FREQ_24_576K, 9, 0, 410, 9, 1},
    {13000000, _FREQ_24_576K, 8, 0, 340, 9, 1},
    {15360000, _FREQ_24_576K, 12, 0, 415, 9, 1},
    {16000000, _FREQ_24_576K, 12, 0, 400, 9, 1},
    {19200000, _FREQ_24_576K, 15, 0, 410, 9, 1},
    {19680000, _FREQ_24_576K, 15, 0, 400, 9, 1},
    {24000000, _FREQ_24_576K, 4, 0, 128, 24, 0}, // accurate, 24M -> 24.576M */

    {400000, _FREQ_22_579K, 0, 0, 566, 4, 1},
    {512000, _FREQ_22_579K, 0, 0, 880, 9, 1},
    {768000, _FREQ_22_579K, 0, 0, 587, 9, 1},
    {800000, _FREQ_22_579K, 0, 0, 567, 9, 1},
    {1024000, _FREQ_22_579K, 0, 0, 440, 9, 1},
    {1600000, _FREQ_22_579K, 1, 0, 567, 9, 1},
    {2048000, _FREQ_22_579K, 0, 0, 220, 9, 1},
    {3072000, _FREQ_22_579K, 0, 0, 148, 9, 1},
    {4096000, _FREQ_22_579K, 2, 0, 330, 9, 1},
    {6000000, _FREQ_22_579K, 2, 0, 227, 9, 1},
    {12000000, _FREQ_22_579K, 8, 0, 340, 9, 1},
    {13000000, _FREQ_22_579K, 9, 0, 350, 9, 1},
    {15360000, _FREQ_22_579K, 10, 0, 325, 9, 1},
    {16000000, _FREQ_22_579K, 11, 0, 340, 9, 1},
    {19200000, _FREQ_22_579K, 13, 0, 330, 9, 1},
    {19680000, _FREQ_22_579K, 14, 0, 345, 9, 1},
    {24000000, _FREQ_22_579K, 24, 0, 588, 24, 0}, // accurate, 24M -> 22.5792M */

    {_FREQ_24_576K / 1, _FREQ_24_576K, 9, 0, 200, 9, 1},   //_FREQ_24_576K
    {_FREQ_24_576K / 2, _FREQ_24_576K, 9, 0, 400, 9, 1},   /*12288000,accurate, 48000 * 256 */
    {_FREQ_24_576K / 4, _FREQ_24_576K, 4, 0, 400, 9, 1},   /*6144000, accurate, 24000 * 256 */
    {_FREQ_24_576K / 16, _FREQ_24_576K, 0, 0, 320, 9, 1},  // 1536000
    {_FREQ_24_576K / 64, _FREQ_24_576K, 0, 0, 640, 4, 1},  // 384000
    {_FREQ_24_576K / 96, _FREQ_24_576K, 0, 0, 960, 4, 1},  // 256000
    {_FREQ_24_576K / 128, _FREQ_24_576K, 0, 0, 512, 1, 1}, // 192000
    {_FREQ_24_576K / 176, _FREQ_24_576K, 0, 0, 880, 4, 0}, // 140000
    {_FREQ_24_576K / 192, _FREQ_24_576K, 0, 0, 960, 4, 0}, // 128000

    {_FREQ_22_579K / 1, _FREQ_22_579K, 9, 0, 200, 9, 1},   //_FREQ_22_579K
    {_FREQ_22_579K / 2, _FREQ_22_579K, 9, 0, 400, 9, 1},   /*11289600,accurate, 44100 * 256 */
    {_FREQ_22_579K / 4, _FREQ_22_579K, 4, 0, 400, 9, 1},   /*5644800, accurate, 22050 * 256 */
    {_FREQ_22_579K / 16, _FREQ_22_579K, 0, 0, 320, 9, 1},  // 1411200
    {_FREQ_22_579K / 64, _FREQ_22_579K, 0, 0, 640, 4, 1},  // 352800
    {_FREQ_22_579K / 96, _FREQ_22_579K, 0, 0, 960, 4, 1},  // 235200
    {_FREQ_22_579K / 128, _FREQ_22_579K, 0, 0, 512, 1, 1}, // 176400
    {_FREQ_22_579K / 176, _FREQ_22_579K, 0, 0, 880, 4, 0}, // 128290
    {_FREQ_22_579K / 192, _FREQ_22_579K, 0, 0, 960, 4, 0}, // 117600

    {_FREQ_22_579K / 6, _FREQ_22_579K, 2, 0, 360, 9, 1},  // 3763200
    {_FREQ_22_579K / 8, _FREQ_22_579K, 0, 0, 160, 9, 1},  /*2822400, accurate, 11025 * 256 */
    {_FREQ_22_579K / 12, _FREQ_22_579K, 0, 0, 240, 9, 1}, // 1881600
    {_FREQ_22_579K / 24, _FREQ_22_579K, 0, 0, 480, 9, 1}, // 940800
    {_FREQ_22_579K / 32, _FREQ_22_579K, 0, 0, 640, 9, 1}, // 705600
    {_FREQ_22_579K / 48, _FREQ_22_579K, 0, 0, 960, 9, 1}, // 470400
};

static const unsigned ac108_bclkdivs[] = {
	 0,   1,   2,   4,
	 6,   8,  12,  16,
	24,  32,  48,  64,
	96, 128, 176, 192,
};

class AC108 : IEDevice
{
public:
    AC108(I2C *i2c, uint8_t device_address, uint8_t chipNumber);

    bool checkDeviceOperation(Communication *communication_protocol);
    bool initialize(SampleRate sampleRate, WordSize wordSize, NumChannels numChannels);
    bool captureStarted();
    bool setVolume(uint8_t vol);
    bool startCapture44();
    bool stopCapture();

private:
    I2C *i2c;
    uint8_t device_address, chipNumber;

    SampleRate sampleRate;
    WordSize wordSize;
    NumChannels numChannels;

    unsigned int mclk;

    bool initializePll();
    bool initializeFmt();
    bool initializeChannels();

    uint8_t getSampleRateRegValue(SampleRate sampleRate);
    uint8_t getSampleResolutionRegValue(WordSize wordSize);

    char readByte(uint8_t address);
    void readBytes(uint8_t address, uint8_t count, uint8_t *dest);
    bool writeByte(uint8_t address, uint8_t data);
};

#endif