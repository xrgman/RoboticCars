#ifndef MAIN_H
#define MAIN_H

#include "pinDefinitions.h"
#include "communication.h"
#include "respeaker6MicArray.h"
#include "../lib/SDWrapper/sdwrapper.h" //Bullshit....

// Communication:
extern Communication comm;

// Respeaker:
extern Respeaker6MicArray respeaker;

// SD card wrapper:
extern SDWrapper sdWrapper;

#endif