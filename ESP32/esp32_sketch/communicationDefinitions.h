#ifndef COMMUNICATIONDEFINITIONS_H
#define COMMUNICATIONDEFINITIONS_H

#include <functional>
#include "stdlib.h"
#include "stdint.h"

#define COMM_BUF_SIZE 250

typedef enum
{
    DEBUG = 0,
    TEST,
    MODE,
    CONTROL,
    PING,
    ACK
} MessageType;

typedef enum
{
    NONE,
    SERIAL_WIRE,
    SERIAL_WIRE_STM32,
    BLUETOOTH,
    WIFI
} RelayOver;

 typedef enum
{
    IDLE,    // State when not receiving a message
    RELAY,   // State when to expect relay over type
    TYPE,    // State wheb expecting the message type
    SIZE,    // State when expecting the size 
    DATA,    // State during the reception of characters (up until end char ('\n'))
    CHECKSUM
} MessageStatus;

typedef struct
{
    MessageStatus status;
    RelayOver relayOver;
    MessageType type;
    uint8_t data_size;
    uint8_t data[COMM_BUF_SIZE];
    uint8_t idx;
    uint8_t checksum;
} ReceivingData;

extern void processReceivedByte(ReceivingData *receiving_data, uint8_t r, std::function<void(MessageType, RelayOver, uint8_t, uint8_t*)>);
extern void sendMessage(RelayOver sendByteOver, RelayOver relayOver, MessageType type, uint8_t size, uint8_t *data, std::function<void(RelayOver, uint8_t)>);


#endif