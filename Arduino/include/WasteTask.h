#pragma once
#include "Task.h"
#include "SMDSFiniteStateMachine.h"

class WasteTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
    void executeState();
    void changeState();
public:
    WasteTask(SMDSFiniteStateMachine* finiteStateMachine);
    virtual void execute() override;
};