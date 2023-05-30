#ifndef CONTROL_H
#define CONTROL_H

#include "mbed.h"
#include "drv8908.h"
#include "communication.h"
#include "statemachine.h"

#define NR_OF_MOTORS 4
#define MAX_MOTOR_VAL 200 //Only allow motors to speed up x%

void checkMotorOperation(Communication *comm);
void initializeMotors();

void processMotors(Statemachine::State currentState);

//Getters and setters:
void setMotorDirection(uint8_t motorId, Direction direction);
void setMotorDirectionAll(Direction direction);
uint8_t* getMotorSpeeds();
Direction* getMotorDirections();


#endif