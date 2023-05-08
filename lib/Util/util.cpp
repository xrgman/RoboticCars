#include "util.h"

void Util::PrintAsBinary(int number) {
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