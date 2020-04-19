#pragma once
#include "StateMachine.h"

class ErrorMenu : public State
{
    virtual void Update(StateMachine &, u64);
    virtual std::string Name() { return "error"; }

public:
    static Result error;
};
