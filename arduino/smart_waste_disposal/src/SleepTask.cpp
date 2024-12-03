#include "SleepTask.h"

SleepTask::SleepTask()
    :noPresenceTimer(TSLEEP)
{
    active = true;
    time = 0;
}

void SleepTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void goToSleep() {
    //Serial.println("IN USCITA DALLA SLEEP MODE");
    //Serial.flush();
}

void SleepTask::execute(){
    if(fsm->state == SLEEPING) {
        attachInterrupt(digitalPinToInterrupt(PIR), goToSleep, CHANGE);
        //Serial.println("ENTERING SLEEP MODE");
        //Serial.flush();
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
        //Serial.println("RISVEGLIATO DALLA SLEEP MODE");
        //Serial.flush();
        detachInterrupt(PIR);
        fsm->state = AVAILABLE;
        noPresenceTimer.reset();
    }

    if(fsm->state != lastState){
        if(fsm->state == AVAILABLE){
            noPresenceTimer.reset();
            Serial.flush();
        }
        lastState = fsm->state;
    }

    noPresenceTimer.update();

    if(digitalRead(PIR) == LOW) {
        if(noPresenceTimer.isOver() && fsm->state == AVAILABLE) {
            fsm->state = SLEEPING;
        }
    } else {
        noPresenceTimer.reset();
    }
}