#include "debug.h"

#include "../../res/resource.h"

#include "../registry.h"
#include "../config.h"
#include "../enginestate.h"
#include "../globals.h"
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>

// FUNCTION: REDLINE 0x0053AB68
bool StateImpl::Debug::Init(int prev_state) { return true; }

// FUNCTION: REDLINE 0x0053AB6F
bool StateImpl::Debug::Shutdown(int next_state) {
    // TODO: Set unknown global
    return true;
}

// FUNCTION: REDLINE 0x0053A970
bool StateImpl::Debug::Tick() { return true; }

// GLOBAL: REDLINE 0x005CD064
bool g_DebugStartupComplete = false;

// GLOBAL: REDLINE 0x005ce70c
char g_NextMap[128]; // Unsure if correct meaning

// GLOBAL: REDLINE 0x005ce4b0
HBITMAP g_LauncherBitmaps[14];

// FUNCTION: REDLINE 0x00553EFA
int launcher_loadbitmaps(HWND dlg, WPARAM wParam, LPARAM lParam) {
    g_LauncherBitmaps[0] = LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_PLAY);
    g_LauncherBitmaps[1] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_PLAY_PRESSED);
    g_LauncherBitmaps[2] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_DISPLAY);
    g_LauncherBitmaps[3] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_DISPLAY_PRESSED);
    g_LauncherBitmaps[4] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_TROUBLESHOOT);
    g_LauncherBitmaps[5] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_TROUBLESHOOT_PRESSED);
    g_LauncherBitmaps[6] = LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_QUIT);
    g_LauncherBitmaps[7] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_QUIT_PRESSED);

    g_LauncherBitmaps[8] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_MPLAYER);
    g_LauncherBitmaps[9] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_MPLAYER_PRESSED);
    g_LauncherBitmaps[10] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_GAMESPY);
    g_LauncherBitmaps[11] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_GAMESPY_PRESSED);
    g_LauncherBitmaps[12] = LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_HEAT);
    g_LauncherBitmaps[13] =
        LoadBitmapA(g_hInstance, (LPCSTR)BMP_LAUNCHER_HEAT_PRESSED);
    return 1;
}

// GLOBAL: REDLINE 0x005cebf8
int g_ReadmeNotFound_Active = false;

// FUNCTION: REDLINE 0x0055403B
void launcher_cmd(HWND dlg, int btn, int param1, int param2) {
    strcpy(&g_LastMap[120], "man"); // I have no clue what this is meant to do
    INT_PTR res = 0;
    bool end = false;
    char buf[256];
    char gamespy_dir[128];
    bool readme_not_found;
    HINSTANCE handle;
    switch (btn) {
    case IDCANCEL:
    case BTN_LAUNCHER_QUIT:
        PlaySoundA("Wav_Switch4", g_hInstance,
                   SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
        EndDialog(dlg, 3);
        break;
    case BTN_LAUNCHER_MPLAYER:
        PlaySoundA("Wav_Switch4", g_hInstance,
                   SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
        if ((unsigned int)ShellExecuteA(dlg, "open", "mplaynow.exe", NULL, NULL,
                                        1) > 32) {
            EndDialog(dlg, 3);
            return;
        }
        MessageBoxA(NULL,
                    "Can't find mplaynow.exe\n Please make sure it's installed "
                    "in the current path.",
                    "MPlayer File Not Found", MB_ICONWARNING);
        break;
    case BTN_LAUNCHER_GAMESPY:
        PlaySoundA("Wav_Switch4", g_hInstance,
                   SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
        ReadRegistry("installdir", "", gamespy_dir, "Software\\GameSpy\\GameSpy 3D");
        if (*gamespy_dir &&
            (unsigned int)ShellExecuteA(dlg, "open", "gamespy.exe", NULL,
                                        gamespy_dir, 1) > 32) {
            EndDialog(dlg, 3);
            return;
        }
        if ((unsigned int)ShellExecuteA(dlg, "open", "http://gamespy.com", NULL,
                                        NULL, 1) <= 32) {
            MessageBoxA(NULL,
                        "Can't find gamespy.exe\n Please make sure it's "
                        "installed and is version 2.08 or newer.\n"
                        "Visit http://www.gamespy.com for the latest version",
                        "Gamespy Not Found", MB_ICONWARNING);
        }
        break;
    case BTN_LAUNCHER_HEAT:
        PlaySoundA("Wav_Switch4", g_hInstance,
                   SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
        if ((unsigned int)ShellExecuteA(dlg, "open", "GoToHeat", "5157", NULL,
                                        1) > 32) {
            EndDialog(dlg, 3);
            return;
        }
        if ((unsigned int)ShellExecuteA(dlg, "open", "http://www.heat.net",
                                        NULL, NULL, 1) <= 32) {
            MessageBoxA(
                NULL,
                "Can't find GotoHeat.exe\n Please make sure it's "
                "installed.\nVisit http://www.heat.net for the latest version",
                "Heat Not Found", MB_ICONWARNING);
        }
        break;
    case BTN_LAUNCHER_DISPLAY:
        PlaySoundA("Wav_Switch4", g_hInstance,
                   SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
        end = true;
        res = 6;
        break;
    case BTN_LAUNCHER_TROUBLESHOOT:
        PlaySoundA("Wav_Switch4", g_hInstance,
                   SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
        readme_not_found = true;
        GetCurrentDirectoryA(256, buf);
        strcat(buf, "\\readme\\index.htm");
        handle = ShellExecuteA(dlg, "open", buf, NULL, NULL, 1);
        if ((unsigned int)handle > 32) {
            readme_not_found = 0;
        }
        if (readme_not_found &&
            (unsigned int)ShellExecuteA(dlg, "open", "readme.rtf", NULL, NULL,
                                        1) <= 32 &&
            !g_ReadmeNotFound_Active) {
            g_ReadmeNotFound_Active = true;
            MessageBoxA(NULL, "Can't find readme.rtf", "File Not Found",
                        MB_ICONWARNING);
            g_ReadmeNotFound_Active = false;
        }
        break;
    case BTN_LAUNCHER_PLAY:
        PlaySoundA("Wav_Switch4", g_hInstance,
                   SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
        end = true;
        res = 0;
        break;
    }
    if (end)
        EndDialog(dlg, res);
}

// FUNCTION: REDLINE 0x00553D0E
int CALLBACK launcher_cb(HWND dlg, unsigned int msg, WPARAM wParam, LPARAM lParam) {
    int res = 1;

    int i;
    DRAWITEMSTRUCT *item;
    HDC hdc;
    int val;

    switch (msg) {
    case WM_COMMAND:
        launcher_cmd(dlg, LOWORD(wParam), lParam, HIWORD(wParam));
        return 0;
    case WM_INITDIALOG:
        return launcher_loadbitmaps(dlg, wParam, lParam);
    case WM_DRAWITEM:
        item = (DRAWITEMSTRUCT *)lParam;
        hdc = CreateCompatibleDC(item->hDC);
        switch (item->CtlID) {
        case BTN_LAUNCHER_PLAY:
            val = 0;
            break;
        case BTN_LAUNCHER_DISPLAY:
            val = 2;
            break;
        case BTN_LAUNCHER_TROUBLESHOOT:
            val = 4;
            break;
        case BTN_LAUNCHER_QUIT:
            val = 6;
            break;
        case BTN_LAUNCHER_MPLAYER:
            val = 8;
            break;
        case BTN_LAUNCHER_GAMESPY:
            val = 10;
            break;
        case BTN_LAUNCHER_HEAT:
            val = 12;
            break;
        }
        if (item->itemState & ODS_SELECTED) {
            i = 1;
        } else {
            i = 0;
        }
        SelectObject(hdc, g_LauncherBitmaps[val + i]);
        BitBlt(item->hDC, item->rcItem.left, item->rcItem.top,
               item->rcItem.right - item->rcItem.left,
               item->rcItem.bottom - item->rcItem.top, hdc, 0, 0, SRCCOPY);
        DeleteDC(hdc);
        break;
    case WM_DESTROY:
        for (i = 0; i < 14; ++i) {
            DeleteObject(&g_LauncherBitmaps[i]);
        }
        break;
    default:
        res = 0;
        break;
    }
    return res;
}

// FUNCTION: REDLINE 0x00553C00
int open_launcher(int nCmdShow) {
    int v2 = 0;
    int unused = 0;
    int res = 4;
    LPCSTR diag = (LPCSTR)DIAG_LAUNCHER;

    while (!v2) {
        switch(DialogBoxParamA(g_hInstance, diag, g_Window, launcher_cb, 0)) {
            case 3:
                return 3;
            case 6:
                v2 = 1;
                res = 6;
                break;
            case 2:
                g_unkBool = true;
                v2 = 1;
                break;
            case 1:
                res = 1;
            case 0:
                v2 = 1;
                break;
        }
    }
    if (!g_Config) return res;
    g_Config->PopulateDefaults();
    g_Config->SetStringValue("LastMap", g_LastMap);
    g_Config->SetStringValue("DisplayDevice", g_DisplayDevice);
    g_Config->SetStringValue("DeviceDriver", g_DeviceDriver);
    return res;
}

// STUB: REDLINE 0x0053A977
bool StateImpl::Debug::EventTick() {
    int v5 = 1;
    if (g_DebugStartupComplete)
        return true;
    bool res = g_Config->ProcessCmdline();
    g_QuickRun = 0;
    if (res) {
        g_Config->GetStringValue("Lastmap", g_NextMap);
        strcat(g_NextMap, ".wld");
    }
    if (g_ConsoleEnabled) {
        // TODO: Console dialog
        res = true;
    }

    while (!res) {
        bool open_display_settings = false;
        switch (open_launcher(g_nCmdShow)) {
            case 3:
                return 0;
            case 5:
                return 0;
            case 2:
                g_unkBool = true;
                break;
            case 6:
                open_display_settings = true;
                break;
        }
        if (!open_display_settings)
            break;

        while(open_display_settings) {
            // TODO: Display settings?
        }
    }

    if (g_unkBool) v5 = 0;
    StateNode* next = g_StateTree->Next(v5);
    if (!next)
        return 0;
    if (g_EngineState->QueueState(next->state_id) != next->state_id)
        return 0;
    g_DebugStartupComplete = true;
    return true;
}
