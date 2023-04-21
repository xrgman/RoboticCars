/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>

#include "mbed.h"
#include "serialcomm.h"
#include "drv8908registers.h"
#include "drv8908.h"

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

void printFault() {
    printf("Printing out fault information:\n\n");
    drv8908.PrintErrorStatus();
}

void printTest() {
    uint8_t address = 0x03;
    uint16_t readMessage = DRV8908_READ_ADDRESS | (address << 8);
   
    //printf("Result: %x\n", readMessage);

    drv8908.Test();
}

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

    while (true)
    {
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
