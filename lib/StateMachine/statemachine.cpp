#include "statemachine.h"


Statemachine::Statemachine(Callback<void(State, State)> state_changed_callback, Communication *communication) {
    this->state_changed_callback = state_changed_callback;
    this->communication = communication;

    //Initializing in IDLE state:
    currentState = State::IDLE;
}

void Statemachine::changeState(State newState) {
    char msg[100];
    State oldState = currentState;

    //Checking if new state exists:
    if(newState >= NUMBER_OF_STATES) {
        snprintf(msg, sizeof(msg), "State with id %d does not exist.\n", newState);
        communication->sendDebugMessage(msg);

        return;
    }

    //Checking if new state is different from previous state:
    if(currentState == newState) {
        communication->sendDebugMessage("New state is the same as the current state, doing nothing.\n");
        return;
    }

    //Checking if state change is allowed:
    if(!allowedStateTransitions[currentState][newState]) {
		snprintf(msg, sizeof(msg), "Changing state %s to state %s is never allowed.\n", StateToString(currentState), StateToString(newState));
        communication->sendDebugMessage(msg);

        return;
    }

    //Changing the state:
    currentState = newState;

    //Perform actions based on the new state:
    state_changed_callback(oldState, newState);
}

Statemachine::State Statemachine::getCurrentState(bool print) {
    if(print) {
        char msg[100];
	    snprintf(msg, sizeof(msg), "Current state: %s.\n", StateToString(currentState));
        communication->sendDebugMessage(msg);
    }
    
    return currentState;
}

char const * Statemachine::StateToString(State state) {
    switch(state) {
        case IDLE:
            return "IDLE";
        case EMERGENCY:
            return "Emergency";
        case CALIBRATION:
            return "CALIBRATION";
        case DRIVING_FORWARD:
            return "DRIVING_FORWARD";
        case DRIVING_BACKWARD:
            return "DRIVING_BACKWARD";
        default:
            return "Not defined";
    }
}