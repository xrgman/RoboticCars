#ifndef COMMUNICATIONDEFINITIONS_H
#define COMMUNICATIONDEFINITIONS_H

#include "stdint.h"

#define COMM_BUF_SIZE 50

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
} ReceivingData;

#endif