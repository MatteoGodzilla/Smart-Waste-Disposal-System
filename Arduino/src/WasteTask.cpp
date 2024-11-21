#include "WasteTask.h"

WasteTask::WasteTask(SMDSFiniteStateMachine* finiteStateMachine){
    active = true;
    fsm = finiteStateMachine;
}

void WasteTask::execute(){
    changeState();
    executeState();
}

void WasteTask::changeState(){
    switch(fsm->state){
        case AVAILABLE:
            break;
        case ACCEPTING_WASTE:
            break;
        case FULL:
            break;
    }
}

void WasteTask::executeState(){
    switch(fsm->state){
        case AVAILABLE:
            break;
        case ACCEPTING_WASTE:
            break;
        case FULL:
            break;
    }
}
