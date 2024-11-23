#pragma once
#include "Task.h"
#include "TemperatureTask.h"
#include "WasteTask.h"

class SerialCommunicator : public Task {
private:
    TemperatureTask* temperatureTask;
    WasteTask* wasteTask;

public:
    SerialCommunicator();
    void bindTemp(TemperatureTask* tTask);
    void bindWaste(WasteTask* wTask);

    void sendTemperature(float celsius);
    void sendFillPercentage(float percentage);
    virtual void execute() override;
};