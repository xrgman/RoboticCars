#include "communication.h"
#include "pinDefinitions.h"

/// @brief Constructor.
/// @param communicationState Set the default communication method upon initialisation.
Communication::Communication(communicationState communicationState) : serialCommunication(SERIAL_TX_PIN, SERIAL_RX_PIN) {
    currentCommState = communicationState;
}

/// @brief Initialize the communication protocol, automatically initializes all underlying ways of communication.
/// @param processed_data_callaback Callback to be called after deciphering the received data.
void Communication::initialize(Callback<void(messageType type, uint8_t size, uint8_t data[])> processed_data_callaback) {
    this->processed_data_callaback = processed_data_callaback;

    //callback(this, Communication::ProcessReceivedByte)
    serialCommunication.initialize(callback(this, &Communication::processReceivedByte));
}

/// @brief Change the state of the communication protocol.
/// @param newState The new way of communicating.
void Communication::setCommunicationState(communicationState newState) {
    //Check whether communicationmethod is implemented: 
    switch(newState) {
        case BLUETOOTH:
            sendDebugMessage("Communication state 'BLUETOOTH' is not implemented.\n");
            break;
        case WIFI:
            sendDebugMessage("Communication state 'WIFI' is not implemented.\n");
            return;
        default:
            sendDebugMessage("Unable to change communication state, state does not exist.\n");
            return;
    }
}

/// @brief Send data of the currently selected communication method, using the communication protocol.
/// @param type The message type, used to decipher the message.
/// @param size The size in bytes if the data.
/// @param data The data to be send. 
void Communication::sendMessage(messageType type, uint8_t size, uint8_t* data) {
    uint8_t checkSum = 0;

    if(type != DEBUG) {
        //Sending start characther:
        sendByte('?');
        checkSum ^= '?';

        //Sending message type:
        sendByte(type);
        checkSum ^= type;

        //Sending message size:
        sendByte(size);
        checkSum ^= size;
    }

    //Sending data:
    for (int i = 0; i < size; i++) {
        sendByte(data[i]);
        checkSum ^= data[i];
    }

    if(type != DEBUG) {
        //Sending checksum:
        sendByte(checkSum);
    }
}

/// @brief Helper method to send basic string message.
/// @param message The message to send of the current communication method.
void Communication::sendDebugMessage(const char* message) {
    //Determine size of message:
    int message_size = strlen(message);

    // Transform to uint8_t array:
    uint8_t* data = (uint8_t*) message;

    //Sending message using conventionel method:
    sendMessage(DEBUG, message_size, data);
}

void Communication::sendByte(uint8_t byte) {
    switch(currentCommState) {
        case SERIAL:
            serialCommunication.sendByte(byte);
            break;
        default:
            //Unimplemented :(
            break;
    }
}

/// @brief Receive and unpack the communication protocol, automatically called by interrupt.
/// @param byte The received byte.
void Communication::processReceivedByte(uint8_t byte) {
    //Detecting start character:
    if (receiving_data.status == msgStatus::IDLE && byte == '?') {
        receiving_data.status = msgStatus::TYPE;
    }
    //Retreiving the message type:
    else if(receiving_data.status == msgStatus::TYPE) {
        receiving_data.type = (messageType) (byte - 48); //TODO remove -48 once manual testing is over :)
        receiving_data.status = msgStatus::SIZE;
    }
    //Retreiving the data length:
    else if(receiving_data.status == msgStatus::SIZE) {
        receiving_data.data_size = byte - 48; //TODO remove -48 once manual testing is over :)
        receiving_data.status = msgStatus::DATA;
    }
    //Retreiving the data:
    else if(receiving_data.status == msgStatus::DATA) {
        //TODO some things :)
        if(byte == '\n') {
            // Printing received message
            char msg[28+receiving_data.data_size];
            snprintf(msg, sizeof(msg), "Type: %d, size: %d, data: %s.\n", receiving_data.type, receiving_data.data_size, receiving_data.data);
            sendDebugMessage(msg);

            //Processing message:
            processed_data_callaback(receiving_data.type, receiving_data.data_size, receiving_data.data);

            //Resetting buffer:
            receiving_data.status = msgStatus::IDLE;
            receiving_data.idx = 0;

            return;
        }
        
        receiving_data.data[receiving_data.idx] = byte;
        receiving_data.idx++;
    }
}