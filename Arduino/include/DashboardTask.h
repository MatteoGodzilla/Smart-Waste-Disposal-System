#pragma once
#include "Task.h"

class DashboardTask : public Task {
public:
    DashboardTask();
    virtual void execute() override;
};