#pragma once
#include <LiquidCrystal_I2C.h>
#include "SMDSFiniteStateMachine.h"

class LCDManager {
private:
    LiquidCrystal_I2C* lcd;
    SMDSFiniteStateMachine* fsm;

public:
    LCDManager(SMDSFiniteStateMachine* finiteStateMachine);
    void updateLCD();

};