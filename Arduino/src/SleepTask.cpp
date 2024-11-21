#include "SleepTask.h"

SleepTask::SleepTask(SMDSFiniteStateMachine* finiteStateMachine){
    active = true;
    fsm = finiteStateMachine;
}

void SleepTask::execute(){

}