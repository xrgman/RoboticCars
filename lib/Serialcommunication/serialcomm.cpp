#include "serialcomm.h"

SerialComm::SerialComm(PinName tx, PinName rx, int baudrate) : serial_connection(tx, rx, baudrate)
{
    this->rxPin = rx;
    this->txPin = tx;
}

void SerialComm::initialize(Callback<void(uint8_t)> on_received) {
    this->on_received = on_received;

    serial_connection.attach(callback(this, &SerialComm::interuptReceived), SerialBase::RxIrq);
}

void SerialComm::sendByte(uint8_t byte) {
    serial_connection.write(&byte, 1);
}

void SerialComm::interuptReceived()
{
    //Read received data:
    uint8_t received;

    if(!serial_connection.read(&received, 1)) {
        return;
    }

    //Calling top level function to process received byte:
    on_received(received);
}
