#include "WasteTask.h"

WasteTask::WasteTask(){
    angle = 0;
    active = true;
    opening = false;
    closing = false;
    emptying = false;
    fullAlarmTriggered = false;
    temperatureAlarmTriggered = false;
    motor.attach(MOTOR);
}

void WasteTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void WasteTask::bindSerialCommunicator(SerialCommunicator* dTask) {
    scTask = dTask;
}

void WasteTask::execute(){
    changeState();
    executeState();
}

float WasteTask::getFillPercentage() {
    digitalWrite(SONAR_TRIG, LOW);
    delayMicroseconds(3);
    digitalWrite(SONAR_TRIG, HIGH);
    delayMicroseconds(5);
    digitalWrite(SONAR_TRIG, LOW);

    float tUS = pulseIn(SONAR_ECHO, HIGH);
    float t = tUS / 1000.0 / 1000.0 / 2;
    float d = EMPTY_DISTANCE - (t*SOUND_SPEED);
    return (((DISTANCE_RANGE - d) / DISTANCE_RANGE) * 100.0);
}

void WasteTask::changeState(){
    switch(fsm->state){
        case AVAILABLE:
            int openButtonState = digitalRead(OPEN_BTN);
            if(openButtonState == HIGH) {
                opening = true;
            }
            if(angle == MAX_ANGLE) { 
                /* When servo has terminated opening procedure then go to ACCPETING_WASTE state */
                opening = false;
                fsm->state = ACCEPTING_WASTE;
            }
            break;
        case ACCEPTING_WASTE:
            int closingButtonState = digitalRead(CLOSE_BTN);
            if(closingButtonState == HIGH) {
                closing = true;
            }
            break;
        case WASTE_RECEIVED:
            if(angle == MIN_ANGLE) { 
                /* When servo has terminated closing procedure, then check waste level. */
                closing = false;
                /* Then decide the next state */
                fsm->state = getFillPercentage() == 100.0 ? FULL : AVAILABLE;
            }
            break;
        case EMPTYING:
            /* Do nothing since signal is received */
            /* On release signal received emptying = true */
            if(angle == -MAX_ANGLE) {
                /* When waste is released */
                emptying = false;
                closing = true;
            }
            if(angle == MIN_ANGLE) {
                closing = false;
                fsm->state = AVAILABLE;
            }
            break;
        case FULL:
            /* Arduino is sending a signal to Dashboard */
            /* On signal received set fullAlarmTriggered and then go to EMPTYING state */
            break;
        case OVERHEATING:
            /* Arduino is sending a signal when the temperature is OK */
            /* Then set temperatureAlarmTriggered to false and go back to latest state */
            break;
        case SLEEPING:
            /* On wake up signal go back to latest state */
            break;
    }
}

void WasteTask::executeState(){
    switch(fsm->state){
        case AVAILABLE:
            if(fullAlarmTriggered) {
                fullAlarmTriggered = false;
                digitalWrite(L1, HIGH);
                digitalWrite(L2, LOW);
            }
            if(opening) {
                /* Moving the servo one degree a clock */
                motor.write(++angle);
            }
            break;
        case ACCEPTING_WASTE:
            /* Setup Timer and CLOSE button check */
            /* Then fsm->WASTE_RECEIVED */
            break;
        case WASTE_RECEIVED:
            if(closing) {
                /* Moving the servo one degree a clock */
                motor.write(--angle);
            }
            break;
        case EMPTYING:
            if(emptying) { /* Moving the servo one degree a clock to release the waste */
                motor.write(--angle);
            } else if (closing) { /* Moving the servo one degree a clock to close */
                motor.write(++angle);
            }
            break;
        case FULL:
            if(!fullAlarmTriggered && (getFillPercentage() == 100)) {
                fullAlarmTriggered = true;
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            break;
        case OVERHEATING:
            if(!temperatureAlarmTriggered) {
                temperatureAlarmTriggered = true;
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            if(angle != MIN_ANGLE) { /* Moving the servo one degree at a time */
                motor.write(angle > MIN_ANGLE ? --angle : ++angle);
            }
            break;
        case SLEEPING:
            break;
    }
}

void WasteTask::onEmptyEvent(){
    
}