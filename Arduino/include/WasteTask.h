#pragma once
#include "Task.h"

enum WasteState{
    AVAILABLE,
    ACCEPTING_WASTE,
    FULL
};

class WasteTask : public Task {
private:
    WasteState state;
    void executeState();
    void transitionState();
public:
    WasteTask();
    virtual void execute() override;
};