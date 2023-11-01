#include "respeaker6MicArray.h"
#include "pinDefinitions.h"
#include "beep.h"

Respeaker6MicArray::Respeaker6MicArray(PinName buttonPin, I2C *i2c, Communication *comm)
    : button(buttonPin),
      ac101(i2c, RESPEAKER6MIC_AC101_ADDRESS, RESPEAKER6MIC_AC101_AMP_EN_PIN),
      ac108_1(i2c, RESPEAKER6MIC_AC108_ADDRESS_1, 0),
      ac108_2(i2c, RESPEAKER6MIC_AC108_ADDRESS_2, 1),
      i2s(RESPEAKER6MIC_I2S_SD, RESPEAKER6MIC_I2S_W, RESPEAKER6MIC_I2S_CLK, comm)
{
    communication = comm;

    // Setting default values:
    sampleRate = I2S_SAMPLE_RATE;
    wordSizeInput = I2S_WORDSIZE_INPUT;
    wordSizeOuput = I2S_WORDSIZE_OUTPUT;
    numChannelsInput = I2S_NUMCHANNELS_INPUT;
    numChannelsOutput = I2S_NUMCHANNELS_OUTPUT;

    button.rise(callback(this, &Respeaker6MicArray::onButtonInterruptRise)); // Release

    debounceTimer.start();
}

bool Respeaker6MicArray::checkDeviceOperation(Communication *communication_protocol)
{
    bool ok = ac101.checkDeviceOperation(communication_protocol);

    ok &= ac108_1.checkDeviceOperation(communication_protocol);
    ok &= ac108_2.checkDeviceOperation(communication_protocol);

    // Saving old configuration:
    SampleRate oldSampleRate = sampleRate;
    WordSize oldWordSizeInput = wordSizeInput;
    WordSize oldWordSizeOutput = wordSizeOuput;
    NumChannels oldNumChannelsInput = numChannelsInput;
    NumChannels oldNumChannelsOutput = numChannelsOutput;

    // Playing beep:
    setVolumeSpeaker(40);
    ok &= configure(SAMPLE_RATE_44100, WORD_SIZE_CURRENT, WORD_SIZE_16_BITS, NUM_CHANNELS_CURRENT, NUM_CHANNELS_CURRENT);

    i2s.write(data_beep, nrElementsBeep);

    // Waiting and then restoring values:
    HAL_Delay(1000);

    ok &= configure(oldSampleRate, oldWordSizeInput, oldWordSizeOutput, oldNumChannelsInput, oldNumChannelsOutput);

    return ok;
}

bool Respeaker6MicArray::initialize()
{
    // Initializing AC101 chip:
    if (!ac101.initialize())
    {
        communication->sendDebugMessage("Failed to start AC101\n");

        return false;
    }

    // Initializing AC108 chips:
    // When I initialize both, I2S stops working (ISR not getting called anymore) -> Reset required
    // Problem is within the respeaker module, thats the one that needs resetting :)
    // Resetting the ac108 properly also fixes it, so something is wrong during initalisation
    if (!ac108_1.initialize(SAMPLE_RATE_44100, WORD_SIZE_16_BITS, NUM_CHANNELS_8))
    {
        communication->sendDebugMessage("Failed to start AC108 - 1\n");

        return false;
    }

    if (!ac108_2.initialize(SAMPLE_RATE_44100, WORD_SIZE_16_BITS, NUM_CHANNELS_8))
    {
        communication->sendDebugMessage("Failed to start AC108 - 2\n");

        return false;
    }

    // ac108_2.startCapture44();

    // ThisThread::sleep_for(10s);

    // ac108_2.stopCapture();

    // Register I2S callbacks:
    i2s.setOnTxCpltCallback(callback(this, &Respeaker6MicArray::onI2STxCpltCallback));
    i2s.setOnRxCpltCallback(callback(this, &Respeaker6MicArray::onI2SRxCpltCallback));

    // Configure the respeaker module:
    bool ok = configure(I2S_SAMPLE_RATE, I2S_WORDSIZE_INPUT, I2S_WORDSIZE_OUTPUT, I2S_NUMCHANNELS_INPUT, I2S_NUMCHANNELS_OUTPUT);

    if (!ok)
    {
        communication->sendDebugMessage("Failed to configure Respeaker module\n");

        return false;
    }

    return true;
}

/// @brief Set the callback function for when the button on the Respeaker board is clicked.
/// @param onButtonClick Function to be called on button click.
void Respeaker6MicArray::setOnButtonClickListener(void (*onButtonClick)())
{
    this->onButtonClick = onButtonClick;
}

void Respeaker6MicArray::setOnI2SWriteDoneListener(Callback<void()> onI2SWriteDone)
{
    this->onI2SWriteDone = onI2SWriteDone;
}

void Respeaker6MicArray::setOnI2SReadDoneListener(Callback<void()> onI2SReadDone)
{
    this->onI2SReadDone = onI2SReadDone;
}

/// @brief Set the volume (0..63) of the speaker on the Respeaker array.
/// @param volume The new volume value.
/// @return Whether setting the volume succeeded.
bool Respeaker6MicArray::setVolumeSpeaker(uint8_t volume)
{
    if (volume > 63)
    {
        return false;
    }

    return ac101.setVolumeSpeaker(volume);
}

// TODO:
bool Respeaker6MicArray::setVolumeMicrophones(uint8_t volume)
{
    bool ok = true;

    ok &= ac108_1.setVolume(volume);
    ok &= ac108_2.setVolume(volume);

    return ok;
}

/// @brief Configure respeaker speaker to the given sample rate and word size, depending on data.
/// @param sampleRate Sample rate of the data that needs to be outputted.
/// @param wordSize  Word size of the data to be outputted
/// @return Whether or not it was a success.
bool Respeaker6MicArray::configure(SampleRate sampleRate, WordSize wordSizeInput, WordSize wordSizeOutput, NumChannels numChannelsInput, NumChannels numChannelsOutput)
{
    bool ok = true;

    // Saving values for later:
    this->sampleRate = sampleRate == SAMPLE_RATE_CURRENT ? this->sampleRate : sampleRate;
    this->wordSizeInput = wordSizeInput == WORD_SIZE_CURRENT ? this->wordSizeInput : wordSizeInput;
    this->wordSizeOuput = wordSizeOuput == WORD_SIZE_CURRENT ? this->wordSizeOuput : wordSizeOuput;
    this->numChannelsInput = numChannelsInput == NUM_CHANNELS_CURRENT ? this->numChannelsInput : numChannelsInput;
    this->numChannelsOutput = numChannelsOutput == NUM_CHANNELS_CURRENT ? this->numChannelsOutput : numChannelsOutput;

    // Setting word size input:
    uint8_t wordSizeInputI2S = SAI_PROTOCOL_DATASIZE_16BIT;

    switch (wordSizeInput)
    {
    case WORD_SIZE_CURRENT:
        // wordSizeAC108 =
        wordSizeInputI2S = i2s.getWordSizeInput();
        break;
    case WORD_SIZE_8_BITS:

        wordSizeInputI2S = SAI_PROTOCOL_DATASIZE_16BIT;
        break;
    case WORD_SIZE_16_BITS:

        wordSizeInputI2S = SAI_PROTOCOL_DATASIZE_16BIT;
        break;
    case WORD_SIZE_20_BITS:

        wordSizeInputI2S = SAI_PROTOCOL_DATASIZE_24BIT;
        break;
    case WORD_SIZE_24_BITS:

        wordSizeInputI2S = SAI_PROTOCOL_DATASIZE_24BIT;
        break;
    }

    // TODO set in ac108

    // Setting word size output:
    AC101::I2sWordSize_t wordSizeAC101 = AC101::WORD_SIZE_8_BITS;
    uint8_t wordSizeOutputI2S = SAI_PROTOCOL_DATASIZE_16BIT;

    switch (wordSizeOutput)
    {
    case WORD_SIZE_CURRENT:
        // wordSizeAC101 =
        wordSizeOutputI2S = i2s.getWordSizeOutput();
        break;
    case WORD_SIZE_8_BITS:
        wordSizeAC101 = AC101::WORD_SIZE_8_BITS;
        wordSizeOutputI2S = SAI_PROTOCOL_DATASIZE_16BIT;
        break;
    case WORD_SIZE_16_BITS:
        wordSizeAC101 = AC101::WORD_SIZE_16_BITS;
        wordSizeOutputI2S = SAI_PROTOCOL_DATASIZE_16BIT;
        break;
    case WORD_SIZE_20_BITS:
        wordSizeAC101 = AC101::WORD_SIZE_20_BITS;
        wordSizeOutputI2S = SAI_PROTOCOL_DATASIZE_24BIT;
        break;
    case WORD_SIZE_24_BITS:
        wordSizeAC101 = AC101::WORD_SIZE_24_BITS;
        wordSizeOutputI2S = SAI_PROTOCOL_DATASIZE_24BIT;
        break;
    }

    ok &= ac101.setI2sWordSize(wordSizeAC101);

    // Setting sample rate:
    AC101::I2sSampleRate_t sampleRateAC101 = AC101::SAMPLE_RATE_8000;
    uint32_t sampleRateI2S = SAI_AUDIO_FREQUENCY_8K;

    switch (sampleRate)
    {
    case SAMPLE_RATE_CURRENT:
        // TODO get from ac101 and ac108
        sampleRateI2S = i2s.getSampleRate();
        break;
    case SAMPLE_RATE_8000:
        sampleRateAC101 = AC101::SAMPLE_RATE_8000;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_8K;
        break;
    case SAMPLE_RATE_11052:
        sampleRateAC101 = AC101::SAMPLE_RATE_11052;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_11K;
        break;
    case SAMPLE_RATE_16000:
        sampleRateAC101 = AC101::SAMPLE_RATE_16000;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_16K;
        break;
    case SAMPLE_RATE_22050:
        sampleRateAC101 = AC101::SAMPLE_RATE_22050;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_22K;
        break;
    case SAMPLE_RATE_32000:
        sampleRateAC101 = AC101::SAMPLE_RATE_32000;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_32K;
        break;
    case SAMPLE_RATE_44100:
        sampleRateAC101 = AC101::SAMPLE_RATE_44100;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_44K;
        break;
    case SAMPLE_RATE_48000:
        sampleRateAC101 = AC101::SAMPLE_RATE_48000;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_48K;
        break;
    case SAMPLE_RATE_96000:
        sampleRateAC101 = AC101::SAMPLE_RATE_96000;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_96K;
        break;
    case SAMPLE_RATE_192000:
        sampleRateAC101 = AC101::SAMPLE_RATE_192000;
        sampleRateI2S = SAI_AUDIO_FREQUENCY_192K;
        break;
    }

    ok &= ac101.setI2sSampleRate(sampleRateAC101);
    // TODO set in ac108.

    // Get number channels input:
    uint8_t channelsInput = numChannelsInput == NUM_CHANNELS_CURRENT ? i2s.getNumChannelsInput() : ((uint8_t)numChannelsInput);
    uint8_t channelsOutput = numChannelsOutput == NUM_CHANNELS_CURRENT ? i2s.getNumChannelsOutput() : ((uint8_t)numChannelsOutput);

    ok &= i2s.initialize(sampleRateI2S, wordSizeInputI2S, wordSizeOutputI2S, channelsInput, channelsOutput);

    return ok;
}

/// @brief Get pointer to the 16 bit read buffer of I2S.
/// @return Pointer to the read buffer.
uint16_t *Respeaker6MicArray::getRxBuffer16()
{
    return i2s.getRxBuffer16();
}

/// @brief Get pointer to the 32 read buffer of I2S.
/// @return Pointer to the read buffer.
uint32_t *Respeaker6MicArray::getRxBuffer32()
{
    return i2s.getRxBuffer32();
}

//**************************************
//******** Write functions *************
//**************************************

/// @brief Write 8 bit data to the speaker module.
/// @param data Data to write.
/// @param size Size of the data array.
void Respeaker6MicArray::writeSpeaker8(uint8_t *data, uint16_t size)
{
    i2s.write(data, size);
}

/// @brief Write 16 bit data to the speaker module.
/// @param data Data to write.
/// @param size Size of the data array.
void Respeaker6MicArray::writeSpeaker16(uint16_t *data, uint16_t size)
{
    i2s.write(data, size);
}

/// @brief Write 32 bit data to the speaker module.
/// @param data Data to write.
/// @param size Size of the data array.
void Respeaker6MicArray::writeSpeaker32(uint32_t *data, uint16_t size)
{
    i2s.write(data, size);
}

//**************************************
//******** Callbacks *******************
//**************************************

/// @brief Private method triggered on rising edge (release) of the button.
void Respeaker6MicArray::onButtonInterruptRise()
{
    if (debounceTimer.elapsed_time().count() > 50)
    {
        onButtonClick();
    }

    debounceTimer.reset();
}

void Respeaker6MicArray::onI2STxCpltCallback()
{
    if (onI2SWriteDone)
    {
        onI2SWriteDone();
    }
}

void Respeaker6MicArray::onI2SRxCpltCallback()
{
    if (onI2SReadDone)
    {
        onI2SReadDone();
    }
}

void Respeaker6MicArray::run()
{
}