#pragma once

#include "Task.h"

#define NUMTASKS 5

class Scheduler {

private:
    int nTask;
    Task* tasks[NUMTASKS];

public:
    void init();
    virtual bool addTask(Task* task);
    virtual void schedule();

};