#include "control.h"
#include "pinDefinitions.h"


//DRV8908 motor chip:
DRV8908 drv8908(DRV8908_MOSI_PIN, DRV8908_MISO_PIN, DRV8908_SCK_PIN, DRV8908_NSS_PIN, DRV8908_SLEEP_PIN, DRV8908_FAULT_PIN);

//Used to store new motor values, written to motors by update_motors function:
uint8_t motor[NR_OF_MOTORS];
Direction motorDirection[NR_OF_MOTORS];

void update_motors() {


    //Setting the motor values in DRV8908 chip:
    for (int i = 0; i < NR_OF_MOTORS; i++) {
        //When speed is zero, simply disable the motor:
        if(motor[i] == 0) {
            drv8908.disableMotor(i);
            continue;
        }

        drv8908.setMotor(i, motor[i], motorDirection[i]);
    }

    drv8908.writeChanges();
}

/// @brief Turn off all motors of the car:
void motors_off() {
    motor[0] = motor[1] = motor[2] = motor[3] = 0;

    update_motors();
}

//************************************************
//******** Initialisation + Operation check  *****
//************************************************

/// @brief Move the car a bit forward and than a bit backward.
/// @param comm The communication protocol.
void checkMotorMovement(Communication *comm) {
    comm->sendDebugMessage("Moving forward.....\n");
    
    motor[0] = motor[1] = motor[2] = motor[3] = 100;
    update_motors();
    thread_sleep_for(500);
    motors_off();

    comm->sendDebugMessage("Moving backward.....\n");

    setMotorDirectionAll(REVERSE);
    motor[0] = motor[1] = motor[2] = motor[3] = 100;
    update_motors();
    thread_sleep_for(500);

    //Resetting:
    setMotorDirectionAll(FORWARD);
    motors_off();

    comm->sendDebugMessage("FINISHED: you should have seen the car move forward and than backward.\n");
}

/// @brief Called from main function, used to check whether all motor hardware is functioning correctly.
/// @param comm The communication protocol.
void checkMotorOperation(Communication *comm) {
    //DRV8908:
    comm->sendDebugMessage("\n***** Checking sensor DRV8908 *****\r\n");
    drv8908.checkDeviceOperation(comm);

    //Checking actual movement:
    comm->sendDebugMessage("\n***** Checking motor movement (watch the car move) *****\r\n");
    checkMotorMovement(comm);
}

/// @brief Initialize motor control, by turning all four motors off:
void initializeMotors() {
    //Initialize motor chip:
    drv8908.initialize();

    //Configure all four motors:
    drv8908.configureMotor(0, HALF_BRIDGE_1, HALF_BRIDGE_2, PWM_CHANNEL_1, 10, true); //P13 -> M1 -> OUT1 (m1+), OUT2 (m1-)
    drv8908.configureMotor(1, HALF_BRIDGE_3, HALF_BRIDGE_4, PWM_CHANNEL_1, 10); //P14 -> M2 -> OUT3 (m2+), OUT4 (m2-)
    drv8908.configureMotor(2, HALF_BRIDGE_5, HALF_BRIDGE_6, PWM_CHANNEL_1, 10, true); //P16 -> M3 -> OUT5 (m3+), OUT6 (m3-)
    drv8908.configureMotor(3, HALF_BRIDGE_7, HALF_BRIDGE_8, PWM_CHANNEL_1, 10); //P17 -> M4 -> OUT7 (m4+), OUT8 (m4-)

    //Initializing motor direction to forward:
    motorDirection[0] = motorDirection[1] = motorDirection[2] = motorDirection[3] = FORWARD;

    //Setting motors to idle state:
    motors_off();
}

//*****************************************
//******** Motor direction functions  *****
//*****************************************

/// @brief Set the direction of a specific motor.
/// @param motorId The id of the motor.
/// @param direction The new direction of the motor, Forward, Reverse, Brake.
void setMotorDirection(uint8_t motorId, Direction direction) {
    motorDirection[0] = direction;
}

/// @brief Set the direction of all motors. 
/// @param direction The new direction of the motor, Forward, Reverse, Brake.
void setMotorDirectionAll(Direction direction) {
    motorDirection[0] = motorDirection[1] = motorDirection[2] = motorDirection[3] = direction;
}

int speed = 20;
bool max2 = false;

void test() {
    speed += 1;

    // motor[0] = motor[1] = motor[2] = motor[3] = 100;
    // update_motors();

    // if(speed > 30) {
    //     motors_off();
    // }
}