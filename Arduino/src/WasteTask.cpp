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
            /*
            *  Everytime the state is changed to AVAILABLE, only one time is done a check if the waste level is over the WASTE_THRESHOLD.
            *  If it does, then is saved the actual time and after a predetermined FULL_TOLERANCE_TIME the check is repeated.
            *  After the second check if the bin appear to be full, then send the FInal State Machine directly to FULL state.
            */
            if( digitalRead(OPEN_BTN) == HIGH ) { /* When open button is pressed then open bin door */
                /*
                *  Every "clock" time the open button state is checked, if the button is pressed then in WasteTask::executeState()
                *  the bin door will be open.
                */
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
            /*
            *  When the Final State Machine is in ACCEPTING_WASTE state, it checks every time the close button state.
            *  If the close button state goes to HIGH (pressed) OR the spill time limit is reached then close the bin and
            *  send Final State Machine to WASTE_RECEIVED state.
            *  The "countdown" for spill time limit is started inside AVAILABLE state, before the Final State Machine change the state.
            */
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
            /*
            *  When the Final State Machine is in WASTE_RECEIVED state, it checks every time the bin is closed.
            *  Then it checks if the waste has reached the WASTE_THRESHOLD and then decide the next state.
            */
            if(showAcceptedWasteTimer.isOver()){
                fsm->state = AVAILABLE;
            }

            break;
        case EMPTYING:
            /*
            *  When the Final State Machine is in EMPTYING state, it starts the release procedure
            *  opening the bin door in RELEASE position.
            *  When the bin door is in RELEASE position then wait a RELEASE_TIME and then close the bin door.
            *  When the bin door is closed then the Final State Machine checks if now the waste level is under
            *  the WASTE_THRESHOLD.
            *  If not send Final State Machine to FULL state else send Final State Machine to AVAILABLE state. !!MAYBE NOT!!
            */
            if(emptyingSimulationTimer.isOver()){
                motor.write(CLOSED_ANGLE);
                fsm->state = AVAILABLE;
            }
            break;
        case FULL:
            /*
            *  Everytime the state is changed to FULL, only one time is done a check if the waste level is already under the WASTE_THRESHOLD.
            *  If it does, then is saved the actual time and after a predetermined FULL_TOLERANCE_TIME the check is repeated.
            *  After the second check if the bin appear to not be full, then send the FInal State Machine directly to AVAILABLE state.
            *  Else wait for Dashboard signal to send the Final State Machine to EMPTYING state.
            */
            if(receivedEmptyEvent){
                motor.write(EMPTYING_ANGLE);
                fsm->state = EMPTYING;
                receivedEmptyEvent = false;
            }
            break;
        case OVERHEATING:
            /*
            *  When the Final State Machine is in OVERHEATING state then wait for TemperatureTask to handle the problem,
            *  WasteTask in this phase only close the bin door if its not closed.
            */
            break;
        case SLEEPING:
            /* When the Final State Machine is in SLEEPING state WasteTask in this phase only close the bin door if its not closed. */
            /* On wake up signal SleepTask send the Final State Machine to AVAILABLE */
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