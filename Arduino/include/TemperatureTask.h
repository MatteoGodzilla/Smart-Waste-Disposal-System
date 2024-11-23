#pragma once
#include <Arduino.h>
#include "Task.h"
#include "SMDSFiniteStateMachine.h"
#include "SerialCommunicator.h"
#include "pins.h"

#define MAXTEMP 30
#define MAXTEMPTIME 2000

class TemperatureTask : public Task {
private:
    SMDSFiniteStateMachine* fsm;
    SerialCommunicator* scTask;
    float tempPrev = 0.0;
    unsigned long time;
public:
    TemperatureTask();
    void bindFSM(SMDSFiniteStateMachine* fsmTask);
    void bindSerialCommunicator(SerialCommunicator* dTask);
    void onFixTemperatureEvent();
    virtual void execute() override;
};