#include "SleepTask.h"

SleepTask::SleepTask(){
    active = true;
    time = 0;
}

void SleepTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void goToSleep() {
    Serial.println("IN USCITA DALLA SLEEP MODE");
    Serial.flush();
}

void SleepTask::execute(){
    if(fsm->state == SLEEPING) {
        attachInterrupt(digitalPinToInterrupt(PIR), goToSleep, CHANGE);
        Serial.println("ENTERING SLEEP MODE");
        Serial.flush();
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
        Serial.println("RISVEGLIATO DALLA SLEEP MODE");
        Serial.flush();
        detachInterrupt(PIR);
        fsm->state = AVAILABLE;
    }

    presence = digitalRead(PIR);
    if(presence == LOW && presencePrev == HIGH) {
        time = millis();
    }
    if(presence == LOW) {
        unsigned long deltaTime = millis();
        if((deltaTime-time) >= TSLEEP) {
            fsm->state = SLEEPING;
        }
    } else {
        time = millis();
    }
    presencePrev = presence;
}