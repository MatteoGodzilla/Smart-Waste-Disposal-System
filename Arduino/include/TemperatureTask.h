#pragma once
#include "Task.h"
#include "SMDSFiniteStateMachine.h"

class TemperatureTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
public:
    TemperatureTask(SMDSFiniteStateMachine* finiteStateMachine);
    virtual void execute() override;
};