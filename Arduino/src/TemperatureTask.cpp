#include "TemperatureTask.h"

TemperatureTask::TemperatureTask(){
    active = true;
    time = 0;
}

void TemperatureTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void TemperatureTask::bindSerialCommunicator(SerialCommunicator* dTask) {
    scTask = dTask;
}

void TemperatureTask::execute(){
    float value = analogRead(TEMP);
    /*
    value is read from the analog sensor
    0.00488 is the precision of the ADC
    0.5 is the tension of 0 °C
    0.01 is the variation of 1 °C
    */
    float temp = ((value * 0.00488) - 0.5) / 0.01;
    scTask->sendTemperature(temp);
    if(temp >= MAXTEMP && tempPrev < MAXTEMP) {
        time = millis(); //Start control time for MAXTEMPTIME
    }
    if(temp >= MAXTEMP) {
        unsigned long deltaTime = millis(); //Time in which MAXTEMP is still outdated
        if((deltaTime - time) >= MAXTEMPTIME) {
            fsm->state = OVERHEATING;
        }
    }
    tempPrev = temp;
}

//called by
void TemperatureTask::onFixTemperatureEvent(){

}