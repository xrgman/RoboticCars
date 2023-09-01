#include "communicationDefinitions.h"

//TODO error callback function for checksum :)

/// @brief Receive and unpack the communication protocol, automatically called by interrupt.
/// @param receiving_data Object that stores the receiving progress.
/// @param r The received byte.
/// @param callback Function called after complete message is retreived.
void processReceivedByte(ReceivingData *receiving_data, uint8_t r, std::function<void(MessageType, RelayOver, uint8_t, uint8_t*)> callback) {
    // Detecting start character:
    if (receiving_data->status == MessageStatus::IDLE && r == '?') {
        receiving_data->status = MessageStatus::RELAY;
        receiving_data->checksum ^= r;
    }
    //Retreiving relay over type:
    else if(receiving_data->status == MessageStatus::RELAY) {
        receiving_data->relayOver = (RelayOver) (r); 
        receiving_data->status = MessageStatus::TYPE;
        receiving_data->checksum ^= r;
    }
    //Retreiving the message type:
    else if(receiving_data->status == MessageStatus::TYPE) {
        receiving_data->type = (MessageType) (r); 
        receiving_data->status = MessageStatus::SIZE;
        receiving_data->checksum ^= r;
    }
    //Retreiving the data length:
    else if(receiving_data->status == MessageStatus::SIZE) {
        receiving_data->data_size = r; 
        receiving_data->status = r == 0 ? MessageStatus::CHECKSUM : MessageStatus::DATA;
        receiving_data->checksum ^= r;
    }
    //Retreiving the data:
    else if(receiving_data->status == MessageStatus::DATA) {
        receiving_data->data[receiving_data->idx] = r;
        receiving_data->idx++;
        receiving_data->checksum ^= r;

        if(receiving_data->idx >= receiving_data->data_size) {
            receiving_data->status = MessageStatus::CHECKSUM;
        }
    }
    //Checking checksum:
    else if(receiving_data->status == MessageStatus::CHECKSUM) {
        if(receiving_data->checksum == r) {
            //Processing message:
            callback(receiving_data->type, receiving_data->relayOver, receiving_data->data_size, receiving_data->data);
        }
        else {
            //Error?
        }

        //Resetting buffer:
        receiving_data->status = MessageStatus::IDLE;
        receiving_data->idx = 0;
        receiving_data->checksum = 0;
    }
}

bool ackReceived = false;

bool sendMessage(RelayOver sendByteOver, RelayOver relayOver, MessageType type, uint8_t size, uint8_t *data, std::function<void(RelayOver, uint8_t)> sendByte, bool waitForAck, std::function<void(uint32_t)> delay) {
    //Sending message:
    sendMessage(sendByteOver, relayOver, type, size, data, sendByte);

    //Wait till ack message is returned or time out
    if(waitForAck) {
        //TODO timeout
        while(!ackReceived) {
            delay(5);

            
        }
    }

    return true;
}

/// @brief Send a message using the defined protocol.
/// @param sendByteOver Passed to the sendByte function
/// @param relayOver Used at the receiver to relay the message if applicable
/// @param type The message type
/// @param size Size of the message data
/// @param data The payload of the message
/// @param sendByte The function that handles the actual sending of the byte
void sendMessage(RelayOver sendByteOver, RelayOver relayOver, MessageType type, uint8_t size, uint8_t *data, std::function<void(RelayOver, uint8_t)> sendByte) {
    uint8_t checkSum = 0;

    //Sending start byte:
    sendByte(sendByteOver, '?');
    checkSum ^= '?';

    //Sending relay over byte:
    sendByte(sendByteOver, relayOver);
    checkSum ^= relayOver;

    //Sending message type:
    sendByte(sendByteOver, type);
    checkSum ^= type;

    //Sending message size:
    sendByte(sendByteOver, size);
    checkSum ^= size;
    
    //Sending data:
    if(data != NULL) {
      for (int i = 0; i < size; i++) {
          sendByte(sendByteOver, data[i]);
          checkSum ^= data[i];
      }
    }

    //Sending checksum:
    sendByte(sendByteOver, checkSum);
}
