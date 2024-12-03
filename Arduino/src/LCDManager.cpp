#include "LCDManager.h"

//TODO REMOVE
#include <Arduino.h>

LCDManager::LCDManager()
    : lcd(LiquidCrystal_I2C(0x27,20,4))
{
    lcd.init();
    lcd.backlight();
    lastState = SLEEPING;
    active = true;
}

void LCDManager::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void LCDManager::execute() {
    if(lastState != fsm->state){
        lcd.clear();
        switch(fsm->state) {
            case AVAILABLE:
                lcd.backlight();
                lcd.setCursor(0,0);
                lcd.print("PRESS OPEN TO ENTER WASTE");
                break;
            case ACCEPTING_WASTE:
                lcd.setCursor(0,0);
                lcd.print("PRESS CLOSE");
                lcd.setCursor(0,1);
                lcd.print("WHEN DONE");
                break;
            case WASTE_RECEIVED:
                lcd.setCursor(0,0);
                lcd.print("WASTE RECEIVED");
                break;
            case FULL:
            case EMPTYING:
                lcd.setCursor(0,0);
                lcd.print("CONTAINER FULL");
                break;
            case OVERHEATING:
                lcd.setCursor(0,0);
                lcd.print("PROBLEM DETECTED");
                break;
            case SLEEPING:
                lcd.noBacklight();
                break;
        }
        lastState = fsm->state;
    }
}