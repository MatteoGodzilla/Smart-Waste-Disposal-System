#include "LCDManager.h"

LCDManager::LCDManager(SMDSFiniteStateMachine* finiteStateMachine) {
    lcd = new LiquidCrystal_I2C(0x27,20,4);
    lcd->init();
    lcd->backlight();
    fsm = finiteStateMachine;
}

void LCDManager::updateLCD() {
    switch(fsm->state) {
        case AVAILABLE:
            break;
        case ACCEPTING_WASTE:
            break;
        case FULL:
            break;
        case OVERHEATING:
            break;
        case SLEEPING:
            break;
    }
}