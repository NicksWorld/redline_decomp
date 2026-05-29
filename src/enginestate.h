#pragma once

#include "mutex.h"

#define STATE_COUNT 14

enum State {
    STATE_NONE = 0,
    STATE_DEBUG = 1,
    STATE_INIT_AV = 2,
    STATE_INTRO = 3,
    STATE_START_MENU = 4,
    STATE_GAME_INIT = 5,
    STATE_GAME_PLAY = 6,
    STATE_VIDEO_PLAYBACK = 7,
    STATE_GAME_MENU = 8,
    STATE_NETWORK_SETUP = 9,
    STATE_NETWORK_DOWNLOAD = 10,
    STATE_NETWORK_TALLY = 11,
    STATE_CONSOLE_PLAY = 12,
    STATE_SHUTDOWN = 13,
    NUM_STATES = 14
};

struct StateDispatchTable {
    bool (*event_tick)();
    bool (*tick)();
    bool (*init)(int state);
    bool (*shutdown)(int state);
    bool flag;
};

void StateName(int state, char *out);

class EngineState {
    bool valid_transitions[STATE_COUNT][STATE_COUNT];
    StateDispatchTable *states[14];
    int active_state;
    int transition_queue[5];
    int transition_queue_len;
    int transition_queue_capacity;
    bool unk;
    Mutex *lock;
    Mutex *queue_lock;

  public:
    EngineState();
    bool SetupStates();
    int GetState();
    int GetQueuedState(bool peek);
    int QueueState(int state);
    int ChangeState(int state);
    bool IsValidStateChange(int src, int dst);
};

struct StateNode {
  public:
    short id;

    // Branches
    short a;
    short b;

    int state_id;
    char name[128];

  public:
    StateNode() {}
    StateNode(const StateNode &other);
};

class StateTree {
    short length;
    short capacity;
    StateNode **nodes;
    unsigned short cur_node;
    short unk;

  public:
    StateTree();
    void AddNode(StateNode *node);
    void PopulateNodes();
    StateNode *NodeById(short id);
    StateNode *Next(int dir);
    ~StateTree();
};
