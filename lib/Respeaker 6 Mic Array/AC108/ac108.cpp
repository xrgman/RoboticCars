#include "ac108.h"
#include "ac108registers.h"

AC108::AC108(I2C *i2c, uint8_t device_address)
{
    this->i2c = i2c;
    this->device_address = device_address;
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

bool AC108::initialize()
{
    bool ok = true;

    // Reset all registers, readback default as sanity check
    ok &= writeByte(CHIP_RST, CHIP_RST_VAL);
    HAL_Delay(200);
    ok &= CHIP_RST_VAL == readByte(CHIP_RST);

    ok &= writeByte(SYSCLK_CTRL, 0b00001000); // sys clock 24000000 Hz

    ok &= writeByte(I2S_CTRL, 0b00110101); // i2s format
    ok &= writeByte(I2S_CTRL, 0b10110101); // i2s format

    ok &= writeByte(PWR_CTRL6, 0b00000001); // power up ac108
    ok &= writeByte(PWR_CTRL7, 0b10011001); // power up ac108
    ok &= writeByte(PWR_CTRL9, 0b00001001); // power up ac108

    ok &= writeByte(I2S_LRCK_CTRL1, 0b00010000); // i2s config LRCK_POLARITY
    ok &= writeByte(I2S_FMT_CTRL1, 0b00001101);  // PCM_FORMAT
    ok &= writeByte(I2S_FMT_CTRL3, 0b00000000);  // MSB first

    ok &= writeByte(HPF_EN, 0b00000000);     // disable high pass filter
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

    // Setting volume:
    ok &= setVolume(0xde);

    return ok;
}

/// @brief Check whether capture has started.
/// @return Whether or not capturing is in progress.
bool AC108::captureStarted()
{
    return readByte(I2S_CTRL) == 0xF5;
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
    ok &= writeByte(I2S_FMT_CTRL2, 0b00110011); // sample resolution 16 bit
    ok &= writeByte(ADC_SPRC, 0b00000111); // sample rate 44100 Hz
    ok &= writeByte(HPF_EN, 0b00001111); // enable high pass filter

    ok &= writeByte(PLL_CTRL5, 0b00011000); // pll divider config
    ok &= writeByte(PLL_CTRL4, 0b01001100); // pll loop divider
    ok &= writeByte(PLL_CTRL3, 0b00000010); // pll loop divider MSB
    ok &= writeByte(PLL_CTRL2, 0b00011000); // pll pre divider
    ok &= writeByte(PLL_LOCK_CTRL, 0b00000001); // pll clock lock en
    ok &= writeByte(SYSCLK_CTRL, 0b10001001); // pll clock source MCLK

    ok &= writeByte(I2S_BCLK_CTRL, 0b00000011); // bclk div

    ok &= writeByte(I2S_TX1_CTRL1, 0b00000011); // set channels
    ok &= writeByte(I2S_TX1_CTRL2, 0b00001111); // tx config
    ok &= writeByte(I2S_TX1_CTRL3, 0b00000000); // tx config
    ok &= writeByte(I2S_TX1_CHMP_CTRL1, 0b01001110); // tx chip map
    ok &= writeByte(I2S_TX1_CHMP_CTRL2, 0b00000000); // tx chip map
    ok &= writeByte(I2S_TX1_CHMP_CTRL3, 0b00000000); // tx chip map
    ok &= writeByte(I2S_TX1_CHMP_CTRL4, 0b00000000); // tx chip map

    ok &= writeByte(MOD_CLK_EN, 0b10010011); // mod clk en
    ok &= writeByte(MOD_RST_CTRL, 0b10010011); // mod rst ctrl

    ok &= writeByte(I2S_CTRL, 0b10110000); // disable all clocks
    ok &= writeByte(I2S_CTRL, 0b11110000); // enable lrck clock
    ok &= writeByte(PLL_CTRL1, 0b01001011); // enable pll
    ok &= writeByte(I2S_CTRL, 0b11110101); // enable global clock

    return ok;
}

bool AC108::stopCapture() {
    bool ok = true;

    ok &= writeByte(I2S_CTRL, 0b11110000); // disable global clock
    ok &= writeByte(PLL_CTRL1, 0b01001000); // disable pll
    ok &= writeByte(I2S_CTRL, 0b10110000); // disable all clocks
    ok &= writeByte(MOD_CLK_EN, 0b00000000); // disable mod clk
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