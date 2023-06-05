#ifndef CONTROL_CONFIGURATION_H
#define CONTROL_CONFIGURATION_H

#include "mbed.h"

typedef struct {
    int throttle; // Value between -100 and 100;
    int angle;
    int pivot;
    
} ControlConfiguration;

static const ControlConfiguration EmptyConfiguration {
    0
};

#endif