#include "Scheduler.h"

//TODO:REMOVE
#include <Arduino.h>

void Scheduler::init(){
    nTask = 0;
}

bool Scheduler::addTask(Task* task){
    if(nTask < NUMTASKS-1) {
        tasks[nTask] = task;
        nTask++;
        return true;
    } else {
        return false;
    }
}

void Scheduler::schedule() {
    for (int i = 0; i < nTask; i++) {
        if(tasks[i]->isActive()) {
            tasks[i]->execute();
        }
    }
}