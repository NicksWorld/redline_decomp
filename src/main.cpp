#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <windows.h>
#include <winuser.h>

#include "3dnow.h"
#include "enginestate.h"
#include "globals.h"
#include "log.h"
#include "pack.h"
#include "resource.h"
#include "scripts.h"

// GLOBAL: REDLINE 0x005c3f70
Log g_Log;
// GLOBAL: REDLINE 0x005ceb14
EngineState *g_EngineState;
// GLOBAL: REDLINE 0x005a8c94
StateTree *g_StateTree;

// GLOBAL: REDLINE 0x005cebd0
HINSTANCE g_hInstance;
// GLOBAL: REDLINE 0x005ceba4
int g_nCmdShow;
// GLOBAL: REDLINE 0x005cebd4
HWND g_Window = NULL;

// GLOBAL: REDLINE 0x005ceb1c
char g_registryKey[128];

// GLOBAL: REDLINE 0x005ccf50
void *g_GameData;

// GLOBAL: REDLINE 0x005cebec
time_t g_time;

// GLOBAL: REDLINE 0x005ce601
bool g_has3DNow;

// GLOBAL: REDLINE 0x005cebb4
int g_unk;

// GLOBAL: REDLINE 0x005a8f64
short g_unknown;

// GLOBAL: REDLINE 0x005ce8f8
bool g_ConsoleEnabled = false;
// GLOBAL: REDLINE 0x005CE8FE
bool g_LobbyEnabled = false;

// GLOBAL: REDLINE 0x005cebac
HANDLE g_HeartbeatThread;
// GLOBAL: REDLINE 0x005cebb0
HANDLE g_MainThread;

// GLOBAL: REDLINE 0x005ce600
bool g_unkBool;

// GLOBAL: REDLINE 0x005a7fd4
class D3dRenderer* g_Direct3d;

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
void InitGlobals() {
    // strcpy(&byte_5CE6EC, "man");
    // strcat(&Buffer, "none_selected.wld");
    g_unkBool = 0;
    g_has3DNow = 0;
    // byte_5CE602 = 0;
    // byte_5CE8FE = 0;
    g_DebugMouse = 0;
    g_DebugFrames = 0;
    g_PlayDemo = 0;
    g_RecordDemo = 0;
    g_replayPlay = 0;
    g_replayRecord = 0;
    g_Windowed = 0;
    g_AIActive = 1;
    g_D3DSound = 0;
    g_QuickRun = 0;
    g_NumFrames = 60;
    // dword_5CE670 = 0;
    g_LastMap[0] = 0;
    // byte_5CE6AC = 0;
    // byte_5CE6EC = 0;
    // Buffer = 0;
    g_FreeLook = 1;
    g_DrawShadows = 1;
    g_MipMapping = 1;
    g_Mouse_Foot = 4.0;
    g_Mouse_Car = 2.0;
    g_DisplayDevice[0] = 0;
    g_DeviceDriver[0] = 0;
    g_ScreenWidth = 640;
    g_ScreenHeight = 480;
    g_ScreenBPP = 16;
    g_ScreenGamma = 1.0;
    g_CreateUseFile = 1;
    // byte_5CE8FF = 0;
    g_DisplayParticles = 1;
    g_DisplayTireTreads = 1;
    g_DisplayScreenFlash = 1;
    g_DifficultyLevel = 1;
    g_EnableFog = 1;
    g_EnviroMapping = 1;
    g_GroundLighting = 1;
    g_PalettedTextures = 1;
    g_TextureDetail = 1;
    g_LimitParticleSize = 0;
    g_TripleBuffer = 1;
    g_DXtextureManager = 0;
    g_DitherEnable = 1;
    g_WeaponDisplayMin = 0;
    g_PersonGunDraw = 1;
    g_carDashboardDraw = 1;
    g_turretDashboardDraw = 1;
    g_carCamMode = 1;
    g_turretCamMode = 0;
    g_carFollowCamMode = 0;
    g_turretFollowCamMode = 0;
    g_ReverseYAxis = 0;
    g_ReverseYAxisCar = 0;
    g_cruiseControl = 1;
    g_soundChannels = 32;
    g_Joystick_UpDown = 4.0;
    g_Joystick_LeftRight = 4.0;
    g_CarJoystick_UpDown = 4.0;
    g_CarJoystick_LeftRight = 4.0;
    g_Joystick_FreeLook = 1;
    g_Joystick_DeadZoneX = 5;
    g_Joystick_DeadZoneY = 5;
    g_Master_Volume = 100.0;
    g_CDAudio_Active = 1;
    g_IntroVideo = 1;
    // g_OptConsoleEnabled = 0;
    g_Net_ConsoleTCP = 1;
    g_Net_Perf_CliSendFrames = 2;
    g_Net_Perf_ServSendFrames = 2;
    g_Net_Perf_CliInterp = 1;
    g_Net_Perf_CliPredict = 1;
    // byte_5CE93A = 1;
    // byte_5CE93B = 0;
    g_Net_Perf_ExtraLatencyOn = 0;
    g_Net_Perf_ExtraLatency = 10;
    // byte_5CE93C = 1;
    // byte_5CE8FD = 0;
    // byte_5CE8FC = 0;
    g_Net_Mode_CTF_FlagDrop = 0;
    g_Net_Mode_ScoreLimitOn = 0;
    g_Net_Mode_TimeLimitOn = 0;
    g_Net_Mode_ScoreLimit = 1;
    // dword_5CE924 = 4;
    g_Net_Mode_TimeLimit = 1;
    g_Net_MaxPlayers = 8;
    g_Net_FastStart = 0;
    g_Net_FastHost = 0;
    // byte_5CE90D = 0;
    g_Net_SmartCrosshair = 0;
    g_Net_FriendlyFire = 0;
    g_Net_PlayerTeam = 0;
    g_Net_PlayerSkel = 0;
    g_Net_TeamPlace = 0;
    g_Net_Mode_CTF = 0;
    g_Net_Mode_CTF_Adv = 1;
    // byte_5CE90F = 0;
    // return sub_53FF47();
}

// Semantics uncertain
// FUNCTION: REDLINE 0x0048F261
void SetUnknown(short v) { g_unknown = v; }

// FUNCTION: REDLINE 0x0048F256
short GetUnknown() { return g_unknown; }

// STUB: REDLINE 0x005536F6
int event_loop() {
    MSG msg;
    while (true) {
        while (PeekMessageA(&msg, NULL, 0, 0, 1)) {
            if (msg.message == WM_QUIT)
                return 1;
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        // TODO

        int next = g_EngineState->GetQueuedState(false);
        if (next != -1) {
            if (g_EngineState->ChangeState(next) != next)
                return 0;
        } else {
            if (!g_EventTick()) {
                g_Log.Debug("Exiting Idle Process");
                return 0;
            }
        }
    }

    return 0;
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
    char *quot = strchr(cmdline, '"');
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
        HANDLE file = CreateFileA(exe, GENERIC_READ, 1, NULL, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != (HANDLE)-1) {
            FILETIME last_write;
            FILETIME last_write_local;
            SYSTEMTIME sys;
            GetFileTime(file, NULL, NULL, &last_write);
            FileTimeToLocalFileTime(&last_write, &last_write_local);
            FileTimeToSystemTime(&last_write_local, &sys);
            CloseHandle(file);

            char msg[128];
            sprintf(msg,
                    "Execute: %s  (Date: %d/%02d/%02d %d:%02d:%02d  Size: %d)",
                    exe, sys.wMonth, sys.wDay, sys.wYear, sys.wHour,
                    sys.wMinute, sys.wSecond, buf.st_size);
            g_Log.Debug(msg);
        }
    }

    if (g_unk != 0) {
        if (!LoadPack("Redline.bgd", 0, 0)) {
            MessageBoxA(NULL,
                        "Fatal Error Loading:  Redline.bgd\n\n\nConsult the "
                        "readme file for more information.",
                        NULL, MB_ICONEXCLAMATION);
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
            while (FindNextFileA(file, &findFileData)) {
                strcpy(pathbuf, findFileData.cFileName);
                if (strcmpi(pathbuf, "redline.bgd") != 0) {
                    LoadPack(pathbuf, 0, 0);
                }
            }
        }
    }

    if (LoadScripts("PC_Script.thg") ||
        LoadScripts("..\\GameData\\PC_Script.thg")) {
        g_GameData = g_Scripts.Lookup(20, "GameData", NULL);
        if (!g_GameData) {
            MessageBoxA(NULL,
                        "Fatal Error Loading: Misc data script 'GameData'",
                        NULL, MB_ICONEXCLAMATION);
            return 0;
        }

        g_EngineState = new EngineState();
        g_StateTree = new StateTree();
        g_StateTree->PopulateNodes();
        StateNode *next = g_StateTree->Next(1);
        if (!next)
            return 0;
        SetUnknown(1);
        int state = g_EngineState->ChangeState(next->state_id);
        if (state != next->state_id)
            return 0;
        // TODO: int 0x005CEBDC = 1
        if (event_loop()) {
            return 1;
        } else {
            // Cleanup stuff
        }
    }

    return 0;
}
