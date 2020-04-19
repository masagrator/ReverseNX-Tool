#include "StateMachine.h"

//Thanks shadowninja108 for the original state machine

StateMachine::StateMachine()
{
    currentState = NULL;
    nextState = NULL;
}

void StateMachine::Update(u64 inputs)
{
    if (nextState != NULL)
    {
        if (currentState != NULL)
            currentState->Exit();

        currentState = nextState;
        nextState = NULL;

        currentState->Enter();
    }

    if (currentState == NULL)
    {
        consoleClear();
        printf("State machine has no current state!");
        return;
    }

    currentState->Update(*this, inputs);
}

void StateMachine::PushState(State *state)
{
    nextState = state;
}

void StateMachine::PushState(std::string str)
{
    for (auto &i : states)
    {
        if (i->Name() == str)
        {
            nextState = i;
            return;
        }
    }
}

StateMachine::~StateMachine()
{
    for (auto &i : states)
        delete i;
}
