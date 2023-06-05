#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "mbed.h"
#include "serialcomm.h"
//TODO even beter place to store this :)
#include "../ESP32/esp32_sketch/communicationDefinitions.h"

class Communication {
    public:
        typedef enum
        {
            SERIAL,
            BLUETOOTH_ESP32,
            WIFI_ESP32
        } CommunicationState;

        Communication(CommunicationState communicationState);
        void initialize(Callback<void(MessageType type, uint8_t size, uint8_t data[])> processed_data_callaback);
        void setCommunicationState(CommunicationState newState);
        void sendDebugMessage(const char *message);
        void sendMessage(MessageType type, uint8_t size, uint8_t* data);
        void sendMessage(MessageType type, uint8_t size, uint8_t* data, bool relayMessage);
        
    private:
        SerialComm serialCommunication;
        SerialComm esp32BluetoothCommunication;
        CommunicationState currentCommState;
        Callback<void(MessageType type, uint8_t size, uint8_t msg[])> processed_data_callaback;

        void sendByte(uint8_t byte);
        void processReceivedByte(uint8_t byte);
        char const* stateToString(CommunicationState state);

        ReceivingData receiving_data;
};

#endif