/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>

#include "mbed.h"
#include "pinDefinitions.h"
#include "statemachine.h"
#include "sensor.h"
#include "communication.h"
#include "drv8908.h"
#include "mpu9250.h"
#include "ak8963.h"
#include "hcsr04.h"

#define WAIT_TIME_MS 100

//Function definitions:
void state_changed_callback(Statemachine::State, Statemachine::State);

//Queue to enable printf in ISR:
EventQueue queue(32 * EVENTS_EVENT_SIZE);

//The four leds of the robot:
DigitalOut led1(LED_1_PIN);
DigitalOut led2(LED_2_PIN);
DigitalOut led3(LED_3_PIN);
DigitalOut led4(LED_4_PIN);

//Communication:
Communication comm(Communication::SERIAL);

//Statemachine:
Statemachine statemachine(state_changed_callback, &comm);

//DRV8908 motor chip:
DRV8908 drv8908(DRV8908_MOSI_PIN, DRV8908_MISO_PIN, DRV8908_SCK_PIN, DRV8908_NSS_PIN, DRV8908_SLEEP_PIN, DRV8908_FAULT_PIN);

//MPU9250 IMU:
MPU9250 mpu9250(MPU9250_SCL_PIN, MPU9250_SDA_PIN, MPU9250_INT_PIN);

//AK8963 magnetometer:
AK8963 ak8963(AK8963_SCL_PIN, AK8963_SDA_PIN);

//Ultrasonic sensors:
HCSR04 ultrasonicFront(ULTRASONIC_FRONT_TRIGGER_PIN, ULTRASONIC_FRONT_ECHO_PIN, &comm);
HCSR04 ultrasonicRight(ULTRASONIC_RIGHT_TRIGGER_PIN, ULTRASONIC_RIGHT_ECHO_PIN, &comm);
HCSR04 ultrasonicLeft(ULTRASONIC_LEFT_TRIGGER_PIN, ULTRASONIC_LEFT_ECHO_PIN, &comm);

void printFault() {
    printf("Printing out fault information:\n\n");
    drv8908.printErrorStatus();
}

void printTest() {
    comm.sendDebugMessage("HOOI\n");
    // drv8908.Test();
}

void command_callback(messageType type, uint8_t size, uint8_t command[]) {
    switch(type) {
        case TEST: //Used for printing test information:
            if(size >= 1) {
                switch(command[0]) {
                    case 'p':
                        queue.call(printFault);
                        break;
                    case 't':
                        queue.call(printTest);
                        break;
                    default:
                        break;
                }
            }
            break;
        case MODE: {
            //Extracting state to change into:
            Statemachine::State newState = (Statemachine::State)(command[0]-48); //TO-DO remove when typing testing is done :)
            
            //Changing the state:
            statemachine.changeState(newState);

            break;
        }
        default:
            break;
    }
}

/// @brief Called when state is changed, can be used to execute certain actions based on the previous and new states.
/// @param oldState State before the change.
/// @param newState State after the change.
void state_changed_callback(Statemachine::State oldState, Statemachine::State newState) {
    //In emergency state stop all motors :)

    //Printing state change:
    char msg[100];
	snprintf(msg, sizeof(msg), "State changed successfully from state %s to state %s.\n", Statemachine::StateToString(oldState), Statemachine::StateToString(newState));
    comm.sendDebugMessage(msg);
}

void checkHardwareConnections() {
    comm.sendDebugMessage("\n***** Starting hardware connectivity check *****\r\n");

    //MPU9250:
    comm.sendDebugMessage("\n***** Checking sensor MPU9250 *****\r\n");
    mpu9250.checkDeviceOperation(&comm);

    //AK8963:
    comm.sendDebugMessage("\n***** Checking sensor AK8963 *****\r\n");
    ak8963.checkDeviceOperation(&comm);

    //DRV8908:
    comm.sendDebugMessage("\n***** Checking sensor DRV8908 *****\r\n");
    drv8908.checkDeviceOperation(&comm);
}


//NULL IS NC :)
int main()
{
    //Printing welcome message:
    printf("Robotic car - Running Mbed OS version %d.%d.%d\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    
    Thread eventThread2;
    eventThread2.start(callback(&queue, &EventQueue::dispatch_forever));

    //Intializing serial connection:
    comm.initialize(command_callback);

    //Initializing MPU9250 chip:
    mpu9250.initialize(MPU9250::GFS_250DPS, MPU9250::AFS_2G);

    //Initializig AK8963:
    ak8963.initialize(AK8963::MFS_16BITS, AK8963::MOP_CONINUES_2);

    //Initializing DRV8908 chip:
    drv8908.initialize();

    // Configuring motors (TO-DO move to seperate class):
    //P16 -> M3 -> OUT5 (m3+), OUT6 (m3-)
    drv8908.configureMotor(0, HALF_BRIDGE_5, HALF_BRIDGE_6, PWM_CHANNEL_1, 10);

    drv8908.setMotor(0, 50, FORWARD);
    drv8908.writeChanges();
    printf("Motor shit applied\n");

    //Checking if all hardware is connected and functioning properly:
    //checkHardwareConnections();

    while (true)
    {
            //drv8908.test();
            // drv8908.printErrorStatus();
            //  mpu9250.PrintAllSensorReadings();

            // if(ak8963.IsDataReady()) {
            //     ak8963.PrintSensorReadings();
            // }

            // HCSR04 test stuff:
            // ultrasonicRight.startMeasurement();
            // distance = ultrasonicRight.getDistanceCmBlocking();
            // while(!ultrasonicRight.isNewDataReady());
            // printf("Distances, front: NO cm, right: %d cm\n", ultrasonicRight.getDistanceCm());

            // printf("Distance %.0fmm\n\r", distance);                 //Send the value to the serial port for monitoring purposes

            led1 = !led1;
            thread_sleep_for(WAIT_TIME_MS);
            led2 = !led2;
            thread_sleep_for(WAIT_TIME_MS);
            led3 = !led3;
            thread_sleep_for(WAIT_TIME_MS);
            led4 = !led4;
            thread_sleep_for(WAIT_TIME_MS);
            led4 = !led4;
            thread_sleep_for(WAIT_TIME_MS);
            led3 = !led3;
            thread_sleep_for(WAIT_TIME_MS);
            led2 = !led2;
            thread_sleep_for(WAIT_TIME_MS);
            led1 = !led1;
            thread_sleep_for(WAIT_TIME_MS);
    }
}
