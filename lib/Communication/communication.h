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
        void initialize(void (*processed_data_callaback)(MessageType, RelayOver, uint8_t, uint8_t*));
        void checkDevicesOperation();
        void setCommunicationState(CommunicationState newState);
        void sendDebugMessage(const char *message);
        void send(MessageType type, uint8_t size, uint8_t* data);
        void send(MessageType type, uint8_t size, uint8_t* data, bool relayMessage);

    private:
        SerialComm serialCommunication;
        SerialComm esp32BluetoothCommunication;
        CommunicationState currentCommState;
        void (*processed_data_callaback)(MessageType, RelayOver, uint8_t, uint8_t *);

        void sendByte(RelayOver relayOver, uint8_t byte);
        void receivedByteSerial(uint8_t byte);
        void receivedByteBluetoothESP32(uint8_t byte);
        void processed_data_callback_intermediate(MessageType, RelayOver, uint8_t, uint8_t *);
        char const *stateToString(CommunicationState state);

        ReceivingData receiving_data;
        ReceivingData receiving_data_bluetooth_ESP32;
};

#endif