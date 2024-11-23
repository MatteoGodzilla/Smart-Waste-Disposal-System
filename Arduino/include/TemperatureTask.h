#pragma once
#include <Arduino.h>
#include "Task.h"
#include "SMDSFiniteStateMachine.h"
#include "DashboardTask.h"
#include "pins.h"

#define MAXTEMP 30
#define MAXTEMPTIME 2000

class TemperatureTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
    DashboardTask* dashboardTask;
    float tempPrev = 0.0;
    unsigned long time;
public:
    TemperatureTask();
    void bindFSM(SMDSFiniteStateMachine* fsmTask);
    void bindDashboard(DashboardTask* dTask);
    virtual void execute() override;
};