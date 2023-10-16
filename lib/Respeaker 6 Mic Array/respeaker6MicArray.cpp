#include "respeaker6MicArray.h"
#include "pinDefinitions.h"

Respeaker6MicArray::Respeaker6MicArray(PinName buttonPin, PinName sda, PinName scl, Communication *comm) 
    : button(buttonPin), 
        //ac108_1(sda, scl), ac108_2(sda, scl, RESPEAKER6MIC_AC108_ADDRESS_2), 
        ac101(sda, scl, RESPEAKER6MIC_AC101_AMP_EN_PIN),
        speaker(RESPEAKER6MIC_I2S_SD, RESPEAKER6MIC_I2S_W, RESPEAKER6MIC_I2S_CLK, comm) {
    communication = comm;

    button.rise(callback(this, &Respeaker6MicArray::onButtonInterruptRise)); //Release

    debounceTimer.start();
}

void Respeaker6MicArray::initialize() {
    //Starting AC101 chip:
    if(!ac101.initialize()) {
        communication->sendDebugMessage("Failed to start AC101");

        return;
    }

    ac101.setVolumeSpeaker(30);
 

    communication->sendDebugMessage("Succesfully initialized AC101\n");

    //Setting sample rate for sample:
    ac101.setI2sSampleRate(AC101::SAMPLE_RATE_22050);
    ac101.setI2sWordSize(AC101::WORD_SIZE_16_BITS);

    speaker.initialize();
}

void Respeaker6MicArray::checkDeviceOperation(Communication *communication_protocol) {
    ac101.checkDeviceOperation(communication_protocol);
    
    //ac108_1.checkDeviceOperation(communication_protocol);
    //ac108_2.checkDeviceOperation(communication_protocol);
}

/// @brief Set the callback function for when the button on the Respeaker board is clicked.
/// @param onButtonClick Function to be called on button click.
void Respeaker6MicArray::setOnButtonClickListener(void (*onButtonClick)()) {
    this->onButtonClick = onButtonClick;
}

/// @brief Private method triggered on rising edge (release) of the button.
void Respeaker6MicArray::onButtonInterruptRise() {
    if(debounceTimer.elapsed_time().count() > 50) {
        onButtonClick();
    }

    debounceTimer.reset();
}


void Respeaker6MicArray::loop() {
    speaker.loop();
}
