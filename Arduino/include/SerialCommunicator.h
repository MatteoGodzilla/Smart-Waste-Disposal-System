#pragma once
#include "Task.h"

//forward declaration of tasks required, in order to break the cyclic dependency
class TemperatureTask;
class WasteTask;

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