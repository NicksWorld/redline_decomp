#include "enginestate.h"

#include <stdio.h>
#include <windows.h>

#include "globals.h"
#include "log.h"

#include "state/none.h"
#include "state/shutdown.h"

#define STATE_STUB(STATE_STUB_NAME) \
    bool STATE_STUB_NAME(int state) { \
        char state_name[64]; \
        StateName(state, state_name); \
        char buf[128]; \
        sprintf(buf, "Called state stub %s with state %s", #STATE_STUB_NAME, state_name); \
        g_Log.Debug(buf); \
        return false; \
    }

// Stubs to populate dispatch function pointers for debug
STATE_STUB(TickStub)
STATE_STUB(InitStub)
STATE_STUB(ShutdownStub)
STATE_STUB(UnkStub)

// STUB: REDLINE 0x00541e4b
bool EngineState::SetupStates() {
    memset(this->valid_transitions, 0, sizeof(this->valid_transitions));
    this->valid_transitions[STATE_DEBUG][STATE_NONE] = 1;
    this->valid_transitions[STATE_NONE][STATE_DEBUG] = 1;
    this->valid_transitions[STATE_NONE][STATE_INIT_AV] = 1;
    this->valid_transitions[STATE_DEBUG][STATE_INIT_AV] = 1;
    this->valid_transitions[STATE_DEBUG][STATE_NETWORK_SETUP] = 1;
    this->valid_transitions[STATE_INIT_AV][STATE_START_MENU] = 1;
    this->valid_transitions[STATE_INIT_AV][STATE_INTRO] = 1;
    this->valid_transitions[STATE_INIT_AV][STATE_GAME_INIT] = 1;
    this->valid_transitions[STATE_INTRO][STATE_START_MENU] = 1;
    this->valid_transitions[STATE_INTRO][STATE_VIDEO_PLAYBACK] = 1;
    this->valid_transitions[STATE_START_MENU][STATE_GAME_INIT] = 1;
    this->valid_transitions[STATE_START_MENU][STATE_NETWORK_SETUP] = 1;
    this->valid_transitions[STATE_GAME_INIT][STATE_GAME_PLAY] = 1;
    this->valid_transitions[STATE_GAME_INIT][STATE_CONSOLE_PLAY] = 1;
    this->valid_transitions[STATE_GAME_INIT][STATE_NETWORK_DOWNLOAD] = 1;
    this->valid_transitions[STATE_GAME_INIT][STATE_GAME_MENU] = 1;
    this->valid_transitions[STATE_GAME_PLAY][STATE_GAME_MENU] = 1;
    this->valid_transitions[STATE_CONSOLE_PLAY][STATE_GAME_MENU] = 1;
    this->valid_transitions[STATE_GAME_PLAY][STATE_VIDEO_PLAYBACK] = 1;
    this->valid_transitions[STATE_GAME_PLAY][STATE_NETWORK_TALLY] = 1;
    this->valid_transitions[STATE_GAME_PLAY][STATE_NETWORK_DOWNLOAD] = 1;
    this->valid_transitions[STATE_CONSOLE_PLAY][STATE_NETWORK_TALLY] = 1;
    this->valid_transitions[STATE_CONSOLE_PLAY][STATE_NETWORK_DOWNLOAD] = 1;
    this->valid_transitions[STATE_NETWORK_SETUP][STATE_DEBUG] = 1;
    this->valid_transitions[STATE_NETWORK_SETUP][STATE_GAME_INIT] = 1;
    this->valid_transitions[STATE_NETWORK_SETUP][STATE_START_MENU] = 1;
    this->valid_transitions[STATE_NETWORK_SETUP][STATE_INIT_AV] = 1;
    this->valid_transitions[STATE_NETWORK_SETUP][STATE_GAME_INIT] = 1;
    this->valid_transitions[STATE_NETWORK_DOWNLOAD][STATE_GAME_PLAY] = 1;
    this->valid_transitions[STATE_NETWORK_DOWNLOAD][STATE_GAME_PLAY] = 1;
    this->valid_transitions[STATE_NETWORK_DOWNLOAD][STATE_CONSOLE_PLAY] = 1;
    this->valid_transitions[STATE_NETWORK_TALLY][STATE_GAME_INIT] = 1;
    this->valid_transitions[STATE_NETWORK_TALLY][STATE_START_MENU] = 1;
    this->valid_transitions[STATE_GAME_MENU][STATE_GAME_INIT] = 1;
    this->valid_transitions[STATE_GAME_MENU][STATE_START_MENU] = 1;
    this->valid_transitions[STATE_VIDEO_PLAYBACK][STATE_GAME_PLAY] = 1;
    this->valid_transitions[STATE_VIDEO_PLAYBACK][STATE_START_MENU] = 1;
    this->valid_transitions[STATE_GAME_PLAY][STATE_START_MENU] = 1;
    this->valid_transitions[STATE_GAME_PLAY][STATE_GAME_INIT] = 1;
    this->valid_transitions[STATE_CONSOLE_PLAY][STATE_START_MENU] = 1;
    this->valid_transitions[STATE_CONSOLE_PLAY][STATE_GAME_INIT] = 1;
    for (int i = 0; i < NUM_STATES; ++i) {
        this->valid_transitions[i][STATE_SHUTDOWN] = 1;
    }
    for (int j = 0; j < NUM_STATES; ++j) {
        this->states[j] = new StateDispatchTable();
    }
    this->states[STATE_NONE]->tick = StateImpl::None::Tick;
    this->states[STATE_NONE]->unk = StateImpl::None::Unk;
    this->states[STATE_NONE]->init = StateImpl::None::Init;
    this->states[STATE_NONE]->shutdown = StateImpl::None::Shutdown;
    this->states[STATE_NONE]->flag = true;

    this->states[STATE_DEBUG]->tick = TickStub;
    this->states[STATE_DEBUG]->unk = UnkStub;
    this->states[STATE_DEBUG]->init = InitStub;
    this->states[STATE_DEBUG]->shutdown = ShutdownStub;
    this->states[STATE_DEBUG]->flag = true;

    this->states[STATE_INIT_AV]->tick = TickStub;
    this->states[STATE_INIT_AV]->unk = UnkStub;
    this->states[STATE_INIT_AV]->init = InitStub;
    this->states[STATE_INIT_AV]->shutdown = ShutdownStub;
    this->states[STATE_INIT_AV]->flag = true;

    this->states[STATE_INTRO]->tick = TickStub;
    this->states[STATE_INTRO]->unk = UnkStub;
    this->states[STATE_INTRO]->init = InitStub;
    this->states[STATE_INTRO]->shutdown = ShutdownStub;
    this->states[STATE_INTRO]->flag = true;

    this->states[STATE_START_MENU]->tick = TickStub;
    this->states[STATE_START_MENU]->unk = UnkStub;
    this->states[STATE_START_MENU]->init = InitStub;
    this->states[STATE_START_MENU]->shutdown = ShutdownStub;
    this->states[STATE_START_MENU]->flag = true;

    this->states[STATE_GAME_INIT]->tick = TickStub;
    this->states[STATE_GAME_INIT]->unk = UnkStub;
    this->states[STATE_GAME_INIT]->init = InitStub;
    this->states[STATE_GAME_INIT]->shutdown = ShutdownStub;
    this->states[STATE_GAME_INIT]->flag = true;

    this->states[STATE_GAME_PLAY]->tick = TickStub;
    this->states[STATE_GAME_PLAY]->unk = UnkStub;
    this->states[STATE_GAME_PLAY]->init = InitStub;
    this->states[STATE_GAME_PLAY]->shutdown = ShutdownStub;
    this->states[STATE_GAME_PLAY]->flag = true;

    this->states[STATE_VIDEO_PLAYBACK]->tick = TickStub;
    this->states[STATE_VIDEO_PLAYBACK]->unk = UnkStub;
    this->states[STATE_VIDEO_PLAYBACK]->init = InitStub;
    this->states[STATE_VIDEO_PLAYBACK]->shutdown = ShutdownStub;
    this->states[STATE_VIDEO_PLAYBACK]->flag = true;

    this->states[STATE_GAME_MENU]->tick = TickStub;
    this->states[STATE_GAME_MENU]->unk = UnkStub;
    this->states[STATE_GAME_MENU]->init = InitStub;
    this->states[STATE_GAME_MENU]->shutdown = ShutdownStub;
    this->states[STATE_GAME_MENU]->flag = true;

    this->states[STATE_NETWORK_SETUP]->tick = TickStub;
    this->states[STATE_NETWORK_SETUP]->unk = UnkStub;
    this->states[STATE_NETWORK_SETUP]->init = InitStub;
    this->states[STATE_NETWORK_SETUP]->shutdown = ShutdownStub;
    this->states[STATE_NETWORK_SETUP]->flag = true;

    this->states[STATE_NETWORK_DOWNLOAD]->tick = TickStub;
    this->states[STATE_NETWORK_DOWNLOAD]->unk = UnkStub;
    this->states[STATE_NETWORK_DOWNLOAD]->init = InitStub;
    this->states[STATE_NETWORK_DOWNLOAD]->shutdown = ShutdownStub;
    this->states[STATE_NETWORK_DOWNLOAD]->flag = true;

    this->states[STATE_NETWORK_TALLY]->tick = TickStub;
    this->states[STATE_NETWORK_TALLY]->unk = UnkStub;
    this->states[STATE_NETWORK_TALLY]->init = InitStub;
    this->states[STATE_NETWORK_TALLY]->shutdown = ShutdownStub;
    this->states[STATE_NETWORK_TALLY]->flag = true;

    this->states[STATE_CONSOLE_PLAY]->tick = TickStub;
    this->states[STATE_CONSOLE_PLAY]->unk = UnkStub;
    this->states[STATE_CONSOLE_PLAY]->init = InitStub;
    this->states[STATE_CONSOLE_PLAY]->shutdown = ShutdownStub;
    this->states[STATE_CONSOLE_PLAY]->flag = true;

    this->states[STATE_SHUTDOWN]->tick = TickStub;
    this->states[STATE_SHUTDOWN]->unk = UnkStub;
    this->states[STATE_SHUTDOWN]->init = InitStub;
    this->states[STATE_SHUTDOWN]->shutdown = StateImpl::Shutdown::Shutdown;
    this->states[STATE_SHUTDOWN]->flag = true;
    
    return true;
}

// FUNCTION: REDLINE 0x005424cb
int EngineState::GetQueuedState(bool peek) {
    int state;

    this->lock->Acquire(-1);
    if (this->transition_queue_len == 0) {
        this->lock->Release();
        return -1;
    } else {
        state = this->transition_queue[0];
        if (!peek) {
            if (this->transition_queue_len > 1) {
                int i = 0;
                for (; i < this->transition_queue_capacity - 1; i++) {
                    this->transition_queue[i] = this->transition_queue[i + 1];
                }
                this->transition_queue[i] = -1;
            } else {
                this->transition_queue[0] = -1;
            }
            this->transition_queue_len--;
        }
        this->lock->Release();
        return state;
    }
}

// FUNCTION: REDLINE 0x00541e37
int EngineState::GetState() { return this->active_state; }

// FUNCTION: REDLINE 0x00541720
void StateName(int state, char *out) {
    switch (state) {
    case 0:
        strcpy(out, "STATE_NONE");
        break;
    case 1:
        strcpy(out, "STATE_DEBUG");
        break;
    case 2:
        strcpy(out, "STATE_INIT_AV");
        break;
    case 3:
        strcpy(out, "STATE_INTRO");
        break;
    case 4:
        strcpy(out, "STATE_START_MENU");
        break;
    case 5:
        strcpy(out, "STATE_GAME_INIT");
        break;
    case 6:
        strcpy(out, "STATE_GAME_PLAY");
        break;
    case 7:
        strcpy(out, "STATE_VIDEO_PLAYBACK");
        break;
    case 8:
        strcpy(out, "STATE_GAME_MENU");
        break;
    case 9:
        strcpy(out, "STATE_NETWORK_SETUP");
        break;
    case 10:
        strcpy(out, "STATE_NETWORK_DOWNLOAD");
        break;
    case 11:
        strcpy(out, "STATE_NETWORK_TALLY");
        break;
    case 12:
        strcpy(out, "STATE_CONSOLE_PLAY");
        break;
    case 13:
        strcpy(out, "STATE_SHUTDOWN");
        break;
    default:
        strcpy(out, "STATE_NONE");
        break;
    }
}

// FUNCTION: REDLINE 0x005425bc
int EngineState::QueueState(int state) {
    this->lock->Acquire(-1);
    if (this->transition_queue_len == this->transition_queue_capacity) {
        g_Log.Debug("Attempt to queue state failed - Maxed out");
        this->lock->Release();
        return -1;
    } else {
        this->transition_queue[this->transition_queue_len] = state;
        this->transition_queue_len++;
        char state_name[64];
        char msg[256];
        StateName(state, state_name);
        sprintf(msg, "   --- queue  %s", state_name);
        g_Log.Debug(msg);
        this->lock->Release();
    }
    return state;
}

// STUB: REDLINE 0x00541ac5
int EngineState::ChangeState(int state) {
    this->lock->Acquire(-1);
    this->unk = true;
    if (state > 0xe || -1 > state) {
        this->unk = false;
        this->lock->Release();
        return this->active_state;
    }

    char dest[64];
    char orig[64];
    StateName(this->active_state, orig);
    StateName(state, dest);
    char msg[256];
    sprintf(msg, "   --- transition  %s to %s", orig, dest);
    g_Log.Debug(msg);

    // TODO: Something about console enablement?

    // if (this->states[this->active_state]->unk == true) {
    // TODO: Unlocks some global mutex
    // }
    // TODO calls a NOP

    if (!this->states[this->active_state]->shutdown(state)) {
        this->unk = false;
        this->queue_lock->Release();
        g_Log.Debug("   --- Previous state's shutdown failed");
        return this->active_state;
    }

    if (!this->states[state]->init(this->active_state)) {
        this->unk = false;
        this->queue_lock->Release();
        g_Log.Debug("   --- new state's Initialization failed");
        return this->active_state;
    }

    // g_StateTick = this->states[state]->tick;
    // g_StateUnk = this->states[state]->unk;
    if (this->states[state]->flag) {
        // one method
    } else {
        // other
    }
    this->active_state = state;
    if (this->states[this->active_state]->flag) {
        // unlock some global mutex
    }
    // NOP call
    StateName(this->active_state, dest);
    sprintf(msg, "   --- Transition to %s complete", dest);
    g_Log.Debug(msg);
    this->unk = 0;
    this->queue_lock->Release();
    // Set a global to 0
    // set global time var
    return this->active_state;
}

// FUNCTION: REDLINE 0x00542440
bool EngineState::IsValidStateChange(int src, int dst) {
    if (this->valid_transitions[src][dst] == true)
        return true;
    char buf[128];
    char src_name[64];
    char dst_name[64];
    StateName(src, src_name);
    StateName(dst, dst_name);
    sprintf(buf, "*Error: Illegal state change attempted. ( from: %s  to:%s)",
            src_name, dst_name);
    g_Log.Debug(buf);
    return false;
}

// FUNCTION: REDLINE 0x005418ad
EngineState::EngineState() {
    this->active_state = STATE_SHUTDOWN;
    for (int i = 0; i < 5; i++) {
        this->transition_queue[i] = -1;
    }
    this->transition_queue_len = 0;
    this->transition_queue_capacity = 5;

    if (!this->SetupStates())
        return;

    // *((int *)0x005cd178) = 0x00540c98; // Function pointer
    // *((int *)0x005cd174) = 0x00540bd0; // Function pointer
    this->unk = false;
    this->lock = new Mutex(NULL);
    this->queue_lock = new Mutex(NULL);
}

// FUNCTION: REDLINE 0x00471002
StateTree::~StateTree() {
    for (short i = 0; i < this->capacity; ++i) {
        if (this->nodes[i] != NULL) {
            delete this->nodes[i];
            this->nodes[i] = NULL;
        }
    }
    if (this->nodes != NULL) {
        delete this->nodes;
        this->nodes = NULL;
    }
}

// FUNCTION: REDLINE 0x00470F8C
StateTree::StateTree() {
    this->length = 0;
    this->capacity = 10;
    this->nodes = new StateNode *[this->capacity]();
    for (short i = 0; i < this->capacity; ++i) {
        this->nodes[i] = NULL;
    }
}

// FUNCTION: REDLINE 0x004710CB
void StateTree::AddNode(StateNode *node) {
    if (this->length == this->capacity) {
        short initial_capacity = this->capacity;
        this->capacity += 10;
        StateNode **new_nodes = new StateNode *[this->capacity]();
        short i;
        for (i = 0; i < this->capacity; ++i) {
            new_nodes[i] = NULL;
        }
        for (i = 0; i < initial_capacity; ++i) {
            new_nodes[i] = this->nodes[i];
        }
        if (this->nodes != NULL) {
            delete this->nodes;
            this->nodes = NULL;
        }
        this->nodes = new_nodes;
    }
    StateNode *copied = new StateNode(*node);
    for (short k = 0; k < this->capacity; k++) {
        if (!this->nodes[k]) {
            this->nodes[k] = copied;
            this->length++;
            return;
        }
    }
}

// FUNCTION: REDLINE 0x004713C4
void StateTree::PopulateNodes() {
    StateNode node;
    node.id = 0;
    node.a = 1;
    node.b = 0;
    node.state_id = 0;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 1;
    node.a = 2;
    node.b = 0;
    node.state_id = 0;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 2;
    node.a = 3;
    node.b = 9;
    node.state_id = 1;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 3;
    node.a = 4;
    node.b = 0;
    node.state_id = 2;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 4;
    node.a = 6;
    node.b = 0;
    node.state_id = 3;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 5;
    node.a = 6;
    node.b = 0;
    node.state_id = 7;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 6;
    node.a = 7;
    node.b = 0;
    node.state_id = 4;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 7;
    node.a = 8;
    node.b = 0;
    node.state_id = 5;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 8;
    node.a = 0;
    node.b = 6;
    node.state_id = 6;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 9;
    node.a = 10;
    node.b = 0;
    node.state_id = 9;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 10;
    node.a = 11;
    node.b = 0;
    node.state_id = 2;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 11;
    node.a = 12;
    node.b = 0;
    node.state_id = 5;
    strcpy(node.name, "selected");
    this->AddNode(&node);
    node.id = 12;
    node.a = 13;
    node.b = 6;
    node.state_id = 10;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 13;
    node.a = 14;
    node.b = 6;
    node.state_id = 6;
    strcpy(node.name, "selected");
    this->AddNode(&node);
    node.id = 14;
    node.a = 11;
    node.b = 6;
    node.state_id = 11;
    strcpy(node.name, "");
    this->AddNode(&node);
    node.id = 18;
    node.a = 0;
    node.b = 0;
    node.state_id = 13;
    strcpy(node.name, "");
    this->AddNode(&node);
    this->cur_node = 0;
}

// FUNCTION: REDLINE 0x00471285
StateNode *StateTree::NodeById(short id) {
    for (short i = 0; i < this->capacity; ++i) {
        if (this->nodes[i] && this->nodes[i]->id == id)
            return this->nodes[i];
    }
    return NULL;
}

// FUNCTION: REDLINE 0x00471344
StateNode *StateTree::Next(int dir) {
    StateNode *cur = this->NodeById(this->cur_node);
    if (cur == NULL) {
        return NULL;
    }
    if (dir == 1) {
        this->cur_node = cur->a;
    } else {
        this->cur_node = cur->b;
    }
    return this->NodeById(this->cur_node);
}

// Intrinsic forced as it is implemented explicitly to require SEH
// This almost certainly was originally a default impl, but for some reason
// this helps?
#pragma intrinsic(memcpy)
// FUNCTION: REDLINE 0x00470F60
StateNode::StateNode(const StateNode &other) {
    memcpy((char *)this, &other, sizeof(StateNode));
}
