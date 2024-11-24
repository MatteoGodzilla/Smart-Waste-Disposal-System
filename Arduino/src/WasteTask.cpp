#include "WasteTask.h"

WasteTask::WasteTask(){
    angle = 0;
    active = true;
    opening = false;
    closing = false;
    wasteReleased = false;
    fullAlarmManaged = true;
    temperatureAlarmManaged = true;
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
            Serial.println("Stato di AVAILABLE! su WasteTask");
            digitalWrite(L1, HIGH);
            buttonState = digitalRead(OPEN_BTN);
            if(angle == OPEN_ANGLE) {
                /* When servo has terminated opening procedure then go to ACCPETING_WASTE state */
                openSince = millis();
                fsm->state = ACCEPTING_WASTE;
            }
            break;
        case ACCEPTING_WASTE:
            buttonState = digitalRead(CLOSE_BTN);
            if((buttonState == HIGH) || ((millis() - openSince) >= TIME_TO_CLOSE)) { /* CLOSE BUTTON pressed OR Timer close condition */
                closing = true;
                fsm->state = WASTE_RECEIVED; /* Maybe check the waste level to skip WASTE_RECEIVED state if the bin is full? */
            }
            break;
        case WASTE_RECEIVED: /* At the moment the system the if the bin is full only when is closed */
            if(angle == CLOSED_ANGLE) {
                /* When servo has terminated closing procedure, then check waste level. */
                closing = false;
                /* Then decide the next state */
                if(getFillPercentage() == 100) {
                    fsm->state = FULL;
                } else {
                    fsm->state = AVAILABLE;
                }
            }
            break;
        case EMPTYING:
            /* Do nothing since release signal is received (someone change the state to EMPTYING)*/
            /* On release signal received then */
            if(angle == RELEASE_ANGLE && !timeset) {
                /* When waste is released */
                opening = false;
                openSince = millis();
                timeset = true;
            }
            if((millis() - openSince) >= RELEASE_TIME) {
                closing = true;
                timeset = false;
            }
            if(angle == CLOSED_ANGLE) {
                closing = false;
                fsm->state = getFillPercentage() == 100 ? FULL : AVAILABLE;
            }
            break;
        case FULL:
            /* Arduino is sending a signal to Dashboard */
            /* On signal received set fullAlarmManaged to false and then go to EMPTYING state */
            if(wasteReleased) {
                wasteReleased = false;
                opening = true;
                fsm->state = EMPTYING;
            }
            break;
        case OVERHEATING:
            /* Wait for TemperatureTask to handle the problem, WasteTask in this phase is not active*/
            if(angle != CLOSED_ANGLE) {
                closing = true;
            }
            break;
        case SLEEPING:
            /* On wake up signal SleepTask set the state to AVAILABLE */
            break;
    }
}

void WasteTask::executeState(){
    switch(fsm->state){
        case AVAILABLE:
            fullAlarmManaged = false;
            temperatureAlarmManaged = false;
            if(buttonState == HIGH && !opening) {
                opening = true;
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
            if(opening) { /* Moving the servo one degree a clock to release the waste */
                motor.write(--angle);
            } else if (closing) { /* Moving the servo one degree a clock to close */
                motor.write(++angle);
            }
            break;
        case FULL:
            if(!fullAlarmManaged) {
                fullAlarmManaged = true;
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            break;
        case OVERHEATING:
            if(!temperatureAlarmManaged) {
                temperatureAlarmManaged = true;
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            if(closing) { /* Moving the servo one degree a clock */
                motor.write(angle > CLOSED_ANGLE ? --angle : ++angle);
            }
            break;
        case SLEEPING:
            break;
    }
}

void WasteTask::onEmptyEvent(){
    wasteReleased = true;
}