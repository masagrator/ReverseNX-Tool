#pragma once

#include <string>
#include <vector>
#include "switch.h"
#include "State.h"

//Thanks shadowninja108 for the original state machine

class StateMachine
{
public:
    std::vector<State *> states;
    State *currentState;
    State *nextState;

    StateMachine();

    void Update(u64);

    void PushState(State *);
    void PushState(std::string);
    ~StateMachine();
};
