/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>

#include "mbed.h"
#include "sensor.h"
#include "serialcomm.h"
#include "drv8908.h"
#include "mpu9250.h"
#include "ak8963.h"

#define WAIT_TIME_MS 100

//Queue to enable printf in ISR:
EventQueue queue(32 * EVENTS_EVENT_SIZE);

DigitalOut led1(PG_2);
DigitalOut led2(PG_4);
DigitalOut led3(PG_3);
DigitalOut led4(PG_5);

//Uart communication:
SerialComm uart1(USBTX, USBRX);

//DRV8908 motor chip:
DRV8908 drv8908(DRV8908_MOSI_PIN, DRV8908_MISO_PIN, DRV8908_SCK_PIN, DRV8908_NSS_PIN, DRV8908_SLEEP_PIN, DRV8908_FAULT_PIN);

//MPU9250 IMU:
MPU9250 mpu9250(MPU9250_SCL, MPU9250_SDA, MPU9250_INT);

//AK8963 magnetometer:
AK8963 ak8963(AK8963_SCL, AK8963_SDA);

//I2C i2c(MPU9250_SDA, MPU9250_SCL);

void printFault() {
    printf("Printing out fault information:\n\n");
    drv8908.PrintErrorStatus();
}

void printTest() {
   // uint8_t address = 0x03;
    //uint16_t readMessage = DRV8908_READ_ADDRESS | (address << 8);
   
    //printf("Result: %x\n", readMessage);

    drv8908.Test();
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

void command_callback(uint8_t command[], uint8_t size) {
    // uart1.serial_connection.write("Received: ", 10);
    // uart1.serial_connection.write(command, size);
    // uart1.serial_connection.write("\n", 1);

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
}

void checkHardwareConnections() {
    printf("***** Starting hardware connectivity check *****\r\n");

    //MPU9250:
    printf("\n***** Checking sensor MPU9250 *****\r\n");
    mpu9250.CheckDeviceOperation();

    //AK8963:
    printf("\n***** Checking sensor AK8963 *****\r\n");
    ak8963.CheckDeviceOperation();

    //DRV8908:
    printf("\n***** Checking sensor DRV8908 *****\r\n");
    drv8908.CheckDeviceOperation();
}

int main()
{
    //Printing welcome message:
    printf("Robotic car - Running Mbed OS version %d.%d.%d\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    
    Thread eventThread2;
    eventThread2.start(callback(&queue, &EventQueue::dispatch_forever));

    //Intializing serial connection:
    uart1.initialize(command_callback);

    //Initializing DRV8908 chip:
    drv8908.Initialize();

    //Initializing MPU9250 chip:
    mpu9250.Initialize(MPU9250::GFS_250DPS, MPU9250::AFS_2G);

    //Initializig AK8963:
    ak8963.Initialize(AK8963::MFS_16BITS, AK8963::MOP_CONINUES_2);

    //Checking if all hardware is connected and functioning properly:
    checkHardwareConnections();

    while (true)
    {
        //drv8908.Test();
        //mpu9250.Test();
        //checkHardwareConnections();
        //mpu9250.PrintAllSensorReadings();

        // if(ak8963.IsDataReady()) {
        //     ak8963.PrintSensorReadings();
        // }

        drv8908.CheckDeviceOperation();

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
