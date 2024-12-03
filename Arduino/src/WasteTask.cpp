#include "WasteTask.h"

WasteTask::WasteTask() :
    fullTolleranceTimer(FULL_TOLERANCE_TIME),
    userCanSpillTimer(TIME_TO_CLOSE),
    showAcceptedWasteTimer(ACCEPTED_WASTE_DELAY),
    emptyingSimulationTimer(EMPTYING_TIME)
{
    active = true;
    receivedEmptyEvent = false;
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
*  This method checks whether the conditions exist to send the Finite State Machine to another state.
*  But before the changing it performs all the actions necessary to go correctly to the other state.
*/
void WasteTask::changeState(){
    fillPercentage = getFillPercentage();
    scTask->sendFillPercentage(fillPercentage);
    switch(fsm->state){
        case AVAILABLE:
            /*
            *  When the Finite State Machine is in AVAILABLE state after some time (defined in fullToleranceTimer) a check is done to verify the waste level.
            *  If the waste level it's over the WASTE_THRESHOLD the Finite State Machine is sent to FULL state.
            *  During all the time the Finite State Machine checks if the OPEN BUTTON is pressed by the user.
            *  When the OPEN BUTTON is pressed and the Finite State Machine is still in AVAILABLE state then open the bin door and send it to ACCEPTING_WASTE state.
            */
            if( digitalRead(OPEN_BTN) == HIGH ) { /* When open button is pressed then open bin door */
                motor.write(OPEN_ANGLE);
                userCanSpillTimer.reset();
                fsm->state = ACCEPTING_WASTE;
            } else if( fullTolleranceTimer.isOver() && ( fillPercentage > WASTE_THRESHOLD ) ) {
                /*
                *  When the fullToleranceTime is over then the Finite State Machine checks if the waste level is over WASTE_THRESHOLD
                *  and if it is then send Finite State Machine to FULL state.
                */
                fsm->state = FULL;
            }

            break;
        case ACCEPTING_WASTE:
            /*
            *  When the Finite State Machine is in ACCEPTING_WASTE state, it checks every time the CLOSE BUTTON state.
            *  If the CLOSE BUTTON goes to HIGH (pressed) OR the spill time limit is reached then the Machine close
            *  the bin and it goes to WASTE_RECEIVED state.
            *  userCanSpillTimer that correspond to the spill time limit is started inside AVAILABLE state, before the state changing.
            *  When the Finite State Machine is in ACCEPTING_WASTE state after some time (defined in fullToleranceTimer) a check is done to verify the waste level.
            *  If the waste level it's over the WASTE_THRESHOLD the Finite State Machine is sent to FULL state.
            */
            if( digitalRead(CLOSE_BTN) == HIGH  || userCanSpillTimer.isOver() ) {
                motor.write(CLOSED_ANGLE);
                showAcceptedWasteTimer.reset();
                fsm->state = WASTE_RECEIVED;
            } else if(fullTolleranceTimer.isOver() && fillPercentage > WASTE_THRESHOLD){
                fsm->state = FULL;
            }
            break;
        case WASTE_RECEIVED:
            /*
            *  When the Finite State Machine is in WASTE_RECEIVED state, it checks if showAcceptedWasteTimer
            *  (started inside ACCEPTING_WASTE before state changing to WASTE_RECEIVED) ends, then it goes to AVAILABLE state.
            */
            if(showAcceptedWasteTimer.isOver()){
                fsm->state = AVAILABLE;
            }

            break;
        case EMPTYING:
            /*
            *  When the Finite State Machine is in EMPTYING state, it closes the bin door.
            *  When the bin door is closed then the Finite State Machine goes to AVAILABLE state.
            */
            if(emptyingSimulationTimer.isOver()){
                motor.write(CLOSED_ANGLE);
                fsm->state = AVAILABLE;
            }
            break;
        case FULL:
            /*
            *  Everytime the state is changed to FULL, it waits for Dashboard signal to start the release procedure and then
            *  send the Finite State Machine to EMPTYING state.
            */
            if(receivedEmptyEvent){
                motor.write(EMPTYING_ANGLE);
                fsm->state = EMPTYING;
                receivedEmptyEvent = false;
                emptyingSimulationTimer.reset();
            }
            break;
        case OVERHEATING:
            /* When the Finite State Machine is in OVERHEATING state then wait for TemperatureTask to handle the problem. */
            break;
        case SLEEPING:
            /* When the Finite State Machine is in SLEEPING state then wait for SleepTask to chamge the state. */
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
            setAlarm(false);
            break;
        case FULL:
        case OVERHEATING:
            setAlarm(true);
            motor.write(CLOSED_ANGLE);
            break;
        case EMPTYING:
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

void WasteTask::setAlarm(bool alarmed) {
    if(alarmed) {
        digitalWrite(L1, LOW);
        digitalWrite(L2, HIGH);
    } else {
        digitalWrite(L1, HIGH);
        digitalWrite(L2, LOW);
    }
}

void WasteTask::onEmptyEvent(){
    receivedEmptyEvent = true;
}