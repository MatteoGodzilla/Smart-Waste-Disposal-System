#pragma once
#include "Task.h"
#include "SMDSFiniteStateMachine.h"

class SleepTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
public:
    SleepTask(SMDSFiniteStateMachine* finiteStateMachine);
    virtual void execute() override;
};