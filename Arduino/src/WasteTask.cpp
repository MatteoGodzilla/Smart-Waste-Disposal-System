#include "WasteTask.h"

WasteTask::WasteTask() :
    fullTolleranceTimer(FULL_TOLERANCE_TIME),
    userCanSpillTimer(TIME_TO_CLOSE),
    showAcceptedWasteTimer(ACCEPTED_WASTE_DELAY),
    emptyingSimulationTimer(EMPTYING_TIME)
{
    //fillPercentage = 0.0;
    active = true;
    //opening = false;
    //closing = false;
    receivedEmptyEvent = false;
    //endAlarmManaged = false;
    //fullAlarmManaged = true;
    //temperatureAlarmManaged = true;
    motor.attach(MOTOR);
    motor.write(CLOSED_ANGLE);
}

void WasteTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void WasteTask::bindSerialCommunicator(SerialCommunicator* dTask) {
    scTask = dTask;
}

void WasteTask::execute(){
    updateTimers();
    changeState();
    executeState();
}

/*
*   This method receive data from SONAR and then calculate the fill percentage of the bin.
*   Then return a value between [0.0, ..., 1.0].
*/
float WasteTask::getFillPercentage() {
    digitalWrite(SONAR_TRIG, LOW);
    delayMicroseconds(3);
    digitalWrite(SONAR_TRIG, HIGH);
    delayMicroseconds(5);
    digitalWrite(SONAR_TRIG, LOW);

    float tUS = pulseIn(SONAR_ECHO, HIGH);
    float t = tUS / 1000.0 / 1000.0 / 2;
    float d = (t*SOUND_SPEED);
    float percent = (((EMPTY_DISTANCE - d) / DISTANCE_RANGE));
    percent = (percent > 1.0) ? 1.0 : percent;
    percent = (percent < 0.0) ? 0.0 : percent;
    return percent;
}

/*
*    This method enter inside the actual state, then check if there are the conditions
*    to change the Final State Machine state.
*/
void WasteTask::changeState(){
    fillPercentage = getFillPercentage();
    scTask->sendFillPercentage(fillPercentage);
    switch(fsm->state){
        case AVAILABLE:
            if( digitalRead(OPEN_BTN) == HIGH ) { /* When open button is pressed then open bin door */
                motor.write(OPEN_ANGLE);
                userCanSpillTimer.reset();
                fsm->state = ACCEPTING_WASTE;
            } else if(fullTolleranceTimer.isOver() && fillPercentage > WASTE_THRESHOLD){
                //should not be necessary
                //motor.write(CLOSED_ANGLE);
                //angle = CLOSED_ANGLE;
                //Serial.print("AAAA");
                fsm->state = FULL;
            }

            break;
        case ACCEPTING_WASTE:
            if( digitalRead(CLOSE_BTN) == HIGH  || userCanSpillTimer.isOver() ) {
                motor.write(CLOSED_ANGLE);
                showAcceptedWasteTimer.reset();
                fsm->state = WASTE_RECEIVED;
            } else if(fullTolleranceTimer.isOver() && fillPercentage > WASTE_THRESHOLD){
                motor.write(CLOSED_ANGLE);
                fsm->state = FULL;
            }
            break;
        case WASTE_RECEIVED:
            if(showAcceptedWasteTimer.isOver()){
                fsm->state = AVAILABLE;
            }

            break;
        case EMPTYING:
            if(emptyingSimulationTimer.isOver()){
                motor.write(CLOSED_ANGLE);
                fsm->state = AVAILABLE;
            }
            break;
        case FULL:
            if(receivedEmptyEvent){
                motor.write(EMPTYING_ANGLE);
                fsm->state = EMPTYING;
                receivedEmptyEvent = false;
            }
            break;
        case OVERHEATING:
        case SLEEPING:
            //Do nothing because it's handled in other tasks
            break;
    }
}

void WasteTask::executeState(){
    if(fillPercentage < WASTE_THRESHOLD){
        fullTolleranceTimer.reset();
    }

    switch(fsm->state){
        case AVAILABLE:
        case ACCEPTING_WASTE:
        case WASTE_RECEIVED:
        case EMPTYING:
            digitalWrite(L1, HIGH);
            digitalWrite(L2, LOW);
            break;
        case FULL:
            digitalWrite(L1, LOW);
            digitalWrite(L2, HIGH);
            break;
        case OVERHEATING:
            digitalWrite(L1, LOW);
            digitalWrite(L2, HIGH);
            motor.write(CLOSED_ANGLE);
            break;
        case SLEEPING:
            break;
    }
}

void WasteTask::updateTimers(){
    fullTolleranceTimer.update();
    userCanSpillTimer.update();
    showAcceptedWasteTimer.update();
    emptyingSimulationTimer.update();
}

void WasteTask::onEmptyEvent(){
    receivedEmptyEvent = true;
}