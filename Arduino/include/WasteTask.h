#pragma once
#include <Arduino.h>
#include <Servo.h>
#include "Task.h"
#include "pins.h"
#include "SMDSFiniteStateMachine.h"
#include "SerialCommunicator.h"
#include "LCDManager.h"

#define OPEN_ANGLE 90
#define CLOSED_ANGLE  0
#define RELEASE_ANGLE -90
#define TIME_TO_CLOSE 10000
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
    SerialCommunicator* scTask;
    Servo motor;
    unsigned long openSince;
    signed short angle;
    int buttonState;
    bool timeset;
    bool opening;
    bool closing;
    bool wasteReleased;
    bool fullAlarmManaged;
    bool temperatureAlarmManaged;
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