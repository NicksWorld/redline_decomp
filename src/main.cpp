#include <windows.h>
#include <stdio.h>

#include "log.h"

// GLOBAL: REDLINE 0x005c3f70
Log g_Log;

// GLOBAL: REDLINE 0x005cebd0
HINSTANCE g_HInstance;
// GLOBAL: REDLINE 0x005ceba4
int g_CmdShow;

// STUB: REDLINE 0x00551d73
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    g_Log.Open("TestLog.txt", true);
    return 0;
}
