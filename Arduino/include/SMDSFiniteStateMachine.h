#pragma once

enum State{
    AVAILABLE,
    ACCEPTING_WASTE,
    FULL,
    OVERHEATING,
    SLEEPING
};

class SMDSFiniteStateMachine {
public:
    State state;
    SMDSFiniteStateMachine();
};