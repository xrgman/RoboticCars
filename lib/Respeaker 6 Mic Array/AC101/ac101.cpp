#include "ac101.h"
#include "ac101registers.h"
#include "util.h"

AC101::AC101(PinName sda, PinName scl, uint8_t device_address, PinName ampEnablePin) : i2c(sda, scl), enableAmp(ampEnablePin)
{
	this->device_address = device_address;
}

void AC101::checkDeviceOperation(Communication *communication_protocol) {
    uint16_t response = readByte(0x00);

    if(response == 0x101 && initialized) { //257
        communication_protocol->sendDebugMessage("SUCCESS: AC101 found and functioning properly.\r\n");

		return;
    }
	else if(response == 0x101 && !initialized) { //257
        communication_protocol->sendDebugMessage("ERROR: AC101 found but not functioning properly.\r\n");

        return;
    }

    communication_protocol->sendDebugMessage("ERROR: AC101 not found or not functioning properly.\r\n");
}

bool AC101::initialize()
{
	bool ok = true;

	// Reset all registers, readback default as sanity check
	ok &= writeByte(CHIP_AUDIO_RS, 0x123);
	HAL_Delay(100);
	ok &= 0x0101 == readByte(CHIP_AUDIO_RS);

	ok &= writeByte(SPKOUT_CTRL, 0xe880);

	// Enable the PLL from 256*44.1KHz MCLK source
	ok &= writeByte(PLL_CTRL1, 0x014f);
	ok &= writeByte(PLL_CTRL2, 0x8600);

	// Clocking system
	ok &= writeByte(SYSCLK_CTRL, 0x8b08);
	ok &= writeByte(MOD_CLK_ENA, 0x800c);
	ok &= writeByte(MOD_RST_CTRL, 0x800c);

	// Set default at I2S, 44.1KHz, 16bit
	ok &= setI2sSampleRate(SAMPLE_RATE_44100);
	ok &= setI2sClock(BCLK_DIV_8, false, LRCK_DIV_32, false);
	ok &= setI2sMode(MODE_SLAVE);
	ok &= setI2sWordSize(WORD_SIZE_16_BITS);
	ok &= setI2sFormat(DATA_FORMAT_I2S);

	// AIF config
	ok &= writeByte(I2S1_SDOUT_CTRL, 0xc000);
	ok &= writeByte(I2S1_SDIN_CTRL, 0xc000);
	ok &= writeByte(I2S1_MXR_SRC, 0x2200);

	ok &= writeByte(ADC_SRCBST_CTRL, 0xccc4);
	ok &= writeByte(ADC_SRC, 0x2020);
	ok &= writeByte(ADC_DIG_CTRL, 0x8000);
	ok &= writeByte(ADC_APC_CTRL, 0xbbc3);

	// Path Configuration
	ok &= writeByte(DAC_MXR_SRC, 0xcc00);
	ok &= writeByte(DAC_DIG_CTRL, 0x8000);
	ok &= writeByte(OMIXER_SR, 0x0081);
	ok &= writeByte(OMIXER_DACA_CTRL, 0xf080);

	ok &= setMode( MODE_DAC );

	//Enabling amplifier:
	enableAmp.write(1);

	initialized = true;

	return ok;
}

void AC101::printRegisters()
{
	for (size_t i = 0; i < 93; ++i)
	{
		uint16_t data = readByte(i);

		printf("0x%x = (0x%x) ->", i, data);
		Util::printAsBinary(data);
	}
}

//****************************************
//******** Change volumne functions ******
//****************************************

uint8_t AC101::getVolumeSpeaker()
{
	// Times 2, to scale to same range as headphone volume
	return (readByte(SPKOUT_CTRL) & 31) * 2;
}

bool AC101::setVolumeSpeaker(uint8_t volume)
{
	// Divide by 2, as it is scaled to same range as headphone volume
	volume /= 2;
	if (volume > 31) volume = 31;

	uint16_t val = readByte(SPKOUT_CTRL);
	val &= ~31;
	val |= volume;
	return writeByte(SPKOUT_CTRL, val);
}

uint8_t AC101::getVolumeHeadphone()
{
	return (readByte(HPOUT_CTRL) >> 4) & 63;
}

bool AC101::setVolumeHeadphone(uint8_t volume)
{
	if (volume > 63) volume = 63;

	uint16_t val = readByte(HPOUT_CTRL);
	val &= ~63 << 4;
	val |= volume << 4;
	return writeByte(HPOUT_CTRL, val);
}

//****************************************
//******** Change I2S settings ***********
//****************************************

bool AC101::setI2sSampleRate(I2sSampleRate_t rate)
{
	return writeByte(I2S_SR_CTRL, rate);
}

bool AC101::setI2sMode(I2sMode_t mode)
{
	uint16_t val = readByte(I2S1_CLK_CTRL);
	val &= ~0x8000;
	val |= uint16_t(mode) << 15;
	return writeByte(I2S1_CLK_CTRL, val);
}

bool AC101::setI2sWordSize(I2sWordSize_t size)
{
	uint16_t val = readByte(I2S1_CLK_CTRL);
	val &= ~0x0030;
	val |= uint16_t(size) << 4;
	return writeByte(I2S1_CLK_CTRL, val);
}

bool AC101::setI2sFormat(I2sFormat_t format)
{
	uint16_t val = readByte(I2S1_CLK_CTRL);
	val &= ~0x000C;
	val |= uint16_t(format) << 2;
	return writeByte(I2S1_CLK_CTRL, val);
}

bool AC101::setI2sClock(I2sBitClockDiv_t bitClockDiv, bool bitClockInv, I2sLrClockDiv_t lrClockDiv, bool lrClockInv)
{
	uint16_t val = readByte(I2S1_CLK_CTRL);
	val &= ~0x7FC0;
	val |= uint16_t(bitClockInv ? 1 : 0) << 14;
	val |= uint16_t(bitClockDiv) << 9;
	val |= uint16_t(lrClockInv ? 1 : 0) << 13;
	val |= uint16_t(lrClockDiv) << 6;
	return writeByte(I2S1_CLK_CTRL, val);
}

bool AC101::setMode(Mode_t mode)
{
	bool ok = true;

	if (MODE_LINE == mode)
	{
		ok &= writeByte(ADC_SRC, 0x0408);
		ok &= writeByte(ADC_DIG_CTRL, 0x8000);
		ok &= writeByte(ADC_APC_CTRL, 0x3bc0);
	}

	if ((MODE_ADC == mode) or (MODE_ADC_DAC == mode) or (MODE_LINE == mode))
	{
		ok &= writeByte(MOD_CLK_ENA,  0x800c);
		ok &= writeByte(MOD_RST_CTRL, 0x800c);
	}

	if ((MODE_DAC == mode) or (MODE_ADC_DAC == mode) or (MODE_LINE == mode))
	{
		// Enable Headphone output
		ok &= writeByte(OMIXER_DACA_CTRL, 0xff80);
		HAL_Delay(100);
		ok &= writeByte(HPOUT_CTRL, 0xfbc0);
		ok &= setVolumeHeadphone(30);

		// Enable Speaker output
		ok &= writeByte(SPKOUT_CTRL, 0xeabd);
		HAL_Delay(10);
		ok &= setVolumeSpeaker(30);
	}

	return ok;
}

//************************************
//******** I2C helper functions ******
//************************************

/// @brief Read one byte from the requested I2C register.
/// @param address Address of the register.
/// @return Data currently in the register.
uint16_t AC101::readByte(uint8_t address) {
    char data[2];
    char data_write[1];
    data_write[0] = address;

    i2c.write(device_address, data_write, 1, true);

    i2c.read(device_address, data, 2, false);

    return Util::to_ui16(data);
}

/// @brief Read multiple bytes from a specific I2C register.
/// @param address Address of the register.
/// @param count Number of bytes to read
/// @param dest Array to store resulting data in.
void AC101::readBytes(uint8_t address, uint8_t count, uint16_t *dest) {
    char data[count];
    char data_write[1];
    data_write[0] = address;

    i2c.write(device_address, data_write, 1, 1); 

    i2c.read(device_address, data, count, 0); 

    for(int i = 0; i < count; i+=2) {
        uint16_t ret = 0;
	    ret |= (data[i] << 8);
	    ret |= data[i+1];

        dest[i] = ret;
    }
}

/// @brief Write one byte to a specific I2C register.
/// @param address Address of the register.
/// @param data Data to write to the register.
bool AC101::writeByte(uint8_t address, uint16_t data) {
    char data_write[3];
    data_write[0] = address;

    //Higher data byte:
    data_write[1] = (data >> 8) & 0xFF;

    //Lower data byte:
    data_write[2] = data & 0xFF;

    return i2c.write(device_address, data_write, 3, false) == 0;
}
