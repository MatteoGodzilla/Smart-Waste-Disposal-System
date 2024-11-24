#include "LCDManager.h"

LCDManager::LCDManager() {
    lcd = new LiquidCrystal_I2C(0x27,20,4);
    lcd->init();
    lcd->backlight();
    lastState = SLEEPING;
}

void LCDManager::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void LCDManager::execute() {
    if(lastState != fsm->state){
        switch(fsm->state) {
            case AVAILABLE:
                //lcd->clear();
                lcd->setCursor(0,0);
                lcd->print("PRESS OPEN TO ENTER WASTE");
                break;
            case ACCEPTING_WASTE:
                //lcd->clear();
                lcd->setCursor(0,0);
                lcd->print("PRESS CLOSE WHEN DONE");
                break;
            case WASTE_RECEIVED:
                //lcd->clear();
                lcd->setCursor(0,0);
                lcd->print("WASTE RECEIVED");
                break;
            case FULL:
                //lcd->clear();
                lcd->setCursor(0,0);
                lcd->print("CONTAINER FULL");
                break;
            case OVERHEATING:
                //lcd->clear();
                lcd->setCursor(0,0);
                lcd->print("PROBLEM DETECTED");
                break;
            case SLEEPING:
                //lcd->clear();
                lcd->noBacklight();
                break;
            case EMPTYING:
                break;
        }
        lastState = fsm->state;
    }
}