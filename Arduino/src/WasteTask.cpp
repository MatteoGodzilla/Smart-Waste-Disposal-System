#include "WasteTask.h"

WasteTask::WasteTask(){
    active = true;
    state = ACCEPTING_WASTE;
}

void WasteTask::execute(){
    transitionState();
    executeState();
}

void WasteTask::transitionState(){
    switch(state){
        case AVAILABLE:
            break;
        case ACCEPTING_WASTE:
            break;
        case FULL:
            break;
    }
}

void WasteTask::executeState(){
    switch(state){
        case AVAILABLE:
            break;
        case ACCEPTING_WASTE:
            break;
        case FULL:
            break;
    }
}
