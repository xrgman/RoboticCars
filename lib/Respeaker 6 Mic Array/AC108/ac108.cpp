#include "ac108.h"
#include "ac108registers.h"

AC108::AC108(PinName sda, PinName scl, uint8_t device_address) : i2c(sda, scl)
{
    this->device_address = device_address;
}

void AC108::checkDeviceOperation(Communication *communication_protocol)
{
    uint8_t response = readByte(0x00);

    if (response == 0x4A)
    { // 74
        communication_protocol->sendDebugMessage("SUCCESS: AC108 found and functioning properly.\r\n");

        return;
    }

    communication_protocol->sendDebugMessage("ERROR: AC108 not found or not functioning properly.\r\n");
}

bool AC108::initialize()
{
    bool ok = true;


    // Reset all registers, readback default as sanity check
    bool test = writeByte(0x00, 0x12);

    if(test) {
        printf("True\n");
    }

    ok &= writeByte(CHIP_RST, CHIP_RST_VAL);
    HAL_Delay(200);
    ok &= CHIP_RST_VAL == readByte(CHIP_RST);

    ok &= writeByte(SYSCLK_CTRL, 0x08); // sys clock 24000000 Hz


    ok &= writeByte(I2S_CTRL, 0x35);// i2s format
    ok &= writeByte(I2S_CTRL, 0xb5); // i2s format

    ok &= writeByte(PWR_CTRL6, 0x01); // power up ac108
    ok &= writeByte(PWR_CTRL7, 0x99); // power up ac108
    ok &= writeByte(PWR_CTRL9, 0x09); // power up ac108

    ok &= writeByte(I2S_LRCK_CTRL1, 0x10); // i2s config LRCK_POLARITY
    ok &= writeByte(I2S_FMT_CTRL1, 0x0d); // PCM_FORMAT
    ok &= writeByte(I2S_FMT_CTRL3, 0x00); // MSB first

    ok &= writeByte(HPF_EN, 0x00); // disable high pass filter
    ok &= writeByte(ADC_DIG_EN, 0x0f); // enable adc

    ok &= writeByte(ANA_ADC1_CTRL1, 0x01); // disable mic bias 1
    ok &= writeByte(ANA_ADC2_CTRL1, 0x01); // disable mic bias 2
    ok &= writeByte(ANA_ADC3_CTRL1, 0x01); // disable mic bias 3
    ok &= writeByte(ANA_ADC4_CTRL1, 0x01); // disable mic bias 4

    ok &= writeByte(ANA_ADC1_CTRL1, 0x05); // disable dsm 1
    ok &= writeByte(ANA_ADC2_CTRL1, 0x05); // disable dsm 2
    ok &= writeByte(ANA_ADC3_CTRL1, 0x05); // disable dsm 3
    ok &= writeByte(ANA_ADC4_CTRL1, 0x05); // disable dsm 4

    ok &= writeByte(ADC_DIG_EN, 0x1f); // disable adc

    ok &= writeByte(ANA_ADC1_CTRL1, 0x07); // enable PGA 1
    ok &= writeByte(ANA_ADC2_CTRL1, 0x07); // enable PGA 2
    ok &= writeByte(ANA_ADC3_CTRL1, 0x07); // enable PGA 3
    ok &= writeByte(ANA_ADC4_CTRL1, 0x07); // enable PGA 4

    ok &= writeByte(0xba, 0x20); // disable dsm globally
    ok &= writeByte(0xbb, 0x0f); // disable adc clk gating globally

    //Setting volume:
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

    i2c.write(device_address, data_write, 1, true);

    i2c.read(device_address, data, 1, false);

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

    i2c.write(device_address, data_write, 1, 1);

    i2c.read(device_address, data, count, 0);

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

    int t = i2c.write(device_address, data_write, 2, false);

    return t == 0;
}