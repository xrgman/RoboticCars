#include "communicationDefinitions.h"

/// @brief Receive and unpack the communication protocol, automatically called by interrupt.
/// @param receiving_data Object that stores the receiving progress.
/// @param r The received byte.
/// @param callback Function called after complete message is retreived.
void processReceivedByte(ReceivingData *receiving_data, uint8_t r, void (*callback)(MessageType, RelayOver, uint8_t, uint8_t*)) {
    // Detecting start character:
    if (receiving_data->status == MessageStatus::IDLE && r == '?') {
        receiving_data->status = MessageStatus::RELAY;
    }
    //Retreiving relay over type:
    else if(receiving_data->status == MessageStatus::RELAY) {
        receiving_data->relayOver = (RelayOver) (r); 
        receiving_data->status = MessageStatus::TYPE;
    }
    //Retreiving the message type:
    else if(receiving_data->status == MessageStatus::TYPE) {
        receiving_data->type = (MessageType) (r); 
        receiving_data->status = MessageStatus::SIZE;
    }
    //Retreiving the data length:
    else if(receiving_data->status == MessageStatus::SIZE) {
        receiving_data->data_size = r; 
        receiving_data->status = MessageStatus::DATA;
    }
    //Retreiving the data:
    else if(receiving_data->status == MessageStatus::DATA) {
        receiving_data->data[receiving_data->idx] = r;
        receiving_data->idx++;

        if(receiving_data->idx >= receiving_data->data_size) {
            
            //Processing message:
            callback(receiving_data->type, receiving_data->relayOver, receiving_data->data_size, receiving_data->data);

            //Resetting buffer:
            receiving_data->status = MessageStatus::IDLE;
            receiving_data->idx = 0;

            return;
        }
    }
}