#include "av.h"

#include <windows.h>
#include <winuser.h>

#include "../enginestate.h"
#include "../globals.h"

#include <stdio.h>
#include "../log.h"

// FUNCTION: REDLINE 0x00552511
int OpenWindow() {
    g_Window = CreateWindowExA(
        WS_EX_APPWINDOW,
        "Redline",
        "Redline",
        WS_POPUP | WS_VISIBLE,
        0, 0,
        10, 10,
        NULL,
        NULL,
        g_hInstance,
        NULL
    );
    if (!g_Window) {
        return false;
    }
    ShowWindow(g_Window, g_nCmdShow);
    UpdateWindow(g_Window);
    if (!g_Windowed)
        ShowCursor(0);
    if (g_IntroVideo) {
        if (!g_ConsoleEnabled) {
            // TODO: Play intro
        }
    }
    g_WindowCreated = 1;
    return true;
}
// FUNCTION: REDLINE 0x0053E307
bool StateImpl::AV::Init(int next_state) {
    if (g_ConsoleEnabled) {
        // TODO: Open server console dialog, and start server
    } else if (!OpenWindow()) {
        return 0;
    }
    return true;
}
