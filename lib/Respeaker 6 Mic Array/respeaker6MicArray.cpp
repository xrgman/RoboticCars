#include "respeaker6MicArray.h"


Respeaker6MicArray::Respeaker6MicArray(PinName buttonPin, Communication *comm) : button(buttonPin) {
    communication = comm;

    button.rise(callback(this, &Respeaker6MicArray::onButtonInterruptRise)); //Release

    debounceTimer.start();
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
