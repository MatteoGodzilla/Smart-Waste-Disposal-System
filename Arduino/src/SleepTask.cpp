#include "SleepTask.h"

//abilita enable interrupt (può essere presente soltanto in un file in tutto il progetto)
#define LIBCALL_ENABLEINTERRUPT

SleepTask::SleepTask(){
    active = true;
}

void SleepTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void goToSleep() {}

void SleepTask::execute(){
    presence = digitalRead(PIR);
    if(presence == LOW && presencePrev == HIGH) {
        time = millis();
    }
    if(presence == LOW) {
        unsigned long deltaTime = millis();
        if((deltaTime-time) >= TSLEEP) {
            fsm->state = SLEEPING;
            enableInterrupt(PIR, goToSleep, CHANGE);
            LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
            disableInterrupt(PIR);
            fsm->state = AVAILABLE;
        }
    } else {
        time = millis();
    }
    presencePrev = presence;
}