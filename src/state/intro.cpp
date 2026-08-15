#include "intro.h"

#include "../globals.h"
#include "../enginestate.h"
#include "../render.h"
#include "../interface.h"

// GLOBAL: REDLINE 0x0059B7D4
bool g_IntroShow = true;

// GLOBAL: REDLINE 0x005CD130
int g_IntroUnk1 = 1;
// GLOBAL: REDLINE 0x0059B7FC
int g_IntroUnk2 = 1;

// GLOBAL: REDLINE 0x005CD12C
Widget* g_IntroWidgets[2];

// FUNCTION: REDLINE 0x0053FFF0
bool StateImpl::Intro::Init(int prev_state) {
    if (g_ConnectDirect || g_ConsoleEnabled) {
        g_IntroShow = false;
    }

    if (g_IntroShow == 1) {
        g_IntroUnk1 = 1;
        g_IntroUnk2 = 1;
        for (int i = 0; i < 1; ++i) {
            GraphicWidget* widget = (GraphicWidget*)CreateWidget(WIDGET_GRAPHIC, NULL);
            if (widget) {
                g_IntroWidgets[i] = widget;
                g_Interface->AddControl(widget);
                widget->SetUnkFloats(0, 0, 0);
                widget->SetDescription("Intro Gfx");
                widget->unk_92 = 0;
                // TODO
                widget->unk_94 = 1;
                // TODO: Legal string is part of a table indexed by i
                widget->SetImage("Legal", 0, 0, 640, 480, 0, 0, 0);
            }
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
        // TODO
        g_Interface->Render(1);
        // TODO
    }
    if (EndScene()) {
        FlipDisplay();
    }
    if (!g_IntroUnk1)
        SkipIntro();
    UnlockRender();

    return true;
}
