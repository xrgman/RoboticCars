#include "communication.h"
#include "pinDefinitions.h"
#include "../ESP32/esp32_sketch/communicationDefinitions.cpp" //BAD but works for now :)


static Callback<void(RelayOver, uint8_t)> sendByteCallback;

/// @brief Constructor.
/// @param communicationState Set the default communication method upon initialisation.
Communication::Communication(CommunicationState communicationState) 
    : serialCommunication(SERIAL_TX_PIN, SERIAL_RX_PIN, 115200), 
        esp32BluetoothCommunication(ESP32_RX_PIN, ESP32_TX_PIN, 115200)  {
    currentCommState = communicationState;
}

/// @brief Initialize the communication protocol, automatically initializes all underlying ways of communication.
/// @param processed_data_callaback Callback to be called after deciphering the received data.
void Communication::initialize(void (*processed_data_callaback)(MessageType, RelayOver, uint8_t, uint8_t*)) {
    this->processed_data_callaback = processed_data_callaback;
    sendByteCallback = callback(this, &Communication::sendByte);

    serialCommunication.initialize(callback(this, &Communication::receivedByteSerial));
    esp32BluetoothCommunication.initialize(callback(this, &Communication::receivedByteBluetoothESP32));
}

/// @brief Check whether all connected communication devices are working
void Communication::checkDevicesOperation() {
    //Checking serial connection to app:
    send(PING, 0, NULL);

    

    //Waiting for ack :/
}

/// @brief Change the state of the communication protocol.
/// @param newState The new way of communicating.
void Communication::setCommunicationState(CommunicationState newState) {
    //Check whether communicationmethod is implemented: 
    switch(newState) {
        case SERIAL:
        case BLUETOOTH_ESP32:
            break;
        case WIFI_ESP32:
            sendDebugMessage("Communication state 'WIFI' is not implemented.\n");
            return;
        default:
            sendDebugMessage("Unable to change communication state, state does not exist.\n");
            return;
    }



    char msg[100];
	snprintf(msg, sizeof(msg), "Changing communication state to: %s\n", stateToString(newState));
    sendDebugMessage(msg);

    currentCommState = newState;
}

/// @brief Send data of the currently selected communication method, using the communication protocol.
/// @param type The message type, used to decipher the message.
/// @param size The size in bytes if the data. 
/// @param data The data to be send. 
void Communication::send(MessageType type, uint8_t size, uint8_t* data) {
    send(type, size, data, true);
}

//This is for testing the general function sendMessage :)
void sendByteWrapper(RelayOver relayOver, uint8_t byte) {
    printf("test\n");

    sendByteCallback(relayOver, byte);
}

/// @brief Send data of the currently selected communication method, using the communication protocol.
/// @param type The message type, used to decipher the message.
/// @param size The size in bytes if the data.
/// @param data The data to be send. 
/// @param relayMessage Whether the message should be relayed by the receiving device
void Communication::send(MessageType type, uint8_t size, uint8_t* data, bool relayMessage) {
    // Sending whether the message is for the ESP32 itself or if it should only relay it onto it's bluetooth line:
    //Should be done for all devices capable of this, like a pi and the onboard chip.
    RelayOver relay = !relayMessage                         ? NONE
                      : currentCommState == BLUETOOTH_ESP32 ? BLUETOOTH
                                                            : NONE;

    sendMessage(NONE, relay, type, size, data, [this](RelayOver r, uint8_t d) { return sendByte(r, d); });
}

/// @brief Helper method to send basic string message.
/// @param message The message to send of the current communication method.
void Communication::sendDebugMessage(const char* message) {
    //Determine size of message:
    int message_size = strlen(message);

    // Transform to uint8_t array:
    uint8_t* data = (uint8_t*) message;

    //Sending message using conventionel method:
    send(DEBUG, message_size, data);
}



void Communication::sendByte(RelayOver relayOver, uint8_t byte) {
    switch(currentCommState) {
        case SERIAL:
            serialCommunication.sendByte(byte);
            break;
        case BLUETOOTH_ESP32:
            esp32BluetoothCommunication.sendByte(byte);
            break;
        default:
            //Unimplemented :(
            break;
    }
}

void Communication::processed_data_callback_intermediate(MessageType type, RelayOver relayOver, uint8_t size, uint8_t * data) {
    //Filter ack messages :)
    if(type == ACK) {
            sendDebugMessage("Hoi ack msg\n");
    }

    //Passing on the callback:
    processed_data_callaback(type, relayOver, size, data);
}

void Communication::receivedByteSerial(uint8_t byte) {
    processReceivedByte(&receiving_data, byte, [this](MessageType type, RelayOver relayOver, uint8_t size, uint8_t * data) { return processed_data_callback_intermediate(type, relayOver, size, data); });
}

void Communication::receivedByteBluetoothESP32(uint8_t byte) {
    processReceivedByte(&receiving_data_bluetooth_ESP32, byte, [this](MessageType type, RelayOver relayOver, uint8_t size, uint8_t * data) { return processed_data_callback_intermediate(type, relayOver, size, data); });
}

char const* Communication::stateToString(CommunicationState state) {
    switch(state) {
        case SERIAL:
            return "Serial";
        case BLUETOOTH_ESP32:
            return "Bluetooth - ESP32";
        default:
            return "Unimplemented!";
    }
}