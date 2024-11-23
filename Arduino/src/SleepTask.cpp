#include "SleepTask.h"

SleepTask::SleepTask(){
    active = true;
}

void SleepTask::bindFSM(SMDSFiniteStateMachine* fsmTask) {
    fsm = fsmTask;
}

void SleepTask::execute(){

}