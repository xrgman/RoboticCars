#include "motor.h"

Motor::Motor(PinName motorHall1Pin, Timer* timer) : motorHallSensor(motorHall1Pin) {
    this->timer = timer;
}

/// @brief Initialize the motor.
void Motor::initialize() {
    motorHallSensor.rise(callback(this, &Motor::hallISR));
}

/// @brief Get the current averaged RPM of the motor.
/// @return Curren rpm of motor.
float Motor::getRPM() {
    return rpm;
}

void Motor::hallISR() {
    //Grabbing current elapsed time:
    uint32_t currentTime = timer->elapsed_time().count();
    uint32_t pulseInterval = currentTime - lastPulseTime;
    
    lastPulseTime = currentTime;

    if(pulseInterval > 0) {
        float currentRPM = 60000000.0 / pulseInterval;

        rpmSum = rpmSum - rpmValues[rpmIndex] + currentRPM;
        rpmValues[rpmIndex] = currentRPM;
        rpmIndex = (rpmIndex + 1) % NR_OF_RPM_SAMPLES;

        rpm = rpmSum / NR_OF_RPM_SAMPLES;
    }
}