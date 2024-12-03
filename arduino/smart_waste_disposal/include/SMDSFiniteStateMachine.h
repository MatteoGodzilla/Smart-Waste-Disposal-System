#pragma once

enum State{
    AVAILABLE,
    ACCEPTING_WASTE,
    WASTE_RECEIVED,
    FULL,
    EMPTYING,
    OVERHEATING,
    SLEEPING
};

class SMDSFiniteStateMachine {
public:
    State state;
    SMDSFiniteStateMachine();
};