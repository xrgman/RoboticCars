#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <stdlib.h>
#include <string>
#include "mbed.h"
#include "communicationinterface.h"

class SerialComm : ICommunication {
    public: 
        SerialComm(PinName rx, PinName tx, int baudrate);
        void initialize(Callback<void(uint8_t)> on_received);
        void sendByte(uint8_t byte);
        void interuptReceived();

    private:
        PinName rxPin, txPin;
        UnbufferedSerial serial_connection; 
        Callback<void(uint8_t)> on_received;
};

#endif //SERIALCOMM_H