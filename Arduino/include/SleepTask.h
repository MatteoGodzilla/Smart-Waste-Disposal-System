#pragma once
#include "Task.h"
#include "SMDSFiniteStateMachine.h"

class SleepTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
public:
    SleepTask();
    void bindFSM(SMDSFiniteStateMachine* fsmTask);
    virtual void execute() override;
};