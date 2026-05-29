#include "none.h"

#include "../config.h"
#include "../enginestate.h"
#include "../globals.h"
#include "../keybinds.h"
#include "../log.h"
#include "../mutex.h"
#include "../replay.h"

// GLOBAL: REDLINE 0x005cd124
bool g_StartupQueued = false;

// FUNCTION: REDLINE 0x0053fb5d
bool StateImpl::None::Shutdown(int next_state) { return true; }

// FUNCTION: REDLINE 0x0053f620
bool StateImpl::None::Tick() { return true; }

// GLOBAL: REDLINE 0x005cebe4
int g_LastTickTime = 0;

// GLOBAL: REDLINE 0x005cebcc
bool g_HeartbeatKill = 0;

// GLOBAL: REDLINE 0x005ce5f0
Mutex g_TickMutex = Mutex(NULL);

// FUNCTION: REDLINE 0x005539FE
void heartbeat_tick() {
    if (g_HeartbeatKill) {
        g_HeartbeatThread = 0;
        ExitThread(0);
    }
    g_TickMutex.Acquire(-1);
    if (g_replayPlay || g_replayRecord) g_Replay->Tick();
    if (g_HeartbeatKill) {
        g_HeartbeatThread = 0;
        g_TickMutex.Release();
        ExitThread(0);
    }
    g_StateTick();
    g_TickMutex.Release();
}

// FUNCTION: REDLINE 0x005539AE
unsigned long __stdcall heartbeat_fn(void *_unused) {
    DWORD time;
    while (true) {
        unsigned int delta;
        do {
            time = timeGetTime();
            delta = time - g_LastTickTime;
            if (delta < 33)
                Sleep(0);
        } while (delta < 33);

        g_LastTickTime = time;
        heartbeat_tick();
    }
    return 0;
}

// STUB: REDLINE 0x0053F687
bool StateImpl::None::Init(int from) {
    g_Log.Debug("Initializing event handler");
    // TODO: Event handler creation
    g_Log.Debug("   --- Starting Heartbeat thread");
    // TODO Global timer id?
    InitKeybinds();
    // TODO ThreadID stuff
    g_carSteerInc = 13;
    g_carSteerMax = 36;
    g_footSteerInc = 13;
    g_footSteerMax = 125;
    if (!g_Config) {
        g_Config = new Config();
        g_Config->Load();
        g_Config->ApplyKeybinds(false);

        for (int i = 0; i < 2; ++i) {
            DefaultConfValue *values;
            int count;
            if (i == 0) {
                values = g_ConfDefaultDebug;
                count = 19;
            } else {
                values = g_ConfDefault;
                count = 72;
            }
            for (int j = 0; j < count; ++j) {
                int res;
                int integer;
                float decimal;
                bool boolean;
                switch (values[j].kind) {
                case VALUE_SIGNED:
                    res = g_Config->GetIntValue(values[j].name, &integer);
                    if (res >= 0)
                        *(int *)values[j].value = integer;
                    break;
                case VALUE_FLOAT:
                    res = g_Config->GetFloatValue(values[j].name, &decimal);
                    if (res >= 0)
                        *(float *)values[j].value = decimal;
                    break;
                case VALUE_INTBOOL:
                    res = g_Config->GetIntValue(values[j].name, &integer);
                    if (res >= 0) {
                        if (integer)
                            *(int *)values[j].value = 1;
                        else
                            *(int *)values[j].value = 0;
                    }
                    break;
                case VALUE_CHARBOOL:
                    res = g_Config->GetBoolValue(values[j].name, &boolean);
                    if (res >= 0) {
                        if (boolean)
                            *(bool *)values[j].value = 1;
                        else
                            *(bool *)values[j].value = 0;
                    }
                    break;
                case VALUE_STRING:
                    res = g_Config->GetStringValue(values[j].name,
                                                   (char *)values[j].value);
                    break;
                }
            }
        }
    }

    g_Windowed = 0;
    g_DebugMouse = 0;
    g_AIActive = 1;
    if (g_replayRecord | g_replayPlay) {
        // TODO: Initialize replay class, and setting a static seed if recording
    }

    DWORD threadId;
    g_HeartbeatThread = CreateThread(0, 0, heartbeat_fn, NULL, 0, &threadId);
    if (!g_HeartbeatThread)
        g_Log.Debug("*Error: Failed to setup Heartbeat thread");
    g_MainThread = GetCurrentThread();
    // TODO Some stuff?
    return true;
}

// FUNCTION: REDLINE 0x0053f627
bool StateImpl::None::EventTick() {
    if (g_StartupQueued)
        return true;
    StateNode *next = g_StateTree->Next(1);
    if (next == NULL)
        return false;

    int queued_id = g_EngineState->QueueState(next->state_id);
    if (queued_id != next->state_id)
        return false;
    g_StartupQueued = true;
    return true;
}
