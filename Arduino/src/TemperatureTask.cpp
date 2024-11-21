#include "TemperatureTask.h"

TemperatureTask::TemperatureTask(SMDSFiniteStateMachine* finiteStateMachine){
    active = true;
    fsm = finiteStateMachine;
}

void TemperatureTask::execute(){

}