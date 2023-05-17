#include "statemachine.h"


Statemachine::Statemachine(Callback<void(State, State)> state_changed_callback) {
    this->state_changed_callback = state_changed_callback;

    //Initializing in IDLE state:
    currentState = State::IDLE;
}

void Statemachine::changeState(State newState) {

}

Statemachine::State Statemachine::getCurrentState() {
    printf("Test: %d\n", allowedStateTransitions[1][2]);

    return currentState;
}