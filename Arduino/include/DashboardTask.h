#pragma once
#include "Task.h"
#include "TemperatureTask.h"
#include "WasteTask.h"

class DashboardTask : public Task {
private:
    TemperatureTask* temperatureTask;
    WasteTask* wasteTask;

public:
    DashboardTask();
    void bindTemp(TemperatureTask* tTask);
    void bindWaste(WasteTask* wTask);
    virtual void execute() override;
};