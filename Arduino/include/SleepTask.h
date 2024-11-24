#pragma once
#include <Arduino.h>
#include "Task.h"
#include "SMDSFiniteStateMachine.h"
#include "pins.h"
#include "LowPower.h"

#define TSLEEP 5000

class SleepTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
    int presence;
    int presencePrev = LOW;
    unsigned long time;
public:
    SleepTask();
    void bindFSM(SMDSFiniteStateMachine* fsmTask);
    virtual void execute() override;
};