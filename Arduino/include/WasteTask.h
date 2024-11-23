#pragma once
#include <Arduino.h>
#include <Servo.h>
#include "Task.h"
#include "pins.h"
#include "SMDSFiniteStateMachine.h"
#include "DashboardTask.h"
#include "LCDManager.h"

#define MAX_ANGLE 90
#define MIN_ANGLE 0
#define MAX_WASTE 7
/* modify this placeholder value */
#define SPILL_TIME 10000
/* maybe change later m/s */
#define SOUND_SPEED 343.4
/* distance in cm */
#define EMPTY_DISTANCE 5
/* distance in cm */
#define FULL_DISTANCE 1
#define DISTANCE_RANGE (EMPTY_DISTANCE - FULL_DISTANCE)

class WasteTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
    DashboardTask* dashboardTask;
    Servo motor;
    signed short angle;
    void executeState();
    void changeState();
    float getFillPercentage();
public:
    WasteTask();
    void bindFSM(SMDSFiniteStateMachine* fsmTask);
    void bindDashboard(DashboardTask* dTask);
    virtual void execute() override;
};