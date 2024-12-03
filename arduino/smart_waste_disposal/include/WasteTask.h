#pragma once
#include <Arduino.h>
#include <Servo.h>
#include "Task.h"
#include "pins.h"
#include "SMDSFiniteStateMachine.h"
#include "SerialCommunicator.h"
#include "LCDManager.h"
#include "ElapsedTimer.h"

#define OPEN_ANGLE 0
#define CLOSED_ANGLE 90
#define EMPTYING_ANGLE 180

/* TIMERS CONSTANTS */
#define TIME_TO_CLOSE 10000
#define FULL_TOLERANCE_TIME 1000
#define EMPTYING_TIME 5000
#define ACCEPTED_WASTE_DELAY 4000

/* MAXIMUM WASTE FILLING PERCENTAGE */
#define WASTE_THRESHOLD 0.95

/* SPEED OF LIGHT (meters/seconds) */
#define SOUND_SPEED 343.4
/* WASTE DISTANCE FROM SONAR WHEN THE BIN IS EMPTY (meters)*/
#define EMPTY_DISTANCE 0.50
/* WASTE DISTANCE FROM SONAR WHEN THE BIN IS FULL (meters) */
#define FULL_DISTANCE 0.10
/* DISTANCE RANGE BETWEEN EMPTY AND FULL (meters) */
#define DISTANCE_RANGE (EMPTY_DISTANCE - FULL_DISTANCE)

class WasteTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
    SerialCommunicator* scTask;
    Servo motor;

    ElapsedTimer fullTolleranceTimer;
    ElapsedTimer userCanSpillTimer;
    ElapsedTimer showAcceptedWasteTimer;
    ElapsedTimer emptyingSimulationTimer;
    bool receivedEmptyEvent;
    float fillPercentage;
    void setAlarm(bool alarmed);
    void updateTimers();
    void executeState();
    void changeState();
    float getFillPercentage();
public:
    WasteTask();
    void bindFSM(SMDSFiniteStateMachine* fsmTask);
    void bindSerialCommunicator(SerialCommunicator* dTask);
    void onEmptyEvent();
    virtual void execute() override;
};