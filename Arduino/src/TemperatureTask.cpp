#include "TemperatureTask.h"

TemperatureTask::TemperatureTask(SMDSFiniteStateMachine* finiteStateMachine){
    active = true;
    fsm = finiteStateMachine;
}

void TemperatureTask::bind(DashboardTask* dTask) {
    dashboardTask = dTask;
}

void TemperatureTask::execute(){
    float value = (analogRead(TEMP)*5)/1023;
    float deltaV = value - 750;
    float deltaC = deltaV / 10;
    float temp = deltaC + 25; //temp contains the real value of temperature
    //dashboardTask->sendTemperature(temp);
    if(temp >= MAXTEMP && tempPrev < MAXTEMP) {
        time = millis(); //Start control time for MAXTEMPTIME
    }
    if(temp >= MAXTEMP) {
        unsigned long deltaTime = millis(); //Time in which MAXTEMP is still outdated
        if((deltaTime - time) >= MAXTEMPTIME) {
            fsm->state = OVERHEATING;
            //TODO: disattivazione di tutti gli altri task tranne dashboard
            digitalWrite(L1, LOW);
            digitalWrite(L2, HIGH);
        }
    }
    tempPrev = temp;
}