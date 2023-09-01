#ifndef CONTROL_H
#define CONTROL_H

#include "mbed.h"
#include "drv8908.h"
#include "communication.h"
#include "statemachine.h"

#define NR_OF_MOTORS 4
#define MIN_MOTOR_VAL 60
#define MAX_MOTOR_VAL 255 //Only allow motors to speed up x%

typedef struct {
    bool forward;
    bool backward;
    bool left;
    bool right;
    bool pivotLeft;
    bool pivotRight;
} Controls;

void checkMotorOperation(Communication *comm);
void initializeMotors(Communication *comm);

void processMotors(Statemachine::State currentState);
void processControlCommand(Communication* comm, uint8_t command[]);

//Getters and setters:
void setMotorDirection(uint8_t motorId, Direction direction);
void setMotorDirectionAll(Direction direction);
uint8_t* getMotorSpeeds();
Direction* getMotorDirections();

//Attach interrupt to hall sensors? and just log that it happens, other thread can do the calculation 




#endif