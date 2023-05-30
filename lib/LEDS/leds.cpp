#include "leds.h"


Leds::Leds() {

}

/// @brief Toggle a led based on it's id.
/// @param ledId The id of the led to toggle.
void Leds::toggleLed(uint8_t ledId) {
    //Checking if ledId exists:
    if(ledId > 3) {
        return;
    }

    leds[ledId] = !leds[ledId];
}