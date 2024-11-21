#pragma once

enum State{
    AVAILABLE,
    ACCEPTING_WASTE,
    WASTE_RECEIVED,
    FULL,
    OVERHEATING,
    SLEEPING
};

class SMDSFiniteStateMachine {
public:
    State state;
    SMDSFiniteStateMachine();
};