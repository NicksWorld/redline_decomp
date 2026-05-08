#include <stdio.h>
#include <windows.h>
#include <time.h>

#include "log.h"
#include "resource.h"
#include "3dnow.h"

// GLOBAL: REDLINE 0x005c3f70
Log g_Log;

// GLOBAL: REDLINE 0x005cebd0
HINSTANCE g_hInstance;
// GLOBAL: REDLINE 0x005ceba4
int g_nCmdShow;

// GLOBAL: REDLINE 0x005ceb1c
char g_registryKey[128];

// GLOBAL: REDLINE 0x005cebec
time_t g_time;

// GLOBAL: REDLINE 0x005ce601
bool g_has3DNow;

// FUNCTION: REDLINE 0x00551cd9
BOOL RegisterWindowClass() {
    WNDCLASSEXA cl;
    cl.cbSize = 0x30;
    cl.style = 0x20;
    cl.lpfnWndProc = NULL; // TODO
    cl.cbClsExtra = 0;
    cl.cbWndExtra = 0;
    cl.hInstance = g_hInstance;
    cl.hIcon = LoadIconA(g_hInstance, MAKEINTRESOURCE(APP_ICON));
    cl.hCursor = LoadCursorA(NULL, IDC_ARROW);
    cl.hbrBackground = NULL;
    cl.lpszMenuName = NULL;
    cl.lpszClassName = "Redline";
    cl.hIconSm = LoadIconA(g_hInstance, MAKEINTRESOURCE(APP_ICON));
    if (RegisterClassExA(&cl) == NULL) {
        return false;
    }
    return true;
}

// FUNCTION: REDLINE 0x00551d73
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    HWND existing_window = FindWindowA("Redline", NULL);
    if (IsWindow(existing_window)) {
        HWND popup = GetLastActivePopup(existing_window);
        if (IsWindow(popup)) {
            existing_window = popup;
            SetForegroundWindow(popup);
            if (IsIconic(existing_window)) {
                ShowWindow(popup, 9);
            }
        }
        return 0;
    }
    g_hInstance = hInstance;
    g_nCmdShow = nCmdShow;

    CoInitialize(0);
    if (RegisterWindowClass() == false) {
        g_Log.Debug("Error: Failed to initialize window class");
        return 0;
    }
    g_Log.Open("Redline - Log.txt", 1);
    strcpy(g_registryKey, "SOFTWARE\\Beyond Games\\Redline");
    g_Log.Debug("Daedalus Engine Initializing");

    g_time = time(0);
    // TODO: Gets stored in thread local storage arrays?
    if (_AMD3D_DetectHardware() == 0) {
        g_Log.Debug("AMD 3DNow! detected");
        g_has3DNow = true;
    }

    return 0;
}

