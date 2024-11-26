#include "WasteTask.h"

WasteTask::WasteTask(){
    angle = CLOSED_ANGLE;
    fillPercentage = 0.0;
    active = true;
    opening = false;
    closing = false;
    wasteReleased = false;
    endAlarmManaged = false;
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
    percent = (percent < 0.0) ? 0.0: percent;
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
            if( ( fillPercentage > WASTE_THRESHOLD ) && !timeset) {
                timeset = true;
                timeSince = millis();
            }
            if( timeset && ( ( millis() - timeSince ) > FULL_TOLERANCE_TIME ) && ( fillPercentage > WASTE_THRESHOLD ) ) {
                timeset = false;
                fsm->state = FULL;
            }
            /* Everytime the state is changed to AVAILABLE, only one time is done a "reset" of the LEDs lights. */
            if( !endAlarmManaged ) {
                endAlarmManaged = true;
                digitalWrite(L1, HIGH);
                digitalWrite(L2, LOW);
            }
            /*
            *  Every "clock" time the open button state is checked, if the button is pressed then in WasteTask::executeState()
            *  the bin door will be open.
            */
            openButtonState = digitalRead(OPEN_BTN);
            /* When servo has terminated opening procedure then send Final State Machine to ACCEPTING_WASTE state */
            if( angle == OPEN_ANGLE ) {
                timeset = false;
                timeSince = millis();
                fsm->state = ACCEPTING_WASTE;
            }
            break;
        case ACCEPTING_WASTE:
            /* 
            *  When the Final State Machine is in ACCEPTING_WASTE state, it checks every time the close button state.
            *  If the close button state goes to HIGH (pressed) OR the spill time limit is reached then close the bin and
            *  send Final State Machine to WASTE_RECEIVED state.
            *  The "countdown" for spill time limit is started inside AVAILABLE state, before the Final State Machine change the state.
            */
            closeButtonState = digitalRead(CLOSE_BTN);
            if( ( closeButtonState == HIGH ) || ( ( millis() - timeSince ) >= TIME_TO_CLOSE ) ) {
                //closeButtonState = LOW; ATTENZIONE - CREDO NON SERVA
                fsm->state = WASTE_RECEIVED; /* Maybe check the waste level to skip WASTE_RECEIVED state if the bin is full? */
            }
            break;
        case WASTE_RECEIVED:
            /* 
            *  When the Final State Machine is in WASTE_RECEIVED state, it checks every time the bin is closed.
            *  Then it checks if the waste has reached the WASTE_THRESHOLD and then decide the next state.
            */
            if( angle == CLOSED_ANGLE ) {
                fsm->state = fillPercentage > WASTE_THRESHOLD ? FULL : AVAILABLE;
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
            if( ( angle == RELEASE_ANGLE ) && !timeset ) {
                /* Waste starts to be released */
                timeSince = millis();
                timeset = true;
            }
            if( timeset && ( millis() - timeSince ) >= RELEASE_TIME ) {
                closing = true;
                timeset = false;
            }
            if( angle == CLOSED_ANGLE ) {
                fsm->state = fillPercentage > WASTE_THRESHOLD ? FULL : AVAILABLE; /* Maybe remove this from here or from AVAILABLE state */
            }
            break;
        case FULL:
            /* 
            *  Everytime the state is changed to FULL, only one time is done a check if the waste level is already under the WASTE_THRESHOLD. 
            *  If it does, then is saved the actual time and after a predetermined FULL_TOLERANCE_TIME the check is repeated.
            *  After the second check if the bin appear to not be full, then send the FInal State Machine directly to AVAILABLE state. 
            *  Else wait for Dashboard signal to send the Final State Machine to EMPTYING state.
            */
            if( fillPercentage < WASTE_THRESHOLD && !timeset) { /* Just in case the PIR send a fake alarm */
                timeset = true;
                timeSince = millis();
            }
            if( ( (millis() - timeSince) > FULL_TOLERANCE_TIME ) && ( fillPercentage < WASTE_THRESHOLD ) ) {
                timeset = false;
                fsm->state = AVAILABLE;
            }
            if( wasteReleased ) {
                wasteReleased = false;
                timeset = false;
                opening = true;
                fsm->state = EMPTYING;
            }
            break;
        case OVERHEATING:
            /* 
            *  When the Final State Machine is in OVERHEATING state then wait for TemperatureTask to handle the problem,
            *  WasteTask in this phase only close the bin door if its not closed.
            */
            if( angle != CLOSED_ANGLE ) {
                closing = true;
            }
            break;
        case SLEEPING:
            /* When the Final State Machine is in SLEEPING state WasteTask in this phase only close the bin door if its not closed. */
            /* On wake up signal SleepTask send the Final State Machine to AVAILABLE */
            break;
    }
}

void WasteTask::executeState(){
    switch(fsm->state){
        case AVAILABLE:
            if(angle != CLOSED_ANGLE) { /* We need to have the door already closed when the Final State MAchine is sent to AVAILABLE state*/
                motor.write(CLOSED_ANGLE);
            }
            /* Resetting boolean guards used to set LEDs light to emergency mode */
            fullAlarmManaged = false;
            temperatureAlarmManaged = false;
            if( ( openButtonState == HIGH ) ) { /* When open button is pressed then open bin door */
                motor.write(OPEN_ANGLE);
                angle = OPEN_ANGLE;
                //openButtonState = LOW; ATTENZIONE - CARLO NON CREDO SERVA VEDIAMO LASCIANDOLO COMMENTATO.
            }
            break;
        case ACCEPTING_WASTE:
            /* All the necessary logic is already in WasteTask::changeState(). */
            break;
        case WASTE_RECEIVED:
            /* Moving the servo to close the bin */
            motor.write(CLOSED_ANGLE);
            angle = CLOSED_ANGLE;
            closing = false;
            break;
        case EMPTYING:
            /* Moving the servo to release the waste. */
            if( opening ) {
                motor.write(RELEASE_ANGLE);
                angle = RELEASE_ANGLE;
                opening = false;
            } else if ( closing ) { /* Moving the servo to close the bin. */
                motor.write(CLOSED_ANGLE);
                angle = CLOSED_ANGLE;
                closing = false;
            }
            break;
        case FULL:
            /* Turn LEDs light to emergency mode and print on LCD the warning. */
            if( !fullAlarmManaged ) {
                endAlarmManaged = false;
                fullAlarmManaged = true;
                Serial.println("INTERVENTION REQUIRED: THE BIN IS FULL");
                Serial.flush();
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            break;
        case OVERHEATING:
            /* Turn LEDs light to emergency mode and print on LCD the warning. */
            if( !temperatureAlarmManaged ) {
                endAlarmManaged = false;
                temperatureAlarmManaged = true;
                Serial.println("INTERVENTION REQUIRED: THE BIN TEMPERATURE IS OVER THE MAXIMUM THRESHOLD");
                Serial.flush();
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            /* Moving the servo to close the bin. */
            if( closing ) {
                motor.write(CLOSED_ANGLE);
                angle = CLOSED_ANGLE;
                closing = false;
            }
            break;
        case SLEEPING:
            /* Moving the servo to close the bin. */
            if (angle != CLOSED_ANGLE) {
                motor.write(CLOSED_ANGLE);
                angle = CLOSED_ANGLE;
            }
            break;
    }
}

void WasteTask::onEmptyEvent(){
    wasteReleased = true;
}