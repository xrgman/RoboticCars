#include "respeaker6MicArray.h"
#include "pinDefinitions.h"
#include "beep.h"

uint8_t buff[1024];

Respeaker6MicArray::Respeaker6MicArray(PinName buttonPin, I2C *i2c, Communication *comm)
    : button(buttonPin),
      ac101(i2c, RESPEAKER6MIC_AC101_ADDRESS, RESPEAKER6MIC_AC101_AMP_EN_PIN),
      ac108_1(i2c, RESPEAKER6MIC_AC108_ADDRESS_1),
      ac108_2(i2c, RESPEAKER6MIC_AC108_ADDRESS_2),
      i2s(RESPEAKER6MIC_I2S_SD, RESPEAKER6MIC_I2S_W, RESPEAKER6MIC_I2S_CLK, comm)
{
    communication = comm;

    button.rise(callback(this, &Respeaker6MicArray::onButtonInterruptRise)); // Release

    debounceTimer.start();
}

bool Respeaker6MicArray::checkDeviceOperation(Communication *communication_protocol)
{
    bool ok = ac101.checkDeviceOperation(communication_protocol);

    ok &= ac108_1.checkDeviceOperation(communication_protocol);
    ok &= ac108_2.checkDeviceOperation(communication_protocol);

    // Playing beep:
    setVolumeSpeaker(40);
    configureSpeaker(SAMPLE_RATE_44100, WORD_SIZE_16_BITS);

    i2s.write(data_beep, nrElementsBeep);

    return ok;
}

void Respeaker6MicArray::initialize()
{
    // Initializing AC101 chip:
    if (!ac101.initialize())
    {
        communication->sendDebugMessage("Failed to start AC101\n");

        return;
    }

    // Initializing AC108 chips:
    // When I initialize both, I2S stops working (ISR not getting called anymore) -> Reset required
    // if (!ac108_1.initialize())
    // {
    //     communication->sendDebugMessage("Failed to start AC108 - 1\n");

    //     return;
    // }

    // if(!ac108_2.initialize()) {
    //     communication->sendDebugMessage("Failed to start AC108 - 2\n");

    //     return;
    // }

    // ac108_2.startCapture44();

    // ThisThread::sleep_for(10s);

    // ac108_2.stopCapture();

    // Initialize i2s for 44Khz and 16 bits:
    i2s.initialize(SAI_AUDIO_FREQUENCY_44K, SAI_PROTOCOL_DATASIZE_16BIT, SAI_PROTOCOL_DATASIZE_16BIT);
    i2s.setOnTxCpltCallback(callback(this, &Respeaker6MicArray::onI2STxCpltCallback));
    i2s.setOnRxCpltCallback(callback(this, &Respeaker6MicArray::onI2SRxCpltCallback));
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
bool Respeaker6MicArray::configureSpeaker(SampleRate sampleRate, WordSize wordSize)
{
    bool ok = true;

    // Setting word size:
    AC101::I2sWordSize_t wordSizeAC101 = AC101::WORD_SIZE_8_BITS;
    uint8_t wordSizeI2S = SAI_PROTOCOL_DATASIZE_16BIT;

    switch (wordSize)
    {
    case WORD_SIZE_8_BITS:
        wordSizeAC101 = AC101::WORD_SIZE_8_BITS;
        wordSizeI2S = SAI_PROTOCOL_DATASIZE_16BIT;
        break;
    case WORD_SIZE_16_BITS:
        wordSizeAC101 = AC101::WORD_SIZE_16_BITS;
        wordSizeI2S = SAI_PROTOCOL_DATASIZE_16BIT;
        break;
    case WORD_SIZE_20_BITS:
        wordSizeAC101 = AC101::WORD_SIZE_20_BITS;
        wordSizeI2S = SAI_PROTOCOL_DATASIZE_24BIT;
        break;
    case WORD_SIZE_24_BITS:
        wordSizeAC101 = AC101::WORD_SIZE_24_BITS;
        wordSizeI2S = SAI_PROTOCOL_DATASIZE_24BIT;
        break;
    }

    ok &= ac101.setI2sWordSize(wordSizeAC101);

    // Setting sample rate:
    AC101::I2sSampleRate_t sampleRateAC101 = AC101::SAMPLE_RATE_8000;
    uint32_t sampleRateI2S = SAI_AUDIO_FREQUENCY_8K;

    switch (sampleRate)
    {
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
    // ok &= ac101.setI2sSampleRate(AC101::SAMPLE_RATE_44100);

    i2s.initialize(sampleRateI2S, wordSizeI2S, i2s.getWordSizeInput());

    return ok;
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
//******** Read functions *************
//**************************************

/// @brief Read 8 bit data to the speaker module.
/// @param data Array to read data into.
/// @param size Size of the data array.
void Respeaker6MicArray::readMicrophones8(uint8_t *data, uint16_t size)
{
    i2s.read(data, size);
}

/// @brief Read 16 bit data to the speaker module.
/// @param data Array to read data into.
/// @param size Size of the data array.
void Respeaker6MicArray::readMicrophones16(uint16_t *data, uint16_t size)
{
    i2s.read(data, size);
}

/// @brief Read 32 bit data to the speaker module.
/// @param data Array to read data into.
/// @param size Size of the data array.
void Respeaker6MicArray::readMicrophones32(uint32_t *data, uint16_t size)
{
    i2s.read(data, size);
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

    // char msg[100];
    // snprintf(msg, sizeof(msg), "Data: %d, %d, %d, %d.\n", buff[0], buff[64], buff[256], buff[512]);
    // communication->sendDebugMessage(msg);)
}

void Respeaker6MicArray::run()
{
    i2s.run();
}

void Respeaker6MicArray::test() {
    i2s.write(buff, 1024);
    // i2s.read(buff, 1024);
}