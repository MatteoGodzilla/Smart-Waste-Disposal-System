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

void WasteTask::changeState(){
    fillPercentage = getFillPercentage();
    scTask->sendFillPercentage(fillPercentage);
    switch(fsm->state){
        case AVAILABLE:
            if( ( fillPercentage > WASTE_THRESHOLD ) && !timeset) {
                timeset = true;
                timeSince = millis();
            }
            if( ( ( millis() - timeSince ) > FULL_TOLERANCE_TIME ) && ( fillPercentage > WASTE_THRESHOLD ) ) {
                timeset = false;
                fsm->state = FULL;
            }
            if( !endAlarmManaged ) {
                endAlarmManaged = true;
                digitalWrite(L1, HIGH);
                digitalWrite(L2, LOW);
            }
            openButtonState = digitalRead(OPEN_BTN);
            if( angle == OPEN_ANGLE ) {
                /* When servo has terminated opening procedure then go to ACCPETING_WASTE state */
                timeset = false;
                timeSince = millis();
                fsm->state = ACCEPTING_WASTE;
            }
            break;
        case ACCEPTING_WASTE:
            closeButtonState = digitalRead(CLOSE_BTN);
            if( ( closeButtonState == HIGH ) || ( ( millis() - timeSince ) >= TIME_TO_CLOSE ) ) { /* CLOSE BUTTON pressed OR TIMELIMIT REACHED event sent */
                //closeButtonState = LOW; ATTENZIONE - CREDO NON SERVA
                fsm->state = WASTE_RECEIVED; /* Maybe check the waste level to skip WASTE_RECEIVED state if the bin is full? */
            }
            break;
        case WASTE_RECEIVED: /* At the moment the system check if the bin is full only when is closed */
            if( angle == CLOSED_ANGLE ) {
                /* Then decide the next state */
                fsm->state = fillPercentage > WASTE_THRESHOLD ? FULL : AVAILABLE;
            }
            break;
        case EMPTYING:
            /* Do nothing since release signal is received (someone change the state to EMPTYING)*/
            /* On release signal received then */
            if( ( angle == RELEASE_ANGLE ) && !timeset ) {
                /* When waste is released */
                timeSince = millis();
                timeset = true;
            }
            if( ( millis() - timeSince ) >= RELEASE_TIME ) {
                closing = true;
                timeset = false;
            }
            if( angle == CLOSED_ANGLE ) {
                fsm->state = fillPercentage > WASTE_THRESHOLD ? FULL : AVAILABLE;
            }
            break;
        case FULL:
            /* Arduino is sending a signal to Dashboard */
            /* On signal received set fullAlarmManaged to false and then go to EMPTYING state */
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
            /* Wait for TemperatureTask to handle the problem, WasteTask in this phase is not active*/
            if( angle != CLOSED_ANGLE ) {
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
            if(angle != CLOSED_ANGLE) {
                motor.write(CLOSED_ANGLE); /* Who wrote this?? It was with no guard condition, every clock it was trying to close! */
            }
            fullAlarmManaged = false;
            temperatureAlarmManaged = false;
            if( ( openButtonState == HIGH ) ) { /* Then open the bin */
                motor.write(OPEN_ANGLE);
                angle = OPEN_ANGLE;
                //openButtonState = LOW; ATTENZIONE - CARLO NON CREDO SERVA VEDIAMO LASCIANDOLO COMMENTATO.
            }
            break;
        case ACCEPTING_WASTE:
            /* Setup Timer and CLOSE button check */
            /* Then fsm->WASTE_RECEIVED */
            break;
        case WASTE_RECEIVED:
                /* Moving the servo to close the bin */
                motor.write(CLOSED_ANGLE);
                angle = CLOSED_ANGLE;
                closing = false;
            break;
        case EMPTYING:
            if( opening ) { /* Moving the servo to release the waste */
                motor.write(RELEASE_ANGLE);
                angle = RELEASE_ANGLE;
                opening = false;
            } else if ( closing ) { /* Moving the servo to close the bin */
                motor.write(CLOSED_ANGLE);
                angle = CLOSED_ANGLE;
                closing = false;
            }
            break;
        case FULL:
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
            if( !temperatureAlarmManaged ) {
                endAlarmManaged = false;
                temperatureAlarmManaged = true;
                Serial.println("INTERVENTION REQUIRED: THE BIN TEMPERATURE IS OVER THE MAXIMUM THRESHOLD");
                Serial.flush();
                digitalWrite(L1, LOW);
                digitalWrite(L2, HIGH);
            }
            if( closing ) { /* Moving the servo to close the bin */
                motor.write(CLOSED_ANGLE);
                angle = CLOSED_ANGLE;
                closing = false;
            }
            break;
        case SLEEPING:
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