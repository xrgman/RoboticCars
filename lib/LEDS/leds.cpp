#include "leds.h"

/// @brief Enable a specific led.
/// @param ledId The id of the led to enable.
void Leds::enableLed(uint8_t ledId) {
    //Checking if ledId exists:
    if(ledId > 3) {
        return;
    }

    leds[ledId] = 1;
}

/// @brief Disable a specific led.
/// @param ledId The id of the led to disable.
void Leds::disableLed(uint8_t ledId) {
    //Checking if ledId exists:
    if(ledId > 3) {
        return;
    }

    leds[ledId] = 0;
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

/// @brief Get the current state of a led.
/// @param ledId The id of the led.
/// @return Whether the led is turned on or off.
bool Leds::getLed(uint8_t ledId) {
    //Checking if ledId exists:
    if(ledId > 3) {
        //TO-DO error of some kind:
        return false;
    }

    return leds[ledId];
}

/// @brief Set a led effect, this disabled the status blinking.
/// @param effect The effect to display on the leds.
void Leds::setEffect(Effect effect) {
    currentLedEffect = effect;

    //Resetting fields:
    reverse = false;

    // Turning off all leds():
    disableLed(0);
    disableLed(1);
    disableLed(2);
    disableLed(3);
}

/// @brief Get the currently running led effect.
/// @return The current led effect.
Leds::Effect Leds::getCurrentEffect() {
    return currentLedEffect;
}

/// @brief Called from main loop every 50ms, processes led effects:
void Leds::processLedEffect(uint32_t systemCounter) {
    switch(currentLedEffect) {
        case NONE:
        default:
            break;
        case VISOR:
            if(systemCounter % 2 == 0) {
                processVisorEffect();
            }
            
            break;
    }
}

/// @brief Process the visor effect.
void Leds::processVisorEffect() {
    if(!reverse) {
        if(!getLed(BLUE)) {
            enableLed(BLUE);
            return;
        }
        else if(!getLed(YELLOW)) {
            enableLed(YELLOW);
            return;
        }
        else if(!getLed(GREEN)) {
            enableLed(GREEN);
            return;
        }
        else if(!getLed(RED)) {
            enableLed(RED);
            reverse = true;
            return;
        }
    }
    else {
        if(getLed(RED)) {
            disableLed(RED);
            return;
        }
        else if(getLed(GREEN)) {
            disableLed(GREEN);
            return;
        }
        else if(getLed(YELLOW)) {
            disableLed(YELLOW);
            return;
        }
        else if(getLed(BLUE)) {
            disableLed(BLUE);
            reverse = false;
            return;
        }
    }
}