#include "ac108.h"
#include "ac108registers.h"
#include "util.h"

AC108::AC108(I2C *i2c, uint8_t device_address, uint8_t chipNumber)
{
    this->i2c = i2c;
    this->device_address = device_address;
    this->chipNumber = chipNumber;
}

bool AC108::checkDeviceOperation(Communication *communication_protocol)
{
    uint8_t response = readByte(0b00000000);

    if (response == 0x4A)
    { // 74
        communication_protocol->sendDebugMessage("SUCCESS: AC108 found and functioning properly.\r\n");

        return true;
    }

    communication_protocol->sendDebugMessage("ERROR: AC108 not found or not functioning properly.\r\n");

    return false;
}

bool AC108::initialize(SampleRate sampleRate, WordSize wordSize, NumChannels numChannels)
{
    this->sampleRate = sampleRate;
    this->wordSize = wordSize;
    this->numChannels = numChannels;

    uint8_t sampleRateRegVal = getSampleRateRegValue(sampleRate);
    uint8_t sampleResRegVal = getSampleResolutionRegValue(wordSize);

    bool ok = true;

    // Reset all registers, readback default as sanity check
    writeByte(CHIP_RST, 0x12); // Results in error, but does reset the chip :)
    HAL_Delay(200);
    ok &= CHIP_RST_VAL == readByte(CHIP_RST);

    // Step 1: set clock
    ok &= writeByte(SYSCLK_CTRL, 0b00001000); // sys clock 24000000 Hz

    // Step 2: set fmt
    ok &= writeByte(PWR_CTRL6, 0b00000001); // Power up ac108 - Enable Analog LDO
    ok &= writeByte(PWR_CTRL7, 0b10011001); // Power up ac108 - REF faststart disable, enable Enable VREF
    ok &= writeByte(PWR_CTRL9, 0b00001001); // Power up ac108 - Enable VREFP (needed by all audio input channels)

    ok &= initializeFmt();

    // Step 3: Hw config
    //  Do something with clock bit, but not sure for now...
    uint8_t lrck_ctrl1 = readByte(I2S_LRCK_CTRL1);
    uint8_t i2s_fmt_ctrl2 = readByte(I2S_FMT_CTRL2);
    uint8_t div = (wordSize * numChannels) - 1;

    Util::setBits(&lrck_ctrl1, 0, 2, ((div >> 8) << 0));

    Util::setBits(&i2s_fmt_ctrl2, SAMPLE_RESOLUTION, 7, sampleResRegVal);
    Util::setBits(&i2s_fmt_ctrl2, SLOT_WIDTH_SEL, 7, sampleResRegVal);

    ok &= writeByte(I2S_LRCK_CTRL1, lrck_ctrl1);
    ok &= writeByte(I2S_LRCK_CTRL2, (div & 0xFF));
    ok &= writeByte(I2S_FMT_CTRL2, i2s_fmt_ctrl2);
    ok &= writeByte(ADC_SPRC, sampleRateRegVal); // Set the sample rate
    ok &= writeByte(HPF_EN, 0b00001111);         // enable high pass filter

    // Step 4: Configure pll
    ok &= initializePll();

    uint8_t bclk_ctrl = readByte(I2S_BCLK_CTRL);

    uint32_t bclkdiv = mclk / (sampleRate * numChannels * wordSize);
    uint8_t i = 0;

    for (i = 0; i < 16; i++)
    {
        if (ac108_bclkdivs[i] >= bclkdiv)
        {
            break;
        }
    }

    Util::setBits(&bclk_ctrl, BCLKDIV, 15, i);

    ok &= writeByte(I2S_BCLK_CTRL, bclk_ctrl);

    // Step 5: Configure channels:
    ok &= initializeChannels();

    // Step 6: Enable recording

    /*0x21: Module clock enable<I2S, ADC digital, MIC offset Calibration, ADC analog>*/
    ok &= writeByte(MOD_CLK_EN, 0b10010011); // mod clk en
    /*0x22: Module reset de-asserted<I2S, ADC digital, MIC offset Calibration, ADC analog>*/
    ok &= writeByte(MOD_RST_CTRL, 0b10010011); // mod rst ctrl

    // Step 7: Something with the clock
    uint8_t i2s_ctrl = readByte(I2S_CTRL);
    uint8_t pll_ctrl1 = readByte(PLL_CTRL1);

    /* enable lrck clock */
    if(i2s_ctrl & (0x01 << BCLK_IOEN)) {
        Util::setBits(&i2s_ctrl, LRCK_IOEN, 2, 0b11);
    }

    /*0x10: PLL Common voltage enable, PLL enable */
    Util::setBits(&pll_ctrl1, PLL_EN, 1, 1);
    Util::setBits(&pll_ctrl1, PLL_COM_EN, 1, 1);

    /* enable global clock */
    Util::setBits(&i2s_ctrl, TXEN, 1, 1);
    Util::setBits(&i2s_ctrl, GEN, 1, 1);

    ok &= writeByte(I2S_CTRL, i2s_ctrl);
    ok &= writeByte(I2S_CTRL, pll_ctrl1);


    ok &= writeByte(ADC_DIG_EN, 0b00001111); // enable adc

    ok &= writeByte(ANA_ADC1_CTRL1, 0b00000001); // disable mic bias 1
    ok &= writeByte(ANA_ADC2_CTRL1, 0b00000001); // disable mic bias 2
    ok &= writeByte(ANA_ADC3_CTRL1, 0b00000001); // disable mic bias 3
    ok &= writeByte(ANA_ADC4_CTRL1, 0b00000001); // disable mic bias 4

    ok &= writeByte(ANA_ADC1_CTRL1, 0b00000101); // disable dsm 1
    ok &= writeByte(ANA_ADC2_CTRL1, 0b00000101); // disable dsm 2
    ok &= writeByte(ANA_ADC3_CTRL1, 0b00000101); // disable dsm 3
    ok &= writeByte(ANA_ADC4_CTRL1, 0b00000101); // disable dsm 4

    ok &= writeByte(ADC_DIG_EN, 0b00011111); // disable adc

    ok &= writeByte(ANA_ADC1_CTRL1, 0b00000111); // enable PGA 1
    ok &= writeByte(ANA_ADC2_CTRL1, 0b00000111); // enable PGA 2
    ok &= writeByte(ANA_ADC3_CTRL1, 0b00000111); // enable PGA 3
    ok &= writeByte(ANA_ADC4_CTRL1, 0b00000111); // enable PGA 4

    ok &= writeByte(0xba, 0b00100000); // disable dsm globally
    ok &= writeByte(0xbb, 0b00001111); // disable adc clk gating globally

    // ok &= writeByte(I2S_CTRL, 0b00110101); // i2s format
    // ok &= writeByte(I2S_CTRL, 0b10110101); // i2s format

    // // Setting volume:
    ok &= setVolume(0xde);

    return ok;
}

bool AC108::initializePll()
{
    bool ok = true;

    uint32_t lrck_ratio = wordSize * numChannels;
    uint32_t pll_freq_in = sampleRate * lrck_ratio;

    pll_div pllDiv = {0};

    for (int i = 0; i < 58; i++)
    {
        if (ac108_pll_div_list[i].freq_in == pll_freq_in && ac108_pll_div_list[i].freq_out % sampleRate == 0)
        {
            pllDiv = ac108_pll_div_list[i];

            break;
        }
    }

    uint8_t pll_ctrl5, pll_ctrl4, pll_ctrl3, pll_ctrl2 = 0;

    Util::setBits(&pll_ctrl5, PLL_POSTDIV1, 5, pllDiv.k1);          // Set K1
    Util::setBits(&pll_ctrl5, PLL_POSTDIV2, 1, pllDiv.k2);          // Set K2
    Util::setBits(&pll_ctrl4, PLL_LOOPDIV_LSB, 8, pllDiv.n);        // Set n LSB
    Util::setBits(&pll_ctrl3, PLL_LOOPDIV_MSB, 2, (pllDiv.n >> 8)); // Set n MSB
    Util::setBits(&pll_ctrl2, PLL_PREDIV1, 5, pllDiv.m1);           // Set m1 and m2
    Util::setBits(&pll_ctrl2, PLL_PREDIV2, 1, pllDiv.m2);           // Set m1 and m2

    ok &= writeByte(PLL_CTRL5, pll_ctrl5);      // pll divider config
    ok &= writeByte(PLL_CTRL4, pll_ctrl4);      // pll loop divider
    ok &= writeByte(PLL_CTRL3, pll_ctrl3);      // pll loop divider MSB
    ok &= writeByte(PLL_CTRL2, pll_ctrl2);      // pll pre divider
    ok &= writeByte(PLL_LOCK_CTRL, 0b00000001); // pll clock lock en
    ok &= writeByte(SYSCLK_CTRL, 0b10011001);   // pll clock source PLL -> Corrected! warning used twice!

    mclk = pllDiv.freq_out;

    return true;
}

bool AC108::initializeFmt()
{
    bool ok = true;

    uint8_t i2s_ctrl = readByte(I2S_CTRL);
    uint8_t bclk_ctrl = readByte(I2S_BCLK_CTRL);
    uint8_t lrck_ctrl = readByte(I2S_LRCK_CTRL1);

    Util::setBits(&i2s_ctrl, LRCK_IOEN, 2, 0);  // BCLK & LRCK input
    Util::setBits(&i2s_ctrl, SDO1_EN, 2, 0b11); // Enable SDO1_EN and SD02_EN
    Util::setBits(&i2s_ctrl, TXEN, 1, 0);       // Transmitter block enable??
    Util::setBits(&i2s_ctrl, GEN, 1, 0);        // Globe Enable

    Util::setBits(&bclk_ctrl, BCLK_POLARITY, 1, 0); // BCLK polarity
    Util::setBits(&lrck_ctrl, LRCK_POLARITY, 1, 1); // LRCK polarity

    ok &= writeByte(I2S_CTRL, i2s_ctrl);
    ok &= writeByte(I2S_BCLK_CTRL, bclk_ctrl);
    ok &= writeByte(I2S_LRCK_CTRL1, lrck_ctrl);
    // ok &= writeByte(I2S_FMT_CTRL1, 0b00001101); // PCM_FORMAT (I2S: 0b00011101)
    ok &= writeByte(I2S_FMT_CTRL1, 0b00011101); // I2S format
    ok &= writeByte(I2S_FMT_CTRL3, 0b01100000); // MSB first (was 0x00)
    ok &= writeByte(HPF_EN, 0b00000000);        // disable high pass filter

    return ok;
}

bool AC108::initializeChannels()
{
    bool ok = true;

    const unsigned vec_mask[] = {
        0x3 << 6 | 0x3, // slots 6,7,0,1
        0xF << 2        // slots 2,3,4,5
    };

    const unsigned vec_maps[] = {/*
                                  * chip 0,
                                  * mic 0 sample -> slot 6
                                  * mic 1 sample -> slot 7
                                  * mic 2 sample -> slot 0
                                  * mic 3 sample -> slot 1
                                  */
                                 0x0 << 12 | 0x1 << 14 | 0x2 << 0 | 0x3 << 2,
                                 /*
                                  * chip 1,
                                  * mic 0 sample -> slot 2
                                  * mic 1 sample -> slot 3
                                  * mic 2 sample -> slot 4
                                  * mic 3 sample -> slot 5
                                  */
                                 0x0 << 4 | 0x1 << 6 | 0x2 << 8 | 0x3 << 10};

    ok &= writeByte(I2S_TX1_CTRL1, numChannels - 1);                    // set channels
    ok &= writeByte(I2S_TX1_CTRL2, (vec_mask[chipNumber] >> 0) & 0xFF); // tx config
    ok &= writeByte(I2S_TX1_CTRL3, (vec_mask[chipNumber] >> 8) & 0xFF); // tx config

    ok &= writeByte(I2S_TX1_CHMP_CTRL1, (vec_maps[chipNumber] >> 0) & 0xFF);  // tx chip map
    ok &= writeByte(I2S_TX1_CHMP_CTRL2, (vec_maps[chipNumber] >> 8) & 0xFF);  // tx chip map
    ok &= writeByte(I2S_TX1_CHMP_CTRL3, (vec_maps[chipNumber] >> 16) & 0xFF); // tx chip map
    ok &= writeByte(I2S_TX1_CHMP_CTRL4, (vec_maps[chipNumber] >> 24) & 0xFF); // tx chip map

    return ok;
}

uint8_t AC108::getSampleRateRegValue(SampleRate sampleRate)
{
    switch (sampleRate)
    {
    case SAMPLE_RATE_8000:
        return 0;
    case SAMPLE_RATE_11052:
        return 1;
    case SAMPLE_RATE_16000:
        return 3;
    case SAMPLE_RATE_22050:
        return 4;
    case SAMPLE_RATE_32000:
        return 6;
    case SAMPLE_RATE_44100:
        return 7;
    case SAMPLE_RATE_48000:
        return 8;
    case SAMPLE_RATE_96000:
        return 9;
    }
}

uint8_t AC108::getSampleResolutionRegValue(WordSize wordSize)
{
    switch (wordSize)
    {
    case WORD_SIZE_8_BITS:
        return 1;
    case WORD_SIZE_16_BITS:
        return 3;
    case WORD_SIZE_20_BITS:
        return 4;
    case WORD_SIZE_24_BITS:
        return 5;
    case WORD_SIZE_32_BITS:
        return 7;
    }
}

/// @brief Check whether capture has started.
/// @return Whether or not capturing is in progress.
bool AC108::captureStarted()
{
    return readByte(I2S_CTRL) == 0b11110101;
}

bool AC108::setVolume(uint8_t vol)
{
    bool ok = true;

    ok &= writeByte(ADC1_DVOL_CTRL, vol); // volume mic 1
    ok &= writeByte(ADC2_DVOL_CTRL, vol); // volume mic 2
    ok &= writeByte(ADC3_DVOL_CTRL, vol); // volume mic 3
    ok &= writeByte(ADC4_DVOL_CTRL, vol); // volume mic 4

    return ok;
}

bool AC108::startCapture44()
{
    bool ok = true;

    ok &= writeByte(I2S_LRCK_CTRL2, 0b00111111); // sample resolution 16 bit

    // ok &= writeByte(I2S_CTRL, 0b10110000);  // disable all clocks
    // ok &= writeByte(I2S_CTRL, 0b11110000);  // enable lrck clock
    // ok &= writeByte(PLL_CTRL1, 0b01001011); // enable pll
    // ok &= writeByte(I2S_CTRL, 0b11110101);  // enable global clock

    return ok;
}

bool AC108::stopCapture()
{
    bool ok = true;

    ok &= writeByte(I2S_CTRL, 0b11110000);  // disable global clock
    ok &= writeByte(PLL_CTRL1, 0b01001000); // disable pll
    ok &= writeByte(I2S_CTRL, 0b10110000);  // disable all clocks

    ok &= writeByte(MOD_CLK_EN, 0b00000000);   // disable mod clk
    ok &= writeByte(MOD_RST_CTRL, 0b00000000); // disable mod rst

    return ok;
}

//************************************
//******** I2C helper functions ******
//************************************

/// @brief Read one byte from the requested I2C register.
/// @param address Address of the register.
/// @return Data currently in the register.
char AC108::readByte(uint8_t address)
{
    char data[1];
    char data_write[1];
    data_write[0] = address;

    i2c->write(device_address, data_write, 1, true);

    i2c->read(device_address, data, 1, false);

    return data[0];
}

/// @brief Read multiple bytes from a specific I2C register.
/// @param address Address of the register.
/// @param count Number of bytes to read
/// @param dest Array to store resulting data in.
void AC108::readBytes(uint8_t address, uint8_t count, uint8_t *dest)
{
    char data[14];
    char data_write[1];
    data_write[0] = address;

    i2c->write(device_address, data_write, 1, 1);

    i2c->read(device_address, data, count, 0);

    for (int i = 0; i < count; i++)
    {
        dest[i] = data[i];
    }
}

/// @brief Write one byte to a specific I2C register.
/// @param address Address of the register.
/// @param data Data to write to the register.
bool AC108::writeByte(uint8_t address, uint8_t data)
{
    char data_write[2];
    data_write[0] = address;
    data_write[1] = data;

    int t = i2c->write(device_address, data_write, 2, false);

    return t == 0;
}