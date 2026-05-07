#pragma once

#include "mutex.h"

#define STATE_COUNT 14

struct State {};

class EngineState {
    bool valid_transitions[STATE_COUNT][STATE_COUNT];
    State* states[14];
    int active_state;
    int transition_queue[5];
    int transition_queue_len;
    int transition_queue_capacity;
    bool unk;
    Mutex* lock;
    Mutex* queue_lock;

    public:
    EngineState();
    bool SetupStates();
    int GetState();
    int GetQueuedState(bool peek);
    int QueueState(int state);
};
