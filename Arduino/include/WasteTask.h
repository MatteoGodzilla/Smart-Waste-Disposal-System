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
#define WASTE_THRESHOLD 0.95
#define TIME_TO_CLOSE 10000
#define FULL_TOLERANCE_TIME 1000
#define RELEASE_TIME 5000
/* maybe change later m/s */
#define SOUND_SPEED 343.4
/* distance in m */
#define EMPTY_DISTANCE 0.50
/* distance in m */
#define FULL_DISTANCE 0.10
/* range of possible waste level values */
#define DISTANCE_RANGE (EMPTY_DISTANCE - FULL_DISTANCE)

class WasteTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
    SerialCommunicator* scTask;
    Servo motor;
    unsigned long timeSince;
    signed short angle;
    int buttonState;
    float fillPercentage;
    bool timeset;
    bool opening;
    bool closing;
    bool wasteReleased;
    bool endAlarmManaged;
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