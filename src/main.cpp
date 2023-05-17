/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>

#include "mbed.h"
#include "pinDefinitions.h"
#include "sensor.h"
#include "communication.h"
#include "drv8908.h"
#include "mpu9250.h"
#include "ak8963.h"
#include "hcsr04.h"

#define WAIT_TIME_MS 100

//Queue to enable printf in ISR:
EventQueue queue(32 * EVENTS_EVENT_SIZE);

DigitalOut led1(LED_1_PIN);
DigitalOut led2(LED_2_PIN);
DigitalOut led3(LED_3_PIN);
DigitalOut led4(LED_4_PIN);

//Communication:
Communication comm(Communication::SERIAL);

//DRV8908 motor chip:
DRV8908 drv8908(DRV8908_MOSI_PIN, DRV8908_MISO_PIN, DRV8908_SCK_PIN, DRV8908_NSS_PIN, DRV8908_SLEEP_PIN, DRV8908_FAULT_PIN);

//MPU9250 IMU:
MPU9250 mpu9250(MPU9250_SCL_PIN, MPU9250_SDA_PIN, MPU9250_INT_PIN);

//AK8963 magnetometer:
AK8963 ak8963(AK8963_SCL_PIN, AK8963_SDA_PIN);

//Ultrasonic sensors:
// HCSR04 ultrasonicFront(ULTRASONIC_FRONT_TRIGGER_PIN, ULTRASONIC_FRONT_ECHO_PIN, &comm);
// HCSR04 ultrasonicRight(ULTRASONIC_RIGHT_TRIGGER_PIN, ULTRASONIC_RIGHT_ECHO_PIN, &comm);

//I2C i2c(MPU9250_SDA, MPU9250_SCL);

void printFault() {
    printf("Printing out fault information:\n\n");
    drv8908.PrintErrorStatus();
}

void printTest() {
    comm.sendDebugMessage("HOOI\n");
    // drv8908.Test();
}

int ack;   
int address; 

// void scanI2C() {
//   for(address=1;address<127;address++) {    
//     ack = i2c.write(address << 1, NULL, 0);
//     if (ack == 0) {
//         printf("I2C device found at address 0x%02X (0x%02X in 8-bit)\n", address, address << 1);
//     }    
//     thread_sleep_for(0.05);
//   } 
// }

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
        default:
            break;
    }
}

void checkHardwareConnections() {
    comm.sendDebugMessage("***** Starting hardware connectivity check *****\r\n");

    //MPU9250:
    comm.sendDebugMessage("\n***** Checking sensor MPU9250 *****\r\n");
    mpu9250.CheckDeviceOperation();

    //AK8963:
    comm.sendDebugMessage("\n***** Checking sensor AK8963 *****\r\n");
    ak8963.CheckDeviceOperation();

    //DRV8908:
    comm.sendDebugMessage("\n***** Checking sensor DRV8908 *****\r\n");
    //drv8908.CheckDeviceOperation();
}

DigitalOut Trigger(ULTRASONIC_FRONT_TRIGGER_PIN);                                //Instance of the DigitalOut class called 'Trigger'
DigitalIn Echo(ULTRASONIC_FRONT_ECHO_PIN); 
Timer pulse; 

float GetDistance(){                                            //Function Name to be called
     int EchoPulseWidth=0,EchoStart=0,EchoEnd=0;                //Assign and set to zero the local variables for this function
     Trigger=1;                                                 //Signal goes High i.e. 3V3
     wait_us(10000);                                              //Wait 100us to give a pulse width
     Trigger=0;                                                 //Signal goes Low i.e. 0V
     pulse.reset();                                             //Rest the instance of the Timer Class
     while(Echo==0){
            if(EchoStart>=25000) {
                printf("H\n"); //never high :(
                break;
            }
                                                  // wait for Echo to go high
            EchoStart=pulse.read_us();                          //AND with timeout to prevent blocking!      
     }
     

     while(Echo==1&&(EchoEnd-EchoStart<25000)){                 //wait for echo to return low
        EchoEnd=pulse.read_us();                                //or'd with timeout to prevent blocking!   
     }

     EchoPulseWidth=EchoEnd-EchoStart;   
                            //time period in us
     return (float)EchoPulseWidth/5.8f;                         //calculate distance in mm and return the value as a float
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
    mpu9250.Initialize(MPU9250::GFS_250DPS, MPU9250::AFS_2G);

    //Initializig AK8963:
    ak8963.Initialize(AK8963::MFS_16BITS, AK8963::MOP_CONINUES_2);

    //Initializing DRV8908 chip:
    //drv8908.Initialize();

    //Checking if all hardware is connected and functioning properly:
    //checkHardwareConnections();

    //ultrasonicRight.startMeasurement();
     float Distance;                                            //Assign a local variable for the main function
     //pulse.start();

    while (true)
    {
        //drv8908.Test();
        //mpu9250.Test();
        //checkHardwareConnections();
        //mpu9250.PrintAllSensorReadings();

        // if(ak8963.IsDataReady()) {
        //     ak8963.PrintSensorReadings();
        // }

        //drv8908.CheckDeviceOperation();

        //ultrasonicFront.startMeasurement();
        //ultrasonicRight.startMeasurement();

        //while(!ultrasonicRight.isNewDataReady());
        

        //printf("Distances, front: %d cm, right: NO cm\n", ultrasonicFront.getDistanceCm());

        // Distance=GetDistance();                                 //Get the value returned from the function GetDistance()
        // printf("Distance %.0fmm\n\r",Distance);                 //Send the value to the serial port for monitoring purposes
        // thread_sleep_for(250);
        // printf("ON\n");
        // Trigger.write(1);
        // thread_sleep_for(5000);
        // printf("OFF\n");
        // Trigger.write(0);  
        // thread_sleep_for(5000);

        // led1 = !led1;
        // thread_sleep_for(WAIT_TIME_MS);
	    // led2 = !led2;
        // thread_sleep_for(WAIT_TIME_MS);
        // led3 = !led3;
        // thread_sleep_for(WAIT_TIME_MS);
        // led4 = !led4;
        // thread_sleep_for(WAIT_TIME_MS);
        // led4 = !led4;
        // thread_sleep_for(WAIT_TIME_MS);
        // led3 = !led3;
        // thread_sleep_for(WAIT_TIME_MS);
        // led2 = !led2;
        // thread_sleep_for(WAIT_TIME_MS);
        // led1 = !led1;
        // thread_sleep_for(WAIT_TIME_MS);
    }
}
