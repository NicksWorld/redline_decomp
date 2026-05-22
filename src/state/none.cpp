#include "none.h"

#include "../config.h"
#include "../enginestate.h"
#include "../globals.h"
#include "../keybinds.h"
#include "../log.h"

// GLOBAL: REDLINE 0x005cd124
bool g_StartupQueued = false;

// FUNCTION: REDLINE 0x0053fb5d
bool StateImpl::None::Shutdown(int next_state) { return true; }

// FUNCTION: REDLINE 0x0053f620
bool StateImpl::None::Unk(int smthn) { return true; }

// STUB: REDLINE 0x0053F687
bool StateImpl::None::Init(int from) {
    g_Log.Debug("Initializing event handler");
    // TODO: Event handler creation
    g_Log.Debug("   --- Starting Heartbeat thread");
    // TODO Global timer id?
    InitKeybinds();
    // TODO ThreadID stuff
    if (!g_Config) {
        g_Config = new Config();
        g_Config->Load();
        g_Config->ApplyKeybinds(false);

        for (int i = 0; i < 2; ++i) {
            DefaultConfValue* values;
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
                switch(values[j].kind) {
                    case VALUE_SIGNED:
                        res = g_Config->GetIntValue(values[j].name, &integer);
                        if (res >= 0)
                            *(int*)values[j].value = integer;
                        break;
                    case VALUE_FLOAT:
                        res = g_Config->GetFloatValue(values[j].name, &decimal);
                        if (res >= 0)
                            *(float*)values[j].value = decimal;
                        break;
                    case VALUE_INTBOOL:
                        res = g_Config->GetIntValue(values[j].name, &integer);
                        if (res >= 0) {
                            if (integer)
                                *(int*)values[j].value= 1;
                            else
                                *(int*)values[j].value= 0;
                        }
                        break;
                    case VALUE_CHARBOOL:
                        res = g_Config->GetBoolValue(values[j].name, &boolean);
                        if (res >= 0) {
                            if (boolean)
                                *(bool*)values[j].value= 1;
                            else
                                *(bool*)values[j].value= 0;
                        }
                        break;
                    case VALUE_STRING:
                        res = g_Config->GetStringValue(values[j].name, (char*) values[j].value);
                        break;
                }
            }
        }
    }

    // TODO: Stuff to do with *actually* making the thread
    return true;
}

// FUNCTION: REDLINE 0x0053f627
bool StateImpl::None::Tick(int state) {
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
