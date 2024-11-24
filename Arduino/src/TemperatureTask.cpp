#include "TemperatureTask.h"

TemperatureTask::TemperatureTask(){
    active = true;
}

void TemperatureTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void TemperatureTask::bindSerialCommunicator(SerialCommunicator* dTask) {
    scTask = dTask;
}

void TemperatureTask::execute(){
    float value = (analogRead(TEMP)*5)/1023;
    float deltaV = value - 750;
    float deltaC = deltaV / 10;
    float temp = deltaC + 25; //temp contains the real value of temperature
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