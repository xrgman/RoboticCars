#ifndef ESP32SERIAL_H
#define ESP32SERIAL_H

#include "mbed.h"
#include "communicationinterface.h"

class ESP32Serial : ICommunication {
    public:
        ESP32Serial(PinName rx, PinName tx);
        void initialize(Callback<void(uint8_t)> on_received);
        void sendByte(uint8_t byte);
        void interuptReceived();

    private:
        PinName rxPin, txPin;
        UnbufferedSerial serial_connection;
        Callback<void(uint8_t)> on_received;
};

#endif