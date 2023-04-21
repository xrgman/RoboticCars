#include "serialcomm.h"

void on_rx_interrupt() {
    serial_conn->processReceivedCharacter();
}

SerialComm::SerialComm(PinName tx, PinName rx) : serial_connection(tx, rx)
{
    this->rxPin = rx;
    this->txPin = tx;

    serial_conn = this;
}

void SerialComm::initialize(Callback<void(uint8_t msg[], uint8_t size)> command_callback)
{
    this->command_callback = command_callback;

    serial_connection.attach(&on_rx_interrupt, SerialBase::RxIrq);
}




void SerialComm::processReceivedCharacter()
{
    //Retreiving character:
    char c;

    // Read the data to clear the receive interrupt.
    if (!serial_connection.read(&c, 1)) {
       return;
    }

    if(receiving_data.status == msgStatus::IDLE && c == '?') {
        receiving_data.status = msgStatus::RECEIVING;
    }
    else if(receiving_data.status == msgStatus::RECEIVING) {
        if(c == '\n') {
            //Processing message:
            // serial_connection.write("Received: ", 10);
            // serial_connection.write(receiving_data.msg, receiving_data.idx);
            // serial_connection.write("\n", 1);

            command_callback(receiving_data.msg, receiving_data.idx);

            //Resetting buffer:
            receiving_data.status = msgStatus::IDLE;
            receiving_data.idx = 0;

            return;
        }
        
        receiving_data.msg[receiving_data.idx] = c;
        receiving_data.idx++;
    }
}

