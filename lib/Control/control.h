#ifndef CONTROL_H
#define CONTROL_H

#include "mbed.h"
#include "drv8908.h"
#include "communication.h"

#define NR_OF_MOTORS 4
#define MAX_MOTOR_VAL 200 //Only allow motors to speed up x%

void checkMotorOperation(Communication *comm);
void initializeMotors();

void setMotorDirection(uint8_t motorId, Direction direction);
void setMotorDirectionAll(Direction direction);

void test();

//Function to process motor control:



#endif