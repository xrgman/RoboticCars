#ifndef COMMUNICATIONINTERFACE_H
#define COMMUNICATIONINTERFACE_H

class ICommunication {
    virtual void sendByte(uint8_t byte) = 0;
    //virtual void sendMessage(const char *message) = 0;
};

#endif