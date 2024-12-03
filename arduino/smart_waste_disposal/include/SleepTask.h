#pragma once
#include <Arduino.h>
#include "Task.h"
#include "SMDSFiniteStateMachine.h"
#include "pins.h"
#include "LowPower.h"
#include "ElapsedTimer.h"

#define TSLEEP 15000

class SleepTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
    ElapsedTimer noPresenceTimer;
    int presence;

    State lastState = AVAILABLE;
    unsigned long time;
public:
    SleepTask();
    void bindFSM(SMDSFiniteStateMachine* fsmTask);
    virtual void execute() override;
};