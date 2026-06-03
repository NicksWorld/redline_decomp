#include "debug.h"

#include "../../res/resource.h"

#include "../registry.h"
#include "../config.h"
#include "../enginestate.h"
#include "../globals.h"
#include "../render.h"
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

// GLOBAL: REDLINE 0x0059C824
int g_SelectedDevice;

// FUNCTION: REDLINE 0x0055480C
void settings_resolution_init(HWND dlg) {
    HWND list = GetDlgItem(dlg, LST_SETTINGS_DISPLAY_DEVICE);
    short dev_sel = (short)SendMessageA(list, LB_GETCURSEL, 0, 0);

    if (dev_sel == g_SelectedDevice) return;
    g_SelectedDevice = dev_sel;
    list = GetDlgItem(dlg, LST_SETTINGS_SCREEN_RESOLUTION);
    int res_sel = SendMessageA(list, LB_GETCURSEL, 0, 0);

    int width;
    int height;
    int bpp;
    char formatted_buf[64];
    if (res_sel != -1) {
        SendMessageA(list, LB_GETTEXT, res_sel, (LPARAM)formatted_buf);
        width = atoi(formatted_buf);
        height = atoi(&formatted_buf[5]);
        bpp = atoi(&formatted_buf[10]);
    } else {
        width = g_ScreenWidth;
        height = g_ScreenHeight;
        bpp = g_ScreenBPP;
    }
    SendMessageA(list, LB_RESETCONTENT, 0, 0);
    for (short i = DeviceModeCount(dev_sel) - 1; i >= 0; --i) {
        short mode_width;
        short mode_height;
        short mode_bpp;
        if (DeviceModeResolution(dev_sel, i, &mode_width, &mode_height, &mode_bpp) == 1) {
            if (SupportsBitDepth(dev_sel, mode_bpp)) {
                FormatResolution(dev_sel, i, formatted_buf);
                SendMessageA(list, LB_ADDSTRING, 0, (LPARAM)formatted_buf);
            }
        }
    }
    int new_sel = ResolutionToMode(dev_sel, width, height, bpp);
    if (new_sel < 0 && bpp != 16)
        new_sel = ResolutionToMode(dev_sel, width, height, 16);
    if (new_sel < 0)
        new_sel = ResolutionToMode(dev_sel, 640, 480, 16);
    if (new_sel >= 0) {
        FormatResolution(dev_sel, new_sel, formatted_buf);
        SendMessageA(list, LB_SELECTSTRING, -1, (LPARAM)formatted_buf);
    } else {
        SendMessageA(list, LB_SETCURSEL, 0, 0);
    }
}

// FUNCTION: REDLINE 0x00554529
int settings_dialog_init(HWND dlg, int param1, int param2) {
    HWND dev_list = GetDlgItem(dlg, LST_SETTINGS_DISPLAY_DEVICE);
    for (short i = 0; i < DeviceCount(); ++i) {
        char* name = DeviceDisplayName(i);
        SendMessageA(dev_list, LB_ADDSTRING, 0, (LPARAM)name);
    }
    short sel_idx;
    if (*g_DisplayDevice) {
        sel_idx = DeviceByName(g_DisplayDevice);
        SendMessageA(dev_list, LB_SELECTSTRING, -1, (LPARAM)DeviceDisplayName(sel_idx));
    } else {
        short best = BestDevice();
        strcpy(g_DisplayDevice, DeviceName(best));
        SendMessageA(dev_list, LB_SELECTSTRING, -1, (LPARAM)DeviceDisplayName(best));
    }
    g_SelectedDevice = -1;
    settings_resolution_init(dlg);
    return 1;
}


// FUNCTION: REDLINE 0x0055462C
void settings_dialog_cmd(HWND dlg, int param, int param2, int param3) {
    int res = 0;
    short done = 0;
    switch (param) {
        case BTN_SETTINGS_ADVANCED:
            done = 1;
            res = 2;
            PlaySoundA("WAV_SWITCH4", g_hInstance, SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
            break;
        case BTN_SETTINGS_DONE:
            done = 1;
            res = 1;
            PlaySoundA("WAV_SWITCH4", g_hInstance, SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
            break;
        case IDCANCEL:
            PlaySoundA("WAV_SWITCH4", g_hInstance, SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
            EndDialog(dlg, 0);
            break;
        case LST_SETTINGS_DISPLAY_DEVICE:
            settings_resolution_init(dlg);
            break;
    }

    if (done) {
        HWND list = GetDlgItem(dlg, LST_SETTINGS_DISPLAY_DEVICE);
        int sel = SendMessageA(list, LB_GETCURSEL, 0, 0);
        if (sel == -1) {
            *g_DisplayDevice = 0;
        } else {
            char buf[96];
            SendMessageA(list, LB_GETTEXT, sel, (LPARAM)buf);
            sel = DeviceByDisplayName(buf);
            strcpy(g_DisplayDevice, DeviceName(sel));
            
            list = GetDlgItem(dlg, LST_SETTINGS_SCREEN_RESOLUTION);
            sel = SendMessageA(list, LB_GETCURSEL, 0, 0);
            if (sel != -1) {
                SendMessageA(list, LB_GETTEXT, sel, (LPARAM)buf);
                g_ScreenWidth = atoi(buf);
                g_ScreenHeight = atoi(&buf[5]);
                g_ScreenWidth = atoi(&buf[10]);
            }
        }
        EndDialog(dlg, res);
    }
}

// FUNCTION: REDLINE 0x005544AC
int CALLBACK settings_cb(HWND dlg, unsigned int msg, WPARAM wParam, LPARAM lParam) {
    int unk = 1;
    switch(msg) {
        case WM_COMMAND:
            settings_dialog_cmd(dlg, LOWORD(wParam), lParam, HIWORD(wParam));
            return 0;
        case WM_INITDIALOG:
            return settings_dialog_init(dlg, wParam, lParam);
        default:
            unk = 0;
    }
    return unk;
}

// FUNCTION: REDLINE 0x005543FA
int open_settings(int nCmdShow) {
    bool v1 = false;
    int v2 = 0;

    while (!v1) {
        v2 = DialogBoxParamA(g_hInstance, (LPCSTR)0x78, g_Window, settings_cb, 0);
        switch(v2) {
            case 0:
                return v2;
            case 1:
                v1 = true;
                break;
            case 2:
                v1 = true;
                break;
        }
    }
    if (!g_Config) return v2;
    g_Config->PopulateDefaults();
    g_Config->SetStringValue("DisplayDevice", g_DisplayDevice);
    g_Config->SetStringValue("DeviceDriver", g_DeviceDriver);
    return v2;
}

int InitWrapper(int flags) {
    return g_Direct3d->Initialize(flags);
}

// STUB: REDLINE 0x0048E731
int ConstructGraphicsGlobals() {
    if (!g_Direct3d) {
        g_Direct3d = new D3dRenderer();
    }
    // TODO
    return 1;
}

// FUNCTION: REDLINE 0x005535AB
char InitializeGraphics(char a) {
    // TODO: Some flag checks?
    if (!ConstructGraphicsGlobals())
        return 0;
    int res = InitWrapper(a | 2);
    if (res == -1) {
        MessageBoxA(
                NULL,
                "Redline requires Microsoft DirectX 6 or above.\n"
                "\n"
                "DirectX 6 can be downloaded from Microsoft at:\n"
                "    http://www.microsoft.com/directx/\n"
                "\n"
                "\n"
                "Consult the readme file for more information.",
                NULL,
                MB_ICONWARNING);
        return 0;
    }
    // TODO
    return 0;
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

        InitializeGraphics(0);

        while(open_display_settings) {
            // TODO: Display settings?
            open_settings(g_nCmdShow);
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
