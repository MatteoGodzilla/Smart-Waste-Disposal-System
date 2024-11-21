#pragma once
#include "Task.h"

class TemperatureTask : public Task {
public:
    TemperatureTask();
    virtual void execute() override;
};