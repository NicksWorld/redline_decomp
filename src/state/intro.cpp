#include "intro.h"

#include "../globals.h"
#include "../enginestate.h"
#include "../render.h"

// GLOBAL: REDLINE 0x0059B7D4
bool g_IntroShow = true;

// FUNCTION: REDLINE 0x0053FFF0
bool StateImpl::Intro::Init(int prev_state) {
    if (g_ConnectDirect || g_ConsoleEnabled) {
        g_IntroShow = false;
    }

    if (g_ConnectDirect == 1) {
        // dword_5CD130 = 1;
        // dword_59B7FC = 1;
        for (int i = 0; i < 1; ++i) {
            // TODO
        }
    }
    return true;
}

// FUNCTION: REDLINE 0x0054022E
bool StateImpl::Intro::Shutdown(int state) {
    return true;
}

// FUNCTION: REDLINE 0x00540108
bool StateImpl::Intro::Tick() {
    return true;
}

// FUNCTION: REDLINE 0x0054034D
void SkipIntro() {
    StateNode* next = g_StateTree->Next(1);
    if (next && next->state_id == 7) {
        if (!g_IntroShow) {
            strcpy(next->name, "skip");
        } else {
            strcpy(next->name, "towr10a.smk");
        }
    }
    g_EngineState->QueueState(next->state_id);
}

// FUNCTION: REDLINE 0x00540172
bool StateImpl::Intro::EventTick() {
    if (!g_IntroShow) {
        SkipIntro();
        return true;
    }

    if (g_ConsoleEnabled)
        return true;

    LockRender();
    ClearViewport(2);
    // TODO: Render lists?
    // sub_43C1A9 = zeroes a ton of stuff
    // sub_43BD50 = fills stuff
    if (BeginScene()) {

    }
    if (EndScene()) {
        FlipDisplay();
    }
    // TODO
    UnlockRender();

    return true;
}
