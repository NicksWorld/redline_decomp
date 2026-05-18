#include "none.h"

#include "../enginestate.h"
#include "../globals.h"

// GLOBAL: REDLINE 0x005cd124
bool g_StartupQueued = false;

// FUNCTION: REDLINE 0x0053fb5d
bool StateImpl::None::Shutdown(int next_state) {
    return true;
}

// FUNCTION: REDLINE 0x0053f620
bool StateImpl::None::Unk(int smthn) {
    return true;
}

// FUNCTION: REDLINE 0x0053f627
bool StateImpl::None::Tick(int state) {
    if (g_StartupQueued) return true;
    StateNode* next = g_StateTree->Next(1);
    if (next == NULL) return false;

    int queued_id =  g_EngineState->QueueState(next->state_id);
    if (queued_id != next->state_id) return false;
    g_StartupQueued = true;
    return true;
}
