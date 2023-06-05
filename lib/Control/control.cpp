#include "control.h"
#include "controlconfiguration.h"
#include "pinDefinitions.h"
#include "Util.h"

//DRV8908 motor chip:
DRV8908 drv8908(DRV8908_MOSI_PIN, DRV8908_MISO_PIN, DRV8908_SCK_PIN, DRV8908_NSS_PIN, DRV8908_SLEEP_PIN, DRV8908_FAULT_PIN);

//Used to store new motor values, written to motors by update_motors function:
uint8_t motors[NR_OF_MOTORS];
Direction motorDirection[NR_OF_MOTORS];

//Currently pressed controls:
Controls controls;

ControlConfiguration configuration;

void update_motors() {
    // Check to see if its allowed to do :)

    // Checking if config needs to be rewritten:
    bool configChanged = false;

    //Setting the motor values in DRV8908 chip:
    for (int i = 0; i < NR_OF_MOTORS; i++) {
        //Skipping motor that does not change speed:
        if(drv8908.getMotor(i) == motors[i]) {
            continue;
        }

        configChanged = true;

        //When speed is zero, simply disable the motor:
        if(motors[i] == 0) {
            drv8908.disableMotor(i);
            continue;
        }

        //Clipping motor speed between 0 and max motor speed:
        motors[i] = MAX(0, MIN(motors[i], MAX_MOTOR_VAL));

        //Writing to the chip:
        drv8908.setMotor(i, motors[i], motorDirection[i]);
    }

    //Writing changes to the chip:
    if(configChanged) {
        drv8908.writeChanges();
    }
}

/// @brief Turn off all motors of the car:
void motors_off() {
    motors[0] = motors[1] = motors[2] = motors[3] = 0;

    update_motors();
}

//************************************************
//******** Control functions *********************
//************************************************

void adjustThrottle(int delta) {
    configuration.throttle += delta;

    //Bound between -100 and 100:
    configuration.throttle = configuration.throttle > 100 ? 100 : configuration.throttle;
    configuration.throttle = configuration.throttle < -100 ? -100 : configuration.throttle;
}

void adjustAngle(int delta) {
    configuration.angle += delta;

    //Bound between -100 and 100:
    configuration.angle = configuration.throttle > 100 ? 100 : configuration.throttle;
    configuration.angle = configuration.throttle < -100 ? -100 : configuration.throttle;
}

void startPivot(int degrees) {
    configuration.pivot = degrees;
}

//************************************************
//******** Initialisation + Operation check  *****
//************************************************

/// @brief Move the car a bit forward and than a bit backward.
/// @param comm The communication protocol.
void checkMotorMovement(Communication *comm) {
    comm->sendDebugMessage("Moving forward.....\n");
    
    motors[0] = motors[1] = motors[2] = motors[3] = 100;
    update_motors();
    thread_sleep_for(500);
    motors_off();

    comm->sendDebugMessage("Moving backward.....\n");

    setMotorDirectionAll(REVERSE);
    motors[0] = motors[1] = motors[2] = motors[3] = 100;
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
    motorDirection[motorId] = direction;
}

/// @brief Set the direction of all motors. 
/// @param direction The new direction of the motor, Forward, Reverse, Brake.
void setMotorDirectionAll(Direction direction) {
    motorDirection[0] = motorDirection[1] = motorDirection[2] = motorDirection[3] = direction;
}

//*****************************************
//******** Motor information retrieval ****
//*****************************************

uint8_t* getMotorSpeeds() {
    return motors;
}

Direction* getMotorDirections() {
    return motorDirection;
}

//*****************************************
//******** Motor processing ***************
//*****************************************

/// @brief Affect the motor behaviour based on the fact that the state of the system is Emergecy. Slowly slow down and stop the car.

void processEmergencyMode() {

    //If below turn medium, shut down motors:
    if(motors[0] <= MIN_MOTOR_VAL && motors[1] <= MIN_MOTOR_VAL && motors[2] <= MIN_MOTOR_VAL && motors[3] <= MIN_MOTOR_VAL) {
        motors_off();

        return;
    }

    //Decreasing motor speed by %:
    for (int i = 0; i < NR_OF_MOTORS; i++) {
        if(motors[i] >= MIN_MOTOR_VAL) {
            motors[i] --; 
        }
    }

    //printf("Motor val: %d\n", motors[0]);

    //Instead of this just increase throttle :/
}

void processManualMode() {
    //Setting motor directions:
    if(configuration.pivot > 0) {
        //Right motors backward left forward:
        setMotorDirection(MOTOR_FRONT_LEFT, FORWARD);
        setMotorDirection(MOTOR_BACK_LEFT, FORWARD);
        setMotorDirection(MOTOR_FRONT_RIGHT, REVERSE);
        setMotorDirection(MOTOR_BACK_RIGHT, REVERSE);

        //printf("Directions: %d, %d, %d, %d", motorDirection[0], motorDirection[1], motorDirection[2], motorDirection[3]);
    }
    else if(configuration.pivot < 0) {
        setMotorDirection(MOTOR_FRONT_LEFT, REVERSE);
        setMotorDirection(MOTOR_BACK_LEFT, REVERSE);
        setMotorDirection(MOTOR_FRONT_RIGHT, FORWARD);
        setMotorDirection(MOTOR_BACK_RIGHT, FORWARD);
    }
    else {
        setMotorDirectionAll(configuration.throttle < 0 ? REVERSE : FORWARD);
    }

    uint8_t motor_speed;

    //Set pivor speed:
    if(configuration.pivot != 0) {
        motor_speed = 255; //Guess?
    }
    else {
        uint8_t throttlePercentage = abs(configuration.throttle);
        motor_speed = MIN_MOTOR_VAL + (((MAX_MOTOR_VAL - MIN_MOTOR_VAL) / 100) * throttlePercentage);

        if(configuration.throttle == 0) {
            motor_speed = 0;
        }
    }

    //Too simple, setting motor speed:
    motors[0] = motors[1] = motors[2] = motors[3] = motor_speed;
}

/// @brief Process motor function, called from the main loop every ...ms.
/// @param currentState The current state of the system.
void processMotors(Statemachine::State currentState) {
    switch(currentState) {
        case Statemachine::State::IDLE:
            motors_off();
            break;
        case Statemachine::State::EMERGENCY:
            processEmergencyMode();
            break;
        case Statemachine::State::MANUAL:
            processManualMode();
            break;
        }

    update_motors();
}

void processControlCommand(Communication* comm, uint8_t command[]) {
    ControlCommandType type = (ControlCommandType) command[0];
    bool increase;
    uint8_t data;

    switch(type) {
        case RST:
            configuration = EmptyConfiguration;
            break;
        case CONTROLS:
            Controls newControls;

            newControls.forward = command[1];
            newControls.backward = command[2];
            newControls.left = command[3];
            newControls.right = command[4];
            newControls.pivotLeft = command[5];
            newControls.pivotRight = command[6];

            controls = newControls;
            break;
        case THROTTLE:
            increase = command[1];
            data = command[2];

            adjustThrottle(increase ? data : -data);
            break;
        case ANGLE:
            increase = command[1];
            data = command[2];

            adjustAngle(increase ? data : -data);
            break;
        case PIVOT:
            increase = command[1];
            data = command[2];

            startPivot(increase ? data : -data);
            break;
        }
}