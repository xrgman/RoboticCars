#include "timers.h"
// #include "main.h"

Ticker ticker;
static bool timer_flag;

uint32_t systemCounter = 0;

bool checkTimerFlag() {
    return timer_flag;
}

void clearTimerFlag() {
    timer_flag = false;
}

void mainLoopTimerHandler()
{
	//systemCounter++;
    systemCounter++;

    timer_flag = true;
}

void initializeTimers() {
    timer_flag = false;

    //Attaching function to ticker:
    ticker.attach(&mainLoopTimerHandler, TIMER_PERIOD);
}