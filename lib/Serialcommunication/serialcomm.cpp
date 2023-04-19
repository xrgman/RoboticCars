#include "serialcomm.h"


/*
    Called on RX received thingy :)
*/
// void on_rx_interrupt() 
// {
//     char c;

//     // Read the data to clear the receive interrupt.
//     // if (SerialComm::serial_connection->read(&c, 1)) {
//     //     // Echo the input back to the terminal.
//     //     SerialComm::serial_connection->write(&c, 1);
//     // }
// }

void SerialComm::processReceivedCharacter(char c, Callback<void(uint8_t msg[], uint8_t size)> command_callback)
{
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

    //this->serial_connection->write(&c, 1);
}

void SerialComm::initialize()
{
    //TODO: fix this
    //this->serial_connection->attach(&on_rx_interrupt, SerialBase::RxIrq);
}

SerialComm::SerialComm(PinName tx, PinName rx) : serial_connection(tx, rx)
{
    this->rxPin = rx;
    this->txPin = tx;
}
