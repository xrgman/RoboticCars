#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "mbed.h"
#include "serialcomm.h"

#define SERIAL_TX USBTX
#define SERIAL_RX USBRX

typedef enum
{
    DEBUG = 0,
    TEST,
    MODE
} messageType;

class Communication {
    public:
        typedef enum
        {
            SERIAL,
            BLUETOOTH,
            WIFI
        } communicationState;

        Communication(communicationState communicationState);
        void initialize(Callback<void(messageType type, uint8_t size, uint8_t data[])> processed_data_callaback);
        void setCommunicationState(communicationState newState);
        void sendDebugMessage(const char *message);
        void sendMessage(messageType type, uint8_t size, uint8_t* data);
        
    private:
        SerialComm serialCommunication;
        communicationState currentCommState;
        Callback<void(messageType type, uint8_t size, uint8_t msg[])> processed_data_callaback;

        void sendByte(uint8_t byte);
        void processReceivedByte(uint8_t byte);

        // TODO: Desing protocol and use message length
        typedef enum
        {
            IDLE,    // State when not receiving a message
            TYPE,    // State wheb expecting the message type
            SIZE,    // State when expecting the size 
            DATA,    // State during the reception of characters (up until end char ('\n'))
            CHECKSUM
        } msgStatus;

        typedef struct
        {
            msgStatus status;
            messageType type;
            uint8_t data_size;
            uint8_t data[BUF_SIZE];
            uint8_t idx;
        } ReceivingData;

        ReceivingData receiving_data;

};

#endif