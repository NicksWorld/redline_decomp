#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <sys/stat.h>
#include <winuser.h>

#include "log.h"
#include "resource.h"
#include "3dnow.h"
#include "scripts.h"
#include "pack.h"
#include "globals.h"
#include "enginestate.h"

// GLOBAL: REDLINE 0x005c3f70
Log g_Log;
// GLOBAL: REDLINE 0x005ceb14
EngineState* g_EngineState;
// GLOBAL: REDLINE 0x005a8c94
StateTree* g_StateTree;

// GLOBAL: REDLINE 0x005cebd0
HINSTANCE g_hInstance;
// GLOBAL: REDLINE 0x005ceba4
int g_nCmdShow;

// GLOBAL: REDLINE 0x005ceb1c
char g_registryKey[128];

// GLOBAL: REDLINE 0x005ccf50
void* g_GameData;

// GLOBAL: REDLINE 0x005cebec
time_t g_time;

// GLOBAL: REDLINE 0x005ce601
bool g_has3DNow;

// GLOBAL: REDLINE 0x005cebb4
int g_unk;

// GLOBAL: REDLINE 0x005a8f64
short g_unknown;

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

// STUB: REDLINE 0x0053fb64
void InitGlobals() {}

// Semantics uncertain
// FUNCTION: REDLINE 0x0048F261
void SetUnknown(short v) {
    g_unknown = v;
}

// FUNCTION: REDLINE 0x0048F256
short GetUnknown() {
    return g_unknown;
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
    srand(g_time);

    InitGlobals();

    if (_AMD3D_DetectHardware() == 0) {
        g_Log.Debug("AMD 3DNow! detected");
        g_has3DNow = true;
    }
    g_unk = 1;

    char exe[32];
    LPSTR cmdline = GetCommandLineA();
    char* quot = strchr(cmdline, '"');
    if (quot != NULL) {
        strcpy(exe, quot + 1);
        quot = strchr(exe, '"');
        if (quot != NULL) {
            *quot = 0; 
        }
    } else {
        strcpy(exe, "redline.exe");
    }
    
    struct _stat buf;
    int exists = _stat(exe, &buf);
    if (exists) {
        // Exists
        HANDLE file = CreateFileA(exe, GENERIC_READ, 1, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != (HANDLE)-1) {
            FILETIME last_write;
            FILETIME last_write_local;
            SYSTEMTIME sys;
            GetFileTime(file, NULL, NULL, &last_write);
            FileTimeToLocalFileTime(&last_write, &last_write_local);
            FileTimeToSystemTime(&last_write_local, &sys);
            CloseHandle(file);

            char msg[128];
            sprintf(msg, "Execute: %s  (Date: %d/%02d/%02d %d:%02d:%02d  Size: %d)", exe, sys.wMonth, sys.wDay, sys.wYear, sys.wHour, sys.wMinute, sys.wSecond, buf.st_size);
            g_Log.Debug(msg);
        }
    }

    if (g_unk != 0) {
        if(!LoadPack("Redline.bgd", 0, 0)) {
            MessageBoxA(NULL, "Fatal Error Loading:  Redline.bgd\n\n\nConsult the readme file for more information.", NULL, MB_ICONEXCLAMATION);
            return 0;
        }
        char pathbuf[128];
        strcpy(pathbuf, "*.bgd");
        WIN32_FIND_DATA findFileData;
        HANDLE file = FindFirstFileA(pathbuf, &findFileData);
        if (file != (HANDLE)-1) {
            strcpy(pathbuf, findFileData.cFileName);
            if (strcmpi(pathbuf, "redline.bgd") != 0) {
                LoadPack(pathbuf, 0, 0);
            }
            while(FindNextFileA(file, &findFileData)) {
                strcpy(pathbuf, findFileData.cFileName);
                if (strcmpi(pathbuf, "redline.bgd") != 0) {
                    LoadPack(pathbuf, 0, 0);
                }
            }
        }
    }

    if (LoadScripts("PC_Script.thg") || LoadScripts("..\\GameData\\PC_Script.thg")) {
        g_GameData = g_Scripts.Lookup(20, "GameData", NULL);
        if (!g_GameData) {
            MessageBoxA(NULL, "Fatal Error Loading: Misc data script 'GameData'", NULL, MB_ICONEXCLAMATION);
            return 0;
        }

        g_EngineState = new EngineState();
        g_StateTree = new StateTree();
        g_StateTree->PopulateNodes();
        StateNode* next = g_StateTree->Next(1);
        if (!next) return 0;
        SetUnknown(1);
        g_EngineState->ChangeState(next->state_id);
    }

    return 0;
}

