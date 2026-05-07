#include <windows.h>
#include <stdio.h>

class Log {
    char filename[0x100];
    public:
    void Debug(char* msg);
};

// GLOBAL: REDLINE 0x005c3f70
Log g_Log;

// FUNCTION: REDLINE 0x004a94a1
void Log::Debug(char* msg) {}

class Mutex {
    HANDLE lock;
    char* name;
    int state;

public:
    Mutex(char* name);
    ~Mutex();
    bool Acquire(DWORD timeout);
    bool Release();
};

// FUNCTION: REDLINE 0x004d2970
Mutex::Mutex(char* name) {
    this->state = 0;
    if (name != NULL) {
        this->name = new char[strlen(name) + 1];
        strcpy(this->name, name);
    } else {
        this->name = NULL;
    }
    this->lock = NULL;
    this->lock = CreateMutexA(0, false, this->name);
}

// FUNCTION: REDLINE 0x004d29f4
Mutex::~Mutex() {
    if (this->state != 0) {
        this->Release();
    }
    if (this->name != NULL) {
        delete this->name;
    }
    CloseHandle(this->lock);
}

// FUNCTION: REDLINE 0x004d2a3c
bool Mutex::Acquire(DWORD timeout) {
    DWORD res = WaitForSingleObject(this->lock, timeout);
    if (res == 0) {
        this->state = 1;
    }

    return res != 0;
}

// FUNCTION: REDLINE 0x004d2a77
bool Mutex::Release() {
    this->state = 0;
    BOOL res = ReleaseMutex(this->lock);
    return res != 1;
}

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

// STUB: REDLINE 0x00541e4b
bool EngineState::SetupStates() { return 0; }

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
int EngineState::GetState() {
    return this->active_state;
}

// FUNCTION: REDLINE 0x00541720
void StateName(int state, char* out) {
    switch(state) {
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

// FUNCTION: REDLINE 0x005418ad
EngineState::EngineState() {
    this->active_state = 0xd;
    for (int i = 0; i < 5; i++) {
        this->transition_queue[i] = -1;
    }
    this->transition_queue_len = 0;
    this->transition_queue_capacity = 5;

    if (!this->SetupStates()) return;

    *((int*)0x005cd178) = 0x00540c98; // Function pointer
    *((int*)0x005cd174) = 0x00540bd0; // Function pointer
    this->unk = false;
    this->lock = new Mutex(NULL);
    this->queue_lock = new Mutex(NULL);
}
// STUB: REDLINE 0x00551d73
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    return 0;
}
