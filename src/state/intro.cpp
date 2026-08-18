#include "intro.h"

#include "../globals.h"
#include "../enginestate.h"
#include "../render.h"
#include "../render_helper.h"
#include "../interface.h"

// GLOBAL: REDLINE 0x0059B7D4
bool g_IntroShow = true;

// GLOBAL: REDLINE 0x005CD130
int g_EndIntroSplash = 1;
// GLOBAL: REDLINE 0x0059B7FC
int g_IntroUnk2 = 1;

// GLOBAL: REDLINE 0x005CD134
bool g_DoingFadeIn = 0;

// GLOBAL: REDLINE 0x005CD128
int g_LegalFadeInOpacity = 0;

// GLOBAL: REDLINE 0x005CD135
bool g_FadeInComplete = 0;

// GLOBAL: REDLINE 0x005CD12C
Widget* g_IntroWidgets[2];

// GLOBAL: REDLINE 0x0059B7F8
int g_IntroWidgetState = -1;

// FUNCTION: REDLINE 0x005402C6
bool IntroWidgetStartup() {
    if (g_IntroWidgetState >= 0) {
        Widget* w = g_IntroWidgets[g_IntroWidgetState];
        if (w) {
            w->unk_92 = 0;
        }
    }
    if (++g_IntroWidgetState < 1) {
        Widget* w = g_IntroWidgets[g_IntroWidgetState];
        if (w)
            w->unk_92 = 1;
        g_IntroUnk2 = 0;
        g_DoingFadeIn = true;
        g_LegalFadeInOpacity = 255;
        return 1;
    }
    return 0;
}

// FUNCTION: REDLINE 0x0053FFF0
bool StateImpl::Intro::Init(int prev_state) {
    if (g_ConnectDirect || g_ConsoleEnabled) {
        g_IntroShow = false;
    }

    if (g_IntroShow == 1) {
        g_EndIntroSplash = 1;
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
        IntroWidgetStartup();
    }
    return true;
}

// FUNCTION: REDLINE 0x0054022E
bool StateImpl::Intro::Shutdown(int state) {
    if (g_IntroShow) {
        for (int i = 0; i < 1; ++i) {
            g_Interface->RemoveControl(g_IntroWidgets[i]);
        }
    }
    return true;
}

// FUNCTION: REDLINE 0x00540108
bool StateImpl::Intro::Tick() {
    if (!g_EndIntroSplash)
        return true;
    // TODO: Input handling?
    if (g_FadeInComplete)
        return 1;
    if (g_DoingFadeIn)
        return true;
    if (++g_IntroUnk2 >= 1 && g_IntroWidgetState >= 0) {
        g_FadeInComplete = 1;
        g_LegalFadeInOpacity = 0;
    }
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

// FUNCTION: REDLINE 0x00540273
bool IntroSplashTick() {
    bool not_done = true;
    g_LegalFadeInOpacity -= 6;
    if (g_LegalFadeInOpacity <= 0) {
        g_LegalFadeInOpacity = 0;
        g_DoingFadeIn = false;
        not_done = false;
    }
    DrawScreenOverlay(0, 0, 0, g_LegalFadeInOpacity, 1);
    return not_done;
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
        // TODO: sub_43BDB5(0);
        g_Interface->Render(1);
        // TODO: sub_43CE39 THIS NEXT
        if (g_FadeInComplete)
            g_EndIntroSplash = 0;
        if (g_DoingFadeIn)
            IntroSplashTick();
    }
    if (EndScene()) {
        FlipDisplay();
    }
    if (!g_EndIntroSplash)
        SkipIntro();
    UnlockRender();

    return true;
}
