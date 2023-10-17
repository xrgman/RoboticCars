#include "respeaker6MicArray.h"
#include "pinDefinitions.h"

const int sample_rate = 44100; // 44.1 kHz sample rate
const int beep_frequency = 10000; // Beep frequency in Hz
const int beep_duration = 100; // Beep duration in milliseconds
const int buffer_size = 4410; // 100ms buffer at 44.1 kHz

Respeaker6MicArray::Respeaker6MicArray(PinName buttonPin, PinName sda, PinName scl, Communication *comm) 
    : button(buttonPin), 
        ac101(sda, scl, RESPEAKER6MIC_AC101_ADDRESS, RESPEAKER6MIC_AC101_AMP_EN_PIN),
        ac108_1(sda, scl, RESPEAKER6MIC_AC108_ADDRESS_1), 
        ac108_2(sda, scl, RESPEAKER6MIC_AC108_ADDRESS_2), 
        speaker(RESPEAKER6MIC_I2S_SD, RESPEAKER6MIC_I2S_W, RESPEAKER6MIC_I2S_CLK, comm) {
    communication = comm;

    button.rise(callback(this, &Respeaker6MicArray::onButtonInterruptRise)); //Release

    debounceTimer.start();
}

void Respeaker6MicArray::initialize() {
    //Initializing AC101 chip:
    if(!ac101.initialize()) {
        communication->sendDebugMessage("Failed to start AC101\n");

        return;
    }

    //Initializing AC108 chips:
    // if(!ac108_1.initialize()) {
    //     communication->sendDebugMessage("Failed to start AC108 - 1\n");

    //     //return;
    // }

    // if(!ac108_2.initialize()) {
    //     communication->sendDebugMessage("Failed to start AC108 - 2\n");

    //     //return;
    // }

    ac101.setVolumeSpeaker(60);


    //Setting sample rate for sample:
    ac101.setI2sSampleRate(AC101::SAMPLE_RATE_22050);
    ac101.setI2sWordSize(AC101::WORD_SIZE_16_BITS);

    speaker.initialize();
}

void Respeaker6MicArray::checkDeviceOperation(Communication *communication_protocol) {
    ac101.checkDeviceOperation(communication_protocol);

    //Play beep:
    uint16_t beep_buffer[buffer_size]; // Buffer to store the beep
    //Generating beep:
    // Beep Configuration
    for (int i = 0; i < buffer_size; i++) {
        uint16_t sample = (i < beep_duration * sample_rate / 1000) ? 0x7FFF : 0x0000;
        beep_buffer[i] = sample;
    }

    ac101.setI2sSampleRate(AC101::SAMPLE_RATE_44100);
    ac101.setI2sWordSize(AC101::WORD_SIZE_16_BITS);


    //speaker.write(beep_buffer, buffer_size);

    printf("Beep done!\n");

    ac108_1.checkDeviceOperation(communication_protocol);
    ac108_2.checkDeviceOperation(communication_protocol);
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
