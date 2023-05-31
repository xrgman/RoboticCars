#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "mbed.h"
#include "communication.h"

class Statemachine {
    public:
        typedef enum
        {
            IDLE,
            EMERGENCY,
            CALIBRATION,
            MANUAL,
            PLACE_HOLDER,
            NUMBER_OF_STATES
        } State;

        Statemachine(Callback<void(State, State)> state_changed_callback, Communication *communication);
        void changeState(State newState);
        State getCurrentState() { return getCurrentState(false); };
        State getCurrentState(bool print);

        static char const* StateToString(State state);
    private:
        State currentState;
        Callback<void(State, State)> state_changed_callback;
        Communication *communication;

        //Array containing data of which state changes are allowed, without considering enviromental conditions.
        //IDLE, EMERGENCY, CALIBRATION, MANUAL, DRIVING_BACKWARD
        bool allowedStateTransitions[State::NUMBER_OF_STATES][State::NUMBER_OF_STATES] = { //[Row][Column], [FROM][TO]
            {true,  true,   true,   true,   true}, //IDLE
            {true,  true,   false,  false,  false}, //EMERGENCY - Can only change to IDLE state, after shutdown procedure has completed.
            {true,  true,   true,   false,  false}, //CALIBRATION - After calibration the mode should always be changed back to IDLE
            {true,  true,   true,   true,   false}, //MANUAL
            {true,  true,   false,  false,  true} //DRIVING_BACKWARD
        };
};

#endif