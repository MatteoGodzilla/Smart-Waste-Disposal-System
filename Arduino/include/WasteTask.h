#pragma once
#include "Task.h"

class WasteTask : public Task {
public:
    WasteTask();
    virtual void execute() override;
};