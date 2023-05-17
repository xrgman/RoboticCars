#include "hcsr04.h"

/// @brief Constructer, intializes the trigger and echo outputs.
/// @param triggerPin The trigger pin.
/// @param echoPin The echo pin.
HCSR04::HCSR04(PinName triggerPin, PinName echoPin) : trigger(triggerPin), echo(echoPin) {
    //Initializing trigger to low state:
    trigger.write(0);
    echo.mode(PullUp);
}

/// @brief Constructer with debugging enabled, intializes the trigger and echo outputs.
/// @param triggerPin The trigger pin.
/// @param echoPin The echo pin.
/// @param communication_protocol Reference to the commonication_protocol, used to relay messages to host.
HCSR04::HCSR04(PinName triggerPin, PinName echoPin, Communication *communication_protocol) : trigger(triggerPin), echo(echoPin) {
    //Initializing trigger to low state:
    trigger.write(0);
    echo.mode(PullUp);
    echo.enable_irq();

    useCommunication = true;
    communication = communication_protocol;
}

/// @brief Start the distance measurement, needs to be called at the start.
void HCSR04::startMeasurement() {
    // Starting pulse:
    trigger.write(1);

    //Attaching interrupts:
    triggerTimeOut.attach(callback(this, &HCSR04::disableTrigger), 10us); //Run trigger for 10us
    //echoTimeOut.attach_us()
    echo.rise(callback(this, &HCSR04::onEchoRise)); //Event called upon receiving first wave back
    
    // Marking data as not new:
    newDataReady = false;
}

/// @brief Check whether the measurement is finished.
/// @return Whether or not new data is available, should be checked before retreiving.
bool HCSR04::isNewDataReady() {
    return newDataReady;
}

/// @brief Get the measured distance in cm.
/// @return Distance in cm.
int HCSR04::getDistanceCm() {
    newDataReady = false;

    return pulseDuration / 58;
}

/// @brief Get the measured distance in inch.
/// @return Distance in inch.
int HCSR04::getDistanceInch() {
    newDataReady = false;

    return pulseDuration / 148;
}

/// @brief Called after trigger time elapsed, turns trigger signal low.
void HCSR04::disableTrigger() {
    trigger.write(0);

    if(useCommunication) {
        communication->sendDebugMessage("Trigger disabled\n");
    }
}

/// @brief Called when a rise in the echo signal is detected.
void HCSR04::onEchoRise() {
    printf("BABABA\n");
    if (!timerStarted)
    {
        timerStarted = true;

        //Starting timer:
        timer.start();

        //Ataching fall event:
        echo.fall(callback(this, &HCSR04::onEchoFall));

        //Disabling rise event callback:
        echo.rise(NULL);
    }

    if(useCommunication) {
        communication->sendDebugMessage("Echo rise event triggered\n");
    }
}

void HCSR04::onEchoFall() {
    //printf("fall\n");
    // Disabling timer:
    timer.stop();

    if(timerStarted) {
        pulseDuration = timer.read();

        newDataReady = true;
    }

    //Resetting all values:
    timer.reset();
    timerStarted = false;
    echo.fall(NULL);

    if(useCommunication) {
        communication->sendDebugMessage("Echo fall event triggered\n");
    }
}
