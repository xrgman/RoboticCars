// #ifndef AC101_H
// #define AC101_H

// #include "mbed.h"
// #include "edevice.h"
// #include "enums.h"

// #define _FREQ_24_576K 24576000
// #define _FREQ_22_579K 22579200

// class AC101 : IEDevice
// {
// public:
//     typedef enum
//     {
//         SAMPLE_RATE_8000 = 0x0000,
//         SAMPLE_RATE_11052 = 0x1000,
//         SAMPLE_RATE_12000 = 0x2000,
//         SAMPLE_RATE_16000 = 0x3000,
//         SAMPLE_RATE_22050 = 0x4000,
//         SAMPLE_RATE_24000 = 0x5000,
//         SAMPLE_RATE_32000 = 0x6000,
//         SAMPLE_RATE_44100 = 0x7000,
//         SAMPLE_RATE_48000 = 0x8000,
//         SAMPLE_RATE_96000 = 0x9000,
//         SAMPLE_RATE_192000 = 0xa000
//     } I2sSampleRate_t;

//     typedef enum
//     {
//         MODE_MASTER = 0x00,
//         MODE_SLAVE = 0x01
//     } I2sMode_t;

//     typedef enum
//     {
//         WORD_SIZE_8_BITS = 0x00,
//         WORD_SIZE_16_BITS = 0x01,
//         WORD_SIZE_20_BITS = 0x02,
//         WORD_SIZE_24_BITS = 0x03
//     } I2sWordSize_t;

//     typedef enum
//     {
//         DATA_FORMAT_I2S = 0x00,
//         DATA_FORMAT_LEFT = 0x01,
//         DATA_FORMAT_RIGHT = 0x02,
//         DATA_FORMAT_DSP = 0x03
//     } I2sFormat_t;

//     typedef enum
//     {
//         BCLK_DIV_1 = 0x0,
//         BCLK_DIV_2 = 0x1,
//         BCLK_DIV_4 = 0x2,
//         BCLK_DIV_6 = 0x3,
//         BCLK_DIV_8 = 0x4,
//         BCLK_DIV_12 = 0x5,
//         BCLK_DIV_16 = 0x6,
//         BCLK_DIV_24 = 0x7,
//         BCLK_DIV_32 = 0x8,
//         BCLK_DIV_48 = 0x9,
//         BCLK_DIV_64 = 0xa,
//         BCLK_DIV_96 = 0xb,
//         BCLK_DIV_128 = 0xc,
//         BCLK_DIV_192 = 0xd
//     } I2sBitClockDiv_t;

//     typedef enum
//     {
//         LRCK_DIV_16 = 0x0,
//         LRCK_DIV_32 = 0x1,
//         LRCK_DIV_64 = 0x2,
//         LRCK_DIV_128 = 0x3,
//         LRCK_DIV_256 = 0x4
//     } I2sLrClockDiv_t;

//     typedef enum
//     {
//         MODE_ADC,
//         MODE_DAC,
//         MODE_ADC_DAC,
//         MODE_LINE
//     } Mode_t;

//     // Constructor.
//     AC101(I2C *i2c, uint8_t device_address, PinName ampEnablePin);

//     // Check the operation and connection to the AC101 chip:
//     bool checkDeviceOperation(Communication *communication_protocol);

//     // Initialize codec, using provided I2C pins and bus frequency.
//     // @return True on success, false on failure.
//     bool initialize(SampleRate sampleRate, WordSize WordSize, NumChannels numChannels);

//     // Get speaker volume.
//     // @return Speaker volume, [63..0] for [0..-43.5] [dB], in increments of 2.
//     uint8_t getVolumeSpeaker();

//     // Set speaker volume.
//     // @param volume   Target volume, [63..0] for [0..-43.5] [dB], in increments of 2.
//     // @return True on success, false on failure.
//     bool setVolumeSpeaker(uint8_t volume);

//     // Get headphone volume.
//     // @return Headphone volume, [63..0] for [0..-62] [dB]
//     uint8_t getVolumeHeadphone();

//     // Set headphone volume
//     // @param volume   Target volume, [63..0] for [0..-62] [dB]
//     // @return True on success, false on failure.
//     bool setVolumeHeadphone(uint8_t volume);

//     // Configure I2S samplerate.
//     // @param rate   Samplerate.
//     // @return True on success, false on failure.
//     bool setI2sSampleRate(I2sSampleRate_t rate);

//     // Configure I2S mode (master/slave).
//     // @param mode   Mode.
//     // @return True on success, false on failure.
//     bool setI2sMode(I2sMode_t mode);

//     // Configure I2S word size (8/16/20/24 bits).
//     // @param size   Word size.
//     // @return True on success, false on failure.
//     bool setI2sWordSize(I2sWordSize_t size);

//     // Configure I2S format (I2S/Left/Right/Dsp).
//     // @param format   I2S format.
//     // @return True on success, false on failure.
//     bool setI2sFormat(I2sFormat_t format);

//     // Configure I2S clock.
//     // @param bitClockDiv   I2S1CLK/BCLK1 ratio.
//     // @param bitClockInv   I2S1 BCLK Polarity.
//     // @param lrClockDiv    BCLK1/LRCK ratio.
//     // @param lrClockInv    I2S1 LRCK Polarity.
//     // @return True on success, false on failure.
//     bool setI2sClock(I2sBitClockDiv_t bitClockDiv, bool bitClockInv, I2sLrClockDiv_t lrClockDiv, bool lrClockInv);

//     // Configure the mode (Adc/Dac/Adc+Dac/Line)
//     // @param mode    Operating mode.
//     // @return True on success, false on failure.
//     bool setMode(Mode_t mode);

//     // Dumpt the current register configuration to serial.
//     void printRegisters();

// protected:
//     I2C *i2c;
//     DigitalOut enableAmp;

//     uint8_t device_address;
//     bool initialized;

//     bool switchHwConfig();
//     bool set_dai_fmt();
//     bool hw_params();
//     bool set_pll(bool master, unsigned int freq_in, unsigned int freq_out);
//     bool aif1clk(bool enable, int quick);
//     bool trigger(bool start);

//     unsigned int sysclk = 24000000;
//     uint8_t aif1_clken = 0;

//     SampleRate sampleRate;
//     WordSize wordSize;
//     NumChannels numChannels;

//     uint16_t readByte(uint8_t address);
//     void readBytes(uint8_t address, uint8_t count, uint16_t *dest);
//     bool writeByte(uint8_t address, uint16_t data);
// };

// #endif
