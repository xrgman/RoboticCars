#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <stdlib.h>
#include <string>
#include "mbed.h"

#define BUF_SIZE 50

//TODO: Desing protocol and use message length
typedef enum 
{ 
	IDLE, //State when not receiving a message
    START, //State after receiving start character ('?')
    RECEIVING //State during the reception of characters (up until end char ('\n'))
} msgStatus;

typedef struct {
    msgStatus status;
    uint8_t msg[BUF_SIZE];
	uint8_t idx;
} ReceivingData;

class SerialComm {
    public: 
        SerialComm(PinName rx, PinName tx);
        void initialize();
        void processReceivedCharacter(char c, Callback<void(uint8_t msg[], uint8_t size)> command_callback);

        //For now, should become private:
        UnbufferedSerial serial_connection; 
    private:
        PinName rxPin, txPin;
        ReceivingData receiving_data;

        

        //void on_rx_interrupt();
};


#endif //SERIALCOMM_H