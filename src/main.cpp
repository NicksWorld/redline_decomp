#include <windows.h>
#include <stdio.h>

#include "resource.h"
#include "log.h"

// GLOBAL: REDLINE 0x005c3f70
Log g_Log;

// GLOBAL: REDLINE 0x005cebd0
HINSTANCE g_hInstance;
// GLOBAL: REDLINE 0x005ceba4
int g_CmdShow;

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

// STUB: REDLINE 0x00551d73
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    HWND existing_window = FindWindowA("Redline", NULL);
    if (!IsWindow(existing_window)) {

    }
    
    return 0;
}
