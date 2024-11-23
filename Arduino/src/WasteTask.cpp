#include "WasteTask.h"

WasteTask::WasteTask(){
    active = true;
    angle = 0;
    motor.attach(MOTOR);
}

void WasteTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void WasteTask::bindDashboard(DashboardTask* dTask) {
    dashboardTask = dTask;
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
    return ((100*d) / DISTANCE_RANGE);
}

void WasteTask::changeState(){
    switch(fsm->state){
        case AVAILABLE:
            break;
        case ACCEPTING_WASTE:
            break;
        case WASTE_RECEIVED:
            break;
        case EMPTYING:
            break;
        case FULL:
            break;
        case OVERHEATING:
            break;
    }
}

void WasteTask::executeState(){
    int openButtonState = digitalRead(OPEN_BTN);
    bool opening = false;
    bool fullAlarmON = false;
    bool temperatureAlarmON = false;
    switch(fsm->state){
        case AVAILABLE:
            if(fullAlarmON) {
                fullAlarmON = false;
                digitalWrite(L1, HIGH);
                digitalWrite(L2, LOW);
            }
            if(openButtonState == HIGH) {
                opening = true;
            }
            if(opening) {
                if(angle != MAX_ANGLE) { /* Moving the servo one degree at a time */
                    motor.write(++angle);
                } else { /* When servo has terminated */
                    opening = false;
                    fsm->state = ACCEPTING_WASTE;
                }
            }
            break;
        case ACCEPTING_WASTE:
            /* Setup Timer and CLOSE button check */
            /* Then fsm->WASTE_RECEIVED */
            break;
        case WASTE_RECEIVED:
            if(angle != MIN_ANGLE) { /* Moving the servo one degree at a time */
                motor.write(--angle);
            } else { /* When servo has terminated */
                opening = false;
                fsm->state = getFillPercentage() == 100.0 ? fsm->state = FULL : fsm->state = AVAILABLE;
            }
            break;
        case EMPTYING:
            if(angle != -MAX_ANGLE) { /* Moving the servo one degree at a time */
                motor.write(--angle);
            } else { /* When servo has terminated */
                fsm->state = AVAILABLE;
            }
            break;
        case FULL:
            if(!fullAlarmON && (getFillPercentage() == 100)) {
                fullAlarmON = true;
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            break;
        case OVERHEATING:
            if(!temperatureAlarmON) {
                temperatureAlarmON = true;
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            if(angle != MIN_ANGLE) { /* Moving the servo one degree at a time */
                motor.write(angle > MIN_ANGLE ? --angle : ++angle);
            }
            break;
    }
}
