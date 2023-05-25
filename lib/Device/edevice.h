#ifndef EDEVICE_H
#define EDEVICE_H

#include "communication.h"

class IEDevice {
    virtual void checkDeviceOperation(Communication *communication_protocol) = 0;
};

#endif