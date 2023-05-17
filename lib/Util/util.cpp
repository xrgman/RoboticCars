#include "util.h"

void Util::printAsBinary(int number) {
    uint32_t n;

    if(number <= 0b1111) {
        n = 0b1000;
    }
    else if(number <= 0b11111111) {
        n = 0b10000000;
    }
    else if(number <= 0b111111111111) {
        n = 0b100000000000;
    }
    else if(number <= 0b1111111111111111) {
        n = 0b1000000000000000;
    }
    else if(number <= 0b11111111111111111111) {
        n = 0b10000000000000000000;
    }
    else if(number <= 0b111111111111111111111111) {
        n = 0b100000000000000000000000;
    }
    else if(number <= 0b1111111111111111111111111111) {
        n = 0b1000000000000000000000000000;
    }

    while(n > 0) {
        int r = number - n;

        //printf("Number: %d, n: %d, r: %d\n", number, n, r);

        n >>= 1;

        if(r < 0) {
            printf("0");
        }
        else {
            printf("1");

            number = r;
        }  
    }

    printf("\r\n");
}

void Util::scanForI2CDevices(PinName sda, PinName scl) {
    int ack;   
    int address;
    I2C i2c(sda, scl);

    for(address=1;address<127;address++) {    
        ack = i2c.write(address << 1, NULL, 0);

        if (ack == 0) {
            printf("I2C device found at address 0x%02X (0x%02X in 8-bit)\n", address, address << 1);
        }    

        thread_sleep_for(0.05);
    }   
}