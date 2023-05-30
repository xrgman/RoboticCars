#ifndef SYSTEMTIMERS_H
#define SYSTEMTIMERS_H

#include "mbed.h"

#define TIMER_PERIOD 50ms //50ms 

extern uint32_t systemCounter;

void initializeTimers();
bool checkTimerFlag();
void clearTimerFlag();

#endif