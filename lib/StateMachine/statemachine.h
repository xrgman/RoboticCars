#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "mbed.h"

//TO-DO: Create table with allowed state changes :)
//From in rows and to in columns, and use binary.



class Statemachine {
    public:
        typedef enum
        {
            IDLE,
            EMERGENCY,
            CALIBRATION,
            DRIVING_FORWARD,
            DRIVING_BACKWARD,
            NUMBER_OF_STATES
        } State;

        Statemachine(Callback<void(State, State)> state_changed_callback);
        void changeState(State newState);
        State getCurrentState();

    private:
        State currentState;
        Callback<void(State, State)> state_changed_callback;

        //Array containing data of which state changes are allowed, without considering enviromental conditions.
        //IDLE, EMERGENCY, CALIBRATION, DRIVING_FORWARD, DRIVING_BACKWARD
        bool allowedStateTransitions[State::NUMBER_OF_STATES][State::NUMBER_OF_STATES] = { //[Row][Column], [FROM][TO]
            {true,  true,   true,   true,   true}, //IDLE
            {true,  true,   false,  false,  false}, //EMERGENCY - Can only change to IDLE state, after shutdown procedure has completed.
            {true,  true,   true,   false,  false}, //CALIBRATION - After calibration the mode should always be changed back to IDLE
            {true,  true,   true,   true,   false}, //DRIVING_FORWARD
            {true,  true,   false,  false,  true}, //DRIVING_BACKWARD
        };
};

#endif