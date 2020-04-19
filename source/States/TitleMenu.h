#pragma once
#include "StateMachine.h"

class ReverseNXMenu : public State
{
public:
    virtual void Update(StateMachine &, u64);
    virtual std::string Name() { return "title"; }

private:
    int selection = 0;
    int titlePage = 0;
    int onScreenItems = 0;
};
