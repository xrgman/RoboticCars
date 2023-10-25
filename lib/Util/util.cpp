#include "util.h"

void Util::printAsBinary(int number) {
    uint32_t n = 0b1000;

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

void Util::scanForI2CDevices(I2C *i2c) {
    int ack;   
    int address;

    for(address=1;address<127;address++) {    
        ack = i2c->write(address << 1, NULL, 0);

        if (ack == 0) {
            // char msg[55];
            // snprintf(msg, sizeof(msg), "I2C device found at address 0x%02X (0x%02X in 8-bit)\n", address, address << 1);
            // communication_protocol->sendDebugMessage(msg);
            printf("I2C device found at address 0x%02X (0x%02X in 8-bit)\n", address, address << 1);
        }    

        thread_sleep_for(0.05);
    }

    printf("Scanning I2C devices done\n");

    //communication_protocol->sendDebugMessage("Scanning I2C devices done.\n");
}

uint8_t Util::getBitsFromData(uint8_t data, uint8_t start, uint8_t end) {
    //Shift bits into position:
    data = data >> (7 - end);

    //Removing leading bits:
    data &= (1 << (end-start+1)) - 1;

    return data;
}

uint16_t Util::getBitsFromData(uint16_t data, uint8_t start, uint8_t end) {
    //Shift bits into position:
    data = data >> (15 - end);

    //Removing leading bits:
    data &= (1 << (end-start+1)) - 1;

    return data;
}

uint32_t Util::getBitsFromData(uint32_t data, uint8_t start, uint8_t end) {
    //Shift bits into position:
    data = data >> (31 - end);

    //Removing leading bits:
    data &= (1 << (end-start+1)) - 1;

    return data;
}

/// @brief Set one or more bits in a binary number to a specific value.
/// @param data Binary data to be changed.
/// @param start Start position, seen from the LSB.
/// @param nrOfBits Number of bits up from the start bit to change.
/// @param value New value for all of the bits that need to be changed. 
bool Util::setBits(uint16_t *data, uint8_t start, uint8_t nrOfBits, uint8_t value) {
    if(start > 15 || start+nrOfBits > 16 || value > 0xFFFF) {
        return false;
    }

    uint16_t maskBits = pow(2, nrOfBits) - 1;
    uint16_t mask = maskBits << start;

    *data = ((*data & ~mask) | (value << start));

    return true;
}

uint16_t Util::to_ui16(char *pData) 
{ 
	uint16_t ret = 0;
	ret |= (pData[0] << 8);
	ret |= pData[1];
	return ret;
}

void Util::delay(uint32_t duration) {
    printf("TESTDELAY\n");

    thread_sleep_for(duration);
}