#include "debug.h"

#include "../../res/resource.h"

#include "../registry.h"
#include "../config.h"
#include "../enginestate.h"
#include "../globals.h"
#include "../render.h"
#include "../pack.h"
#include <mmsystem.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <stdio.h>

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
    int res = 0;
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
    int res = 0;

    while (!v1) {
        res = DialogBoxParamA(g_hInstance, (LPCSTR)DIAG_SETTINGS, g_Window, settings_cb, 0);
        switch(res) {
            case 0:
                return res;
            case 1:
                v1 = true;
                break;
            case 2:
                v1 = true;
                break;
        }
    }
    if (!g_Config) return res;
    g_Config->PopulateDefaults();
    g_Config->SetStringValue("DisplayDevice", g_DisplayDevice);
    g_Config->SetStringValue("DeviceDriver", g_DeviceDriver);
    return res;
}

// FUNCTION: REDLINE 0x00554E40
void adv_settings_dialog_cmd(HWND dlg, int param, int param2, int param3) {
    bool apply = false;
    switch (param) {
        case BTN_ADVANCED_DONE:
            PlaySoundA("WAV_Switch4", g_hInstance, SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
            apply = true;
            break;
        case IDCANCEL:
            PlaySoundA("WAV_Switch4", g_hInstance, SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
            EndDialog(dlg, 0);
            break;
        case LST_SETTINGS_DISPLAY_DEVICE:
            PlaySoundA("WAV_Switch4", g_hInstance, SND_ASYNC | SND_NOSTOP | SND_RESOURCE);
            settings_resolution_init(dlg); // ??? Why.
            break;
    }

    if (apply) {
        g_DrawShadows = IsDlgButtonChecked(dlg, CHK_ADVANCED_SHADOWS) == 1;
        g_EnableFog = IsDlgButtonChecked(dlg, CHK_ADVANCED_FOG) == 1;
        g_EnviroMapping = IsDlgButtonChecked(dlg, CHK_ADVANCED_CHROME) == 1;
        g_GroundLighting = IsDlgButtonChecked(dlg, CHK_ADVANCED_GROUND_LIGHTING) == 1;
        g_PalettedTextures = IsDlgButtonChecked(dlg, CHK_ADVANCED_PALETTED_TEXTURES) == 1;
        g_LimitParticleSize = IsDlgButtonChecked(dlg, CHK_ADVANCED_UNLIMITED_PARTICLESIZE) == 1;
        g_TripleBuffer = IsDlgButtonChecked(dlg, CHK_ADVANCED_TRIPLE_BUFFER) == 1;
        g_DisplayScreenFlash = IsDlgButtonChecked(dlg, CHK_ADVANCED_SCREEN_FLASHES) == 1;
        g_DisplayTireTreads = IsDlgButtonChecked(dlg, CHK_ADVANCED_TIRE_TREADS) == 1;
        g_DXtextureManager = IsDlgButtonChecked(dlg, CHK_ADVANCED_DX_TEXTUREMANAGER) == 1;
        g_DitherEnable = IsDlgButtonChecked(dlg, CHK_ADVANCED_DITHERING) == 1;
        g_D3DSound = IsDlgButtonChecked(dlg, CHK_ADVANCED_3D_SOUND) == 1;

        int v = 0;
        if (IsDlgButtonChecked(dlg, RDO_ADVANCED_PARTICLES_NORM) == 1)
            v = 1;
        else if (IsDlgButtonChecked(dlg, RDO_ADVANCED_PARTICLES_LOTS) == 1)
            v = 2;
        g_DisplayParticles = v;

        v = 0;
        if (IsDlgButtonChecked(dlg, RDO_ADVANCED_TEXDETAIL_MED) == 1)
            v = 1;
        else if (IsDlgButtonChecked(dlg, RDO_ADVANCED_TEXDETAIL_HIGH) == 1)
            v = 2;
        g_TextureDetail = v;

        v = 0;
        if (IsDlgButtonChecked(dlg, RDO_ADVANCED_MIP_BILINEAR) == 1)
            v = 1;
        else if (IsDlgButtonChecked(dlg, RDO_ADVANCED_MIP_TRILINEAR) == 1)
            v = 2;
        g_MipMapping = v;

        v = 0;
        if (IsDlgButtonChecked(dlg, RDO_ADVANCED_SNDCHNLS_4) == 1)
            v = 4;
        else if (IsDlgButtonChecked(dlg, RDO_ADVANCED_SNDCHNLS_8) == 1)
            v = 8;
        else if (IsDlgButtonChecked(dlg, RDO_ADVANCED_SNDCHNLS_16) == 1)
            v = 16;
        else if (IsDlgButtonChecked(dlg, RDO_ADVANCED_SNDCHNLS_32) == 1)
            v = 32;
        g_soundChannels = v;
        EndDialog(dlg, 1);
    }
}

// FUNCTION: REDLINE 0x00554B8C
int adv_settings_dialog_init(HWND dlg, int param1, int param2) {
    int btn;

    btn = RDO_ADVANCED_PARTICLES_LOTS;
    switch (g_DisplayParticles) {
        case 0:
            btn = RDO_ADVANCED_PARTICLES_MIN;
            break;
        case 1:
            btn = RDO_ADVANCED_PARTICLES_NORM;
            break;
    }
    CheckRadioButton(dlg, RDO_ADVANCED_PARTICLES_MIN, RDO_ADVANCED_PARTICLES_LOTS, btn);

    btn = RDO_ADVANCED_TEXDETAIL_HIGH;
    switch (g_TextureDetail) {
        case 0:
            btn = RDO_ADVANCED_TEXDETAIL_LOW;
            break;
        case 1:
            btn = RDO_ADVANCED_TEXDETAIL_MED;
            break;
    }
    CheckRadioButton(dlg, RDO_ADVANCED_TEXDETAIL_LOW, RDO_ADVANCED_TEXDETAIL_HIGH, btn);

    btn = RDO_ADVANCED_MIP_TRILINEAR;
    switch (g_MipMapping) {
        case 0:
            btn = RDO_ADVANCED_MIP_NONE;
            break;
        case 1:
            btn = RDO_ADVANCED_MIP_BILINEAR;
            break;
    }
    CheckRadioButton(dlg, RDO_ADVANCED_MIP_NONE, RDO_ADVANCED_MIP_TRILINEAR, btn);

    btn = RDO_ADVANCED_SNDCHNLS_32;
    switch (g_soundChannels) {
        case 0:
            btn = RDO_ADVANCED_SNDCHNLS_NONE;
            break;
        case 4:
            btn = RDO_ADVANCED_SNDCHNLS_4;
            break;
        case 8:
            btn = RDO_ADVANCED_SNDCHNLS_8;
            break;
        case 16:
            btn = RDO_ADVANCED_SNDCHNLS_16;
            break;
    }
    CheckRadioButton(dlg, RDO_ADVANCED_SNDCHNLS_4, RDO_ADVANCED_SNDCHNLS_NONE, btn);

    CheckDlgButton(dlg, CHK_ADVANCED_SHADOWS, g_DrawShadows != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_FOG, g_EnableFog != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_CHROME, g_EnviroMapping != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_GROUND_LIGHTING, g_GroundLighting != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_PALETTED_TEXTURES, g_PalettedTextures != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_UNLIMITED_PARTICLESIZE, g_LimitParticleSize != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_TRIPLE_BUFFER, g_TripleBuffer != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_SCREEN_FLASHES, g_DisplayScreenFlash != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_TIRE_TREADS, g_DisplayTireTreads != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_DX_TEXTUREMANAGER, g_DXtextureManager != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_DITHERING, g_DitherEnable != 0);
    CheckDlgButton(dlg, CHK_ADVANCED_3D_SOUND, g_D3DSound != 0);
    return 1;
}

// FUNCTION: REDLINE 0x00554B0F
int CALLBACK adv_settings_cb(HWND dlg, unsigned int msg, WPARAM wParam, LPARAM lParam) {
    int res = 1;
    switch(msg) {
        case WM_COMMAND:
            adv_settings_dialog_cmd(dlg, LOWORD(wParam), lParam, HIWORD(wParam));
            return 0;
        case WM_INITDIALOG:
            return adv_settings_dialog_init(dlg, wParam, lParam);
        default:
            res = 0;
    }
    return res;
}

// FUNCTION: REDLINE 0x00554A70
int open_adv_settings(int nCmdShow) {
    bool done = false;
    while (!done) {
        switch(DialogBoxParamA(g_hInstance, (LPCSTR)DIAG_ADVANCED, g_Window, adv_settings_cb, 0)) {
            case 0:
                return 1;
            case 1:
                done = true;
                break;
        }
    }

    if (!g_Config) return 1;
    g_Config->PopulateDefaults();
    g_Config->SetStringValue("DisplayDevice", g_DisplayDevice);
    g_Config->SetStringValue("DeviceDriver", g_DeviceDriver);
    return 1;
}

// FUNCTION: REDLINE 0x00417B1A
void GetAssetFilesystemPath(const char* filename, char* unk, char* out) {
    struct Locals {
        char path[512];
        char *last_slash;
        char *dot;
        char dir[64];
        char ext[4];
        bool unpacked;
    } l;
    l.unpacked = false;
    *l.dir = 0;
    l.dot = strrchr(filename, '.');
    if (l.dot) {
        memcpy(l.ext, l.dot + 1, 3);
        l.ext[3] = NULL;
        strupr(l.ext);
        if (!strcmp(l.ext, "GEO") || !strcmp(l.ext, "GLD")) {
            strcpy(l.dir, "geo\\");
        } else if (!strcmp(l.ext, "BTF") || !strcmp(l.ext, "TGA")) {
            strcpy(l.dir, "textures\\");
        } else if (!strcmp(l.ext, "RSG")) {
            strcpy(l.dir, "saved games\\");
            l.unpacked = 1;
        } else if (!strcmp(l.ext, "WAV")) {
            strcpy(l.dir, "wav\\");
        } else if (!strcmp(l.ext, "WLD")) {
            strcpy(l.dir, "wld\\");
        } else if (!strcmp(l.ext, "ANM")) {
            strcpy(l.dir, "anm\\");
        } else if (!strcmp(l.ext, "GGR")) {
            strcpy(l.dir, "geo\\");
        } else if (!strcmp(l.ext, "MOT") || !strcmp(l.ext, "SKL")) {
            strcpy(l.dir, "motion\\");
        } else if (!strcmp(l.ext, "THG")) {
            // ???
        } else if (!strcmp(l.ext, "EVT")) {
            strcpy(l.dir, "events\\");
        } else if (!strcmp(l.ext, "BMP")) {
            strcpy(l.dir, "textures\\");
        }
    }

    strcpy(out, unk);
    if (out[strlen(out) - 1] != '\\')
        strcat(out, "\\");
    strcat(out, l.dir);
    strcat(out, filename);
    if (l.unpacked) {
        strcpy(l.path, out);
        l.last_slash = strrchr(l.path, '\\');
        if (l.last_slash)
            *l.last_slash = NULL;

        // Create directory if non-existent (original passes a zero-length string instead of dir path)
        if (GetFileAttributesA(l.last_slash) == -1)
            CreateDirectoryA(l.path, NULL);
    }
}

struct WorldHeader {
    char pad[10];
    bool is_multiplayer;
    char pad2[21];
};

// FUNCTION: REDLINE 0x0055F2DE
bool read_world_header(char* filename, WorldHeader* buf) {
    char filebuf[40];
    FileContainer cont;
    if (!cont.Read(filename, filebuf, 40))
        return false;
    if (strncmp(filebuf, "WLD", 3))
        return false;
    int version;
    memcpy(&version, &filebuf[3], sizeof(version));
    if (version < 37 || version > 41)
        return false;

    memcpy(buf, &filebuf[7], 32);
    return true;
}

// FUNCTION: REDLINE 0x00555BC3
bool is_multiplayer_world(char* name) {
    struct Locals {
        char path[128];
        short len;
        char _pad1[2];
        WorldHeader header;
        char buf[128];
    } l;
    strcpy(l.buf, name);
    l.len = strlen(l.buf);
    if (l.len > 4 && strcmpi(&l.buf[l.len - 4], ".wld"))
        strcat(l.buf, ".wld");
    GetAssetFilesystemPath(l.buf, g_GameData->data_dir, l.path);
    if (read_world_header(l.path, &l.header) && l.header.is_multiplayer)
        return 1;
    return 0;
}

// FUNCTION: REDLINE 0x00555632
int console_dlg_init(HWND dlg, int param1, int param2) {
    char entry_name[128];

    // Populate map list
    HWND widget = GetDlgItem(dlg, LST_CNSL_MAP_LIST);
    if (g_unk == 1) {
        int i = 0;
        while (true) {
            i = g_Assets.Next("wld", i);
            if (i < 0)
                break;
            g_Assets.EntryName(i, entry_name);

            char* dot = strrchr(entry_name, '.');
            if (dot)
                *dot = NULL;
            if (is_multiplayer_world(entry_name)) {
                strlwr(entry_name);
                SendMessageA(widget, LB_ADDSTRING, 0, (LPARAM)entry_name);
            }
            i++;
        }
    } else {
        char path[256];
        strcpy(path, g_GameData->data_dir);
        if (path[strlen(path) - 1] != '\\')
            strcat(path, "\\");
        GetAssetFilesystemPath("*.wld", path, entry_name);
        WIN32_FIND_DATAA ffind;
        HANDLE fhandle = FindFirstFileA(entry_name, &ffind);
        if (fhandle != (HANDLE)-1) {
            strcpy(entry_name, ffind.cFileName);
            char* dot = strrchr(entry_name, '.');
            if (dot)
                *dot = NULL;
            if (entry_name[0] != '.' && is_multiplayer_world(entry_name))
                SendMessageA(widget, LB_ADDSTRING, 0, (LPARAM)entry_name);

            while (FindNextFileA(fhandle, &ffind)) {
                strcpy(entry_name, ffind.cFileName);
                dot = strrchr(entry_name, '.');
                if (dot)
                    *dot = NULL;
                if (is_multiplayer_world(entry_name))
                    SendMessageA(widget, LB_ADDSTRING, 0, (LPARAM)entry_name);
            }
        }
    }

    widget = GetDlgItem(dlg, LST_CNSL_MAP_ORDER);
    for (short j = 0; j < 4; ++j) {
        char key[128];
        char val[128];
        sprintf(key, "Net_Level_%d", j + 1);
        int res = g_Config->GetStringValue(key, val);
        if (res != -1 && is_multiplayer_world(val)) {
            short len = strlen(val); // FIXME: Probably was supposed to be strrchr(val, '.');
            if (len > 4 && !strcmpi(&val[len], ".wld"))
                val[len] = NULL;
            SendMessageA(widget, LB_ADDSTRING, 0, (LPARAM)val);
        }
    }

    widget = GetDlgItem(dlg, EDT_CNSL_GAME_NAME);
    SetWindowTextA(widget, g_Net_GameName);
    g_PublicGame = true;
    CheckDlgButton(dlg, CHK_CNSL_PRIVATE, !g_PublicGame);
    CheckDlgButton(dlg, CHK_CNSL_TEAMS, g_Net_Teams != 0);
    CheckDlgButton(dlg, CHK_CNSL_FRIENDLY_FIRE, g_Net_FriendlyFire != 0);
    CheckDlgButton(dlg, CHK_CNSL_TEAM_PLACEMENT, g_Net_TeamPlace != 0);
    CheckDlgButton(dlg, CHK_CNSL_ADV_CTF_RULES, g_Net_Mode_CTF_Adv != 0);
    CheckRadioButton(dlg, RDO_CNSL_NET_TCP, RDO_CNSL_NET_IPX, g_Net_ConsoleTCP ? RDO_CNSL_NET_TCP : RDO_CNSL_NET_IPX);
    
    if (g_Net_Mode_ScoreLimitOn)
        sprintf(entry_name, "%d", g_Net_Mode_ScoreLimit);
    else
        sprintf(entry_name, "0");
    widget = GetDlgItem(dlg, EDT_CNSL_KILL_LIMIT);
    SetWindowTextA(widget, entry_name);

    if (g_Net_Mode_TimeLimitOn)
        sprintf(entry_name, "%d", g_Net_Mode_TimeLimit);
    else
        sprintf(entry_name, "0");
    widget = GetDlgItem(dlg, EDT_CNSL_TIME_LIMIT);
    SetWindowTextA(widget, entry_name);

    if (g_Net_MaxPlayers > 12)
        g_Net_MaxPlayers = 12;
    else if (g_Net_MaxPlayers < 2)
        g_Net_MaxPlayers = 2;
    sprintf(entry_name, "%d", g_Net_MaxPlayers);
    widget = GetDlgItem(dlg, EDT_CNSL_MAX_PLAYERS);
    SetWindowTextA(widget, entry_name);
    return 1;
}

// FUNCTION: REDLINE 0x005561B1
void console_cb_cmd_map_add(HWND dlg) {
    HWND list = GetDlgItem(dlg, LST_CNSL_MAP_ORDER);
    if (SendMessageA(list, LB_GETCOUNT, 0, 0) >= 4)
        return;

    HWND item = GetDlgItem(dlg, LST_CNSL_MAP_LIST);
    int idx = SendMessageA(item, LB_GETCURSEL, 0, 0);
    if (idx != -1) {
        char buf[128];
        SendMessageA(item, LB_GETTEXT, idx, (LPARAM)buf);
        if (!buf[0])
            return;
        SendMessageA(list, LB_ADDSTRING, 0, (LPARAM)buf);
    }
}

// FUNCTION: REDLINE 0x00556260
void console_cb_cmd_map_remove(HWND dlg) {
    HWND list = GetDlgItem(dlg, LST_CNSL_MAP_ORDER);
    int idx = SendMessageA(list, LB_GETCURSEL, 0, 0);
    if (idx != -1)
        SendMessageA(list, LB_DELETESTRING, idx, 0);
}

// FUNCTION: REDLINE 0x005562AD
void console_cb_cmd_map_clear(HWND dlg) {
    HWND list = GetDlgItem(dlg, LST_CNSL_MAP_ORDER);
    SendMessageA(list, LB_RESETCONTENT, 0, 0);
}

// FUNCTION: REDLINE 0x00555C81
void console_cb_cmd(HWND dlg, int btn, int param1, int param2) {
    int res = 0;
    short done = 0;
    
    switch(btn) {
        case BTN_CNSL_START:
            done = true;
            res = 1;
            break;
        case BTN_CNSL_MAP_ADD:
            console_cb_cmd_map_add(dlg);
            break;
        case BTN_CNSL_MAP_REMOVE:
            console_cb_cmd_map_remove(dlg);
            break;
        case BTN_CNSL_MAP_CLEAR:
            console_cb_cmd_map_clear(dlg);
            break;
        case IDCANCEL:
        case BTN_CNSL_EXIT:
            EndDialog(dlg, 0);
            break;
    }
    if (done) {
        HWND map_order = GetDlgItem(dlg, LST_CNSL_MAP_ORDER);
        short count = (short)SendMessageA(map_order, LB_GETCOUNT, 0, 0);
        bool no_map_order = count < 1;
        if (count >= 1) {
            HWND game_name = GetDlgItem(dlg, EDT_CNSL_GAME_NAME);
            GetWindowTextA(game_name, g_Net_GameName, 127);

            g_Net_Teams = IsDlgButtonChecked(dlg, CHK_CNSL_TEAMS) == 1;
            g_Net_FriendlyFire = IsDlgButtonChecked(dlg, CHK_CNSL_FRIENDLY_FIRE) == 1;
            g_Net_TeamPlace = IsDlgButtonChecked(dlg, CHK_CNSL_TEAM_PLACEMENT) == 1;
            g_Net_Mode_CTF_Adv = IsDlgButtonChecked(dlg, CHK_CNSL_ADV_CTF_RULES) == 1;
            g_PublicGame = IsDlgButtonChecked(dlg, CHK_CNSL_PRIVATE) == 0;
            if (!(g_Net_ConsoleTCP = IsDlgButtonChecked(dlg, RDO_CNSL_NET_TCP) == 1)) {
                g_PublicGame = false;
            }

            char buf[128];
            GetWindowTextA(GetDlgItem(dlg, EDT_CNSL_KILL_LIMIT), buf, 127);
            g_Net_Mode_ScoreLimit = atoi(buf);
            sprintf(buf, "%d", g_Net_Mode_TimeLimit);
            GetWindowTextA(GetDlgItem(dlg, EDT_CNSL_TIME_LIMIT), buf, 127);
            g_Net_Mode_TimeLimit = atoi(buf);
            GetWindowTextA(GetDlgItem(dlg, EDT_CNSL_MAX_PLAYERS), buf, 127);
            g_Net_MaxPlayers = atoi(buf);

            g_Net_Mode_TimeLimitOn = g_Net_Mode_TimeLimit != 0;
            g_Net_Mode_ScoreLimitOn = g_Net_Mode_ScoreLimit != 0;

            if (g_Net_MaxPlayers > 12)
                g_Net_MaxPlayers = 12;
            else if (g_Net_MaxPlayers < 2)
                g_Net_MaxPlayers = 2;

            if (g_Config) {
                g_Config->PopulateDefaults();
                g_Config->SetStringValue("net_gamename", g_Net_GameName);
                g_Config->SetBoolValue("Net_Mode_ScoreLimitOn", g_Net_Mode_ScoreLimitOn);
                g_Config->SetBoolValue("Net_Mode_TimeLimitOn", g_Net_Mode_TimeLimitOn);
                g_Config->SetIntValue("Net_Mode_ScoreLimit", g_Net_Mode_ScoreLimit);
                g_Config->SetIntValue("Net_Mode_TimeLimit", g_Net_Mode_TimeLimit);
                g_Config->SetBoolValue("Net_Teams", g_Net_Teams);
                g_Config->SetBoolValue("Net_FriendlyFire", g_Net_FriendlyFire);
                g_Config->SetBoolValue("Net_TeamPlace", g_Net_TeamPlace);
                g_Config->SetIntValue("Net_MaxPlayers", g_Net_MaxPlayers);
                for (short i = 0; i < count; i++) {
                    char mapname[128];
                    SendMessageA(map_order, LB_GETTEXT, i, (LPARAM)mapname);
                    short mapname_len = strlen(mapname);
                    if (mapname_len > 4 && strcmpi(&mapname[mapname_len], ".wld"))
                        strcat(mapname, ".wld");
                    sprintf(buf, "Net_Level_%d", i + 1);
                    g_Config->SetStringValue(buf, mapname);
                }
                char next_level[128];
                g_Config->GetStringValue("Net_Level_1", next_level);
                sprintf(g_NextMap, "%s", next_level);
            }
            EndDialog(dlg, res);
        }
    }
}

// FUNCTION: REDLINE 0x005555B5
int CALLBACK console_cb(HWND dlg, unsigned int msg, WPARAM wParam, LPARAM lParam) {
    int res = 1;
    switch(msg) {
        case WM_COMMAND:
            console_cb_cmd(dlg, LOWORD(wParam), lParam, HIWORD(wParam));
            return 0;
        case WM_INITDIALOG:
            return console_dlg_init(dlg, wParam, lParam);
        default:
            res = 0;
    }
    return res;
}

// FUNCTION: REDLINE 0x00555552
int open_console_setup(int nCmdShow) {
    bool done = false;
    int res = 0;
    while (!done) {
        res = DialogBoxParamA(g_hInstance, (LPCSTR)DIAG_CNSL, g_Window, console_cb, 0);
        switch(res) {
            case 0:
                return res;
            case 1:
                done = true;
                break;
        }
    }

    return res;
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

// GLOBAL: REDLINE 0x005CEBC0
int g_RenderFlagUnk = 0;

// FUNCTION: REDLINE 0x005535AB
char InitializeGraphics(int a) {
    if (g_RenderFlagUnk && (g_RenderFlagUnk == 1 && (a & 4) == 0)) {
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
        if (!res) {
            if ((a & 4) == 0) {
                MessageBoxA(
                        NULL,
                        "Redline requires 3D hardware acceleration.\n"
                        "Verify that your 3D card is installed correctly, and that its drivers are current.\n"
                        "\n"
                        "\n"
                        "Consult the readme file for more information.",
                        "Redline can't find a 3D accelerator",
                        MB_ICONWARNING);
            }
            return 0;
        } else {
            if ((a & 4) != 0)
                g_RenderFlagUnk = 1;
            else
                g_RenderFlagUnk = 2;
        }
    }
    return 1;
}

// FUNCTION: REDLINE 0x0053A977
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
        if (open_console_setup(g_nCmdShow) == 0)
            return 0;
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
            int res = open_settings(g_nCmdShow);
            switch (res) {
                case 1:
                    open_display_settings = 0;
                    break;
                case 2:
                    open_adv_settings(g_nCmdShow);
                    break;
            }
        }
    }

    // TODO: Set global bool
    if (g_unkBool) v5 = 0;
    StateNode* next = g_StateTree->Next(v5);
    if (!next)
        return 0;
    if (g_EngineState->QueueState(next->state_id) != next->state_id)
        return 0;
    g_DebugStartupComplete = true;
    return true;
}
