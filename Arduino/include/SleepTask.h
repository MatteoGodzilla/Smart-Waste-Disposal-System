#pragma once
#include "Task.h"

class SleepTask : public Task {
public:
    SleepTask();
    virtual void execute() override;
};