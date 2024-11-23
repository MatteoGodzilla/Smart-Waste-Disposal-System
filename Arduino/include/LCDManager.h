#pragma once
#include <LiquidCrystal_I2C.h>
#include "SMDSFiniteStateMachine.h"
#include "Task.h"

class LCDManager : public Task{
private:
    LiquidCrystal_I2C* lcd;
    SMDSFiniteStateMachine* fsm;
    State lastState;
public:
    LCDManager();
    void bindFSM(SMDSFiniteStateMachine* fsmTask);
    void execute() override;

};