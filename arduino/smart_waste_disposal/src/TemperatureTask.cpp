#include "TemperatureTask.h"

TemperatureTask::TemperatureTask()
    :temperatureHighTimer(MAXTEMPTIME)
{
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

    if(temp < MAXTEMP){
        temperatureHighTimer.reset();
    }

    temperatureHighTimer.update();

    if(temperatureHighTimer.isOver() && (fsm->state == AVAILABLE || fsm->state == ACCEPTING_WASTE || fsm->state == FULL)){
        fsm->state = OVERHEATING;
    }
}

void TemperatureTask::onFixTemperatureEvent(){
    fsm->state = AVAILABLE;
}