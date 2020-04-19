#pragma once
#include "switch.h"
#include "../menu.h"
#include "../Utils.h"

class StateMachine;

class State
{
public:
    virtual void Enter(){};
    virtual void Update(StateMachine &stateMachine, u64 kDown) = 0;
    virtual void Exit(){};
    virtual std::string Name() = 0;
    virtual ~State(){};
};
