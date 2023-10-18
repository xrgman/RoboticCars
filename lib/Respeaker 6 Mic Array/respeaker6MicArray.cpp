#include "respeaker6MicArray.h"
#include "pinDefinitions.h"
#include "beep.h"

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

    //Playing beep:
    ac101.setVolumeSpeaker(40);
    ac101.setI2sSampleRate(AC101::SAMPLE_RATE_44100);
    ac101.setI2sWordSize(AC101::WORD_SIZE_16_BITS);

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
    //  if(!ac108_1.initialize()) {
    //      communication->sendDebugMessage("Failed to start AC108 - 1\n");

    //     //return;
    // }

    // if(!ac108_2.initialize()) {
    //     communication->sendDebugMessage("Failed to start AC108 - 2\n");

    //     //return;
    // }

    

    // Setting sample rate for sample:
    // ac101.setI2sSampleRate(AC101::SAMPLE_RATE_22050);
    // ac101.setI2sWordSize(AC101::WORD_SIZE_16_BITS);

    // Initialize i2s:
    i2s.initialize();
}

/// @brief Set the callback function for when the button on the Respeaker board is clicked.
/// @param onButtonClick Function to be called on button click.
void Respeaker6MicArray::setOnButtonClickListener(void (*onButtonClick)())
{
    this->onButtonClick = onButtonClick;
}

/// @brief Private method triggered on rising edge (release) of the button.
void Respeaker6MicArray::onButtonInterruptRise()
{
    if (debounceTimer.elapsed_time().count() > 50)
    {
        onButtonClick();
    }

    debounceTimer.reset();
}

void Respeaker6MicArray::loop()
{
    // i2s.loop();
}
