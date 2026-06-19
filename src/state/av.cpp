#include "av.h"

#include <smack.h>
#include <ddraw.h>

#include <windows.h>
#include <winuser.h>

#include "../config.h"
#include "../enginestate.h"
#include "../globals.h"
#include "../log.h"
#include "../render.h"

#include <stdio.h>

// GLOBAL: REDLINE 0x005CCF64
int g_SmackUseMMX;

// GLOBAL: REDLINE 0x005CCF58
void* g_SmackAllocUnk;

// GLOBAL: REDLINE 0x005ccfdc
LPDIRECTDRAW g_SmackDirectDraw;

// GLOBAL: REDLINE 0x005CCF68
DDSURFACEDESC g_SmackSurfaceDesc;

// GLOBAL: REDLINE 0x005CCFE0
LPDIRECTDRAWSURFACE g_SmackSurface;

// GLOBAL: REDLINE 0x005CCFD8
Smack* g_SmackVideo;

// GLOBAL: REDLINE 0x005CCFE8
HSMACKBLIT g_SmackBlit;

// GLOBAL: REDLINE 0x005CCF60
int g_SmackDDSurfaceType;

// GLOBAL: REDLINE 0x005CCFD4
int g_SmackIsSoftwareCursor;

// GLOBAL: REDLINE 0x005CCF5C
HCURSOR g_SmackCursor;

// GLOBAL: REDLINE 0x00597BB4
const int g_SmackBlitFlags[5] = {1, 2, 4, 8, 1};
// GLOBAL: REDLINE 0x005CCFEC
int g_SmackUnk = 0;


// STUB: REDLINE 0x0043CBE0
void SomeDebug(const char* msg) {
    // In reality this is empty
    g_Log.Debug(msg);
}

// FUNCTION: REDLINE 0x00533C3D
void DrawIntro(HWND window) {
    int cursor_check;
    if (GetFocus() == window) {
        if (g_SmackVideo->NewPalette)
            SmackBlitSetPalette(g_SmackBlit, g_SmackVideo->Palette, g_SmackVideo->PalType);
        SmackDoFrame(g_SmackVideo);
        if (g_SmackIsSoftwareCursor)
            cursor_check = SmackCheckCursor(window, 0, 0, 2 * g_SmackVideo->Width, 2 * g_SmackVideo->Height);
        while (g_SmackSurface->Lock(0, &g_SmackSurfaceDesc, 1, 0) == DDERR_SURFACELOST) {
            if (g_SmackSurface->Restore())
                goto err;
        }
        while (SmackToBufferRect(g_SmackVideo, 0)) {
            SmackBlit(
                    g_SmackBlit,
                    g_SmackSurfaceDesc.lpSurface,
                    g_SmackSurfaceDesc.lPitch,
                    g_SmackVideo->LastRectx,
                    g_SmackVideo->LastRecty,
                    g_SmackAllocUnk,
                    g_SmackVideo->Width,
                    g_SmackVideo->LastRectx,
                    g_SmackVideo->LastRecty,
                    g_SmackVideo->LastRectw,
                    g_SmackVideo->LastRecth);
        }
        g_SmackSurface->Unlock(&g_SmackSurfaceDesc);
        SmackNextFrame(g_SmackVideo);
    }

err:
    if (g_SmackIsSoftwareCursor)
        SmackRestoreCursor(cursor_check);
}

// FUNCTION: REDLINE 0x00533840
int PlayIntroVideo() {
    HWND window = g_Window;
    if (!window) {
        SomeDebug("Smacker: no WHND");
        return 0;
    }

    short video_idx = 0;
    g_SmackUseMMX = SmackUseMMX(1);
    SmackSoundUseDirectSound(0);
    
    g_SmackAllocUnk = radmalloc(307200);
    if (!g_SmackAllocUnk) {
        SomeDebug("Smacker: radmalloc failed");
    } else {
        if (DirectDrawCreate(NULL, &g_SmackDirectDraw, NULL)) {
            SomeDebug("Smacker: DD create failed");
        } else if (g_SmackDirectDraw->SetCooperativeLevel(window, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE)) {
            SomeDebug("Smacker: IDirectDraw_SetCooperativeLevel");
            g_SmackDirectDraw->Release();
        } else if (g_SmackDirectDraw->SetDisplayMode(640, 480, 16)) {
            SomeDebug("Smacker: IDirectDraw_SetDisplayMode");
            g_SmackDirectDraw->Release();
        } else {
            memset(&g_SmackSurfaceDesc, 0, sizeof(g_SmackSurfaceDesc));
            g_SmackSurfaceDesc.dwSize = sizeof(g_SmackSurfaceDesc);
            g_SmackSurfaceDesc.dwFlags = 1;
            g_SmackSurfaceDesc.ddsCaps.dwCaps = 512;
            if (g_SmackDirectDraw->CreateSurface(&g_SmackSurfaceDesc, &g_SmackSurface, NULL)) {
                SomeDebug("Smacker: IDirectDraw_CreateSurface");
            } else {
                SetWindowPos(window, NULL, 0, 0, 640, 480, 2);
                Sleep(1000);
                while (true) {
                    if (video_idx == 0)
                        g_SmackVideo = SmackOpen("Accolade_Logo.smk", SMACKTRACKS, -1);
                    if (video_idx == 1)
                        g_SmackVideo = SmackOpen("REDLINE_logo.smk", SMACKTRACKS, -1);
                    if (video_idx == 2)
                        g_SmackVideo = NULL;
                    video_idx++;
                    if (!g_SmackVideo)
                        break;

                    SmackToBuffer(g_SmackVideo, 0, 0, g_SmackVideo->Width, g_SmackVideo->Height, g_SmackAllocUnk, 0);
                    g_SmackBlit = SmackBlitOpen(SMACKBUFFER565 | SMACKBLIT1X);
                    if (!g_SmackBlit)
                        break;
                    g_SmackDDSurfaceType = SmackDDSurfaceType(g_SmackSurface);
                    g_SmackIsSoftwareCursor = SmackIsSoftwareCursor(g_SmackSurface, g_SmackCursor);
                    SmackBlitSetFlags(g_SmackBlit, g_SmackBlitFlags[g_SmackUnk] | g_SmackDDSurfaceType);
                    if (g_SmackDDSurfaceType != -1) {
                        while (true) {
                            if(!SmackWait(g_SmackVideo)) {
                                DrawIntro(window);
                                if (g_SmackVideo->FrameNum == g_SmackVideo->Frames - 1)
                                    break;
                                if ((GetAsyncKeyState(32) & 0x8000) != 0
                                        || (GetAsyncKeyState(13) & 0x8000) != 0
                                        || (GetAsyncKeyState(27) & 0x8000) != 0)
                                    break;
                            }
                        }
                    }

                    SmackClose(g_SmackVideo);
                    if (g_SmackBlit)
                        SmackBlitClose(g_SmackBlit);
                }
            }
        }
    }
    if (g_SmackDirectDraw->SetCooperativeLevel(window, 8))
        SomeDebug("Smacker: IDirectDraw_SetCooperativeLevel");
    g_SmackDirectDraw->RestoreDisplayMode();
    if (g_SmackSurface)
        g_SmackSurface->Release();
    if (g_SmackDirectDraw)
        g_SmackDirectDraw->Release();
    return true;
}

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
            PlayIntroVideo();
        }
    }
    g_WindowCreated = 1;
    return true;
}

// STUB: REDLINE 0x00555196
int ConsoleServerEntry(int unk) {return 1;}

// GLOBAL: REDLINE 0x005A8F54
bool g_RendererInited = false;

// FUNCTION: REDLINE 0x0048ED04
int InitRenderer(HWND window, short dev_idx, short d3d_idx, unsigned short width, unsigned short height, unsigned short bpp, int flags) {
    g_RendererInited = false;
    int res = g_Direct3d->InitializeDevice(window, dev_idx, d3d_idx, width, height, bpp, flags);
    if (res > 0) {
        SetCapGlobals();
        g_RendererInited = true;
    }
    return res;
}

// FUNCTION: REDLINE 0x0055637E
short UnkAgain(short dev_idx, short driver_idx, short width, short height, short bpp) {
    // TODO: Lock a mutex
    ClearViewport(2);
    BeginScene();
    if (EndScene()) {
        // TODO
    }
    // TODO: Unload textures?
    int flags = 0x1C | (g_Windowed == 0 ? 2 : 0);
    if (InitRenderer(g_Window, dev_idx, driver_idx, width, height, bpp, flags) <= 0) {
        // TODO: Unlock mutex
        return 0;
    }

    RenderText(5, 10, "Redline Loading...");
    FlipDisplay();
    if (g_Windowed) {
        tagPOINT p;
        p.x = p.y = 0;
        ClientToScreen(g_Window, &p);
        SetWindowOrigin(p.x, p.y);
    }
    short unk = 1;
    // TODO: Set globals
    while (true) {}
}

// FUNCTION: REDLINE 0x005562DA
int UnkSomethingElse() {
    short dev = DeviceByName(g_DisplayDevice);
    if (dev == -1) {
        dev = BestDevice();
        strcpy(g_DisplayDevice, DeviceName(dev));
    }
    short d3d_dev = D3dDeviceByName(dev, g_DeviceDriver);
    if (d3d_dev == -1)
        d3d_dev = 0;

    return UnkAgain(dev, d3d_dev, g_ScreenWidth, g_ScreenHeight, g_ScreenBPP);
}

// FUNCTION: REDLINE 0x00552775
int UnkSomething() {
    // sub_48F167(g_ConsoleEnabled == 0);
    if (g_ConsoleEnabled) {
        if (!ConstructGraphicsGlobals())
            return 0;
    } else if (!InitializeGraphics(4) && !InitializeGraphics(0)) {
        return 0;
    }

    // TODO: A bunch of global class initializations

    if (!g_ConsoleEnabled) {
        while(!UnkSomethingElse()) {
            ShowWindow(g_Window, 0);
            if (!g_Windowed)
                ShowCursor(true);
            bool exit = false;
            if (g_ScreenWidth == 640 && g_ScreenHeight == 480 && g_ScreenBPP == 16) {
                short dev = DeviceByDisplayName(g_DisplayDevice);
                char msg[256];
                if (g_Windowed) {
                    sprintf(
                            msg,
                            "Failed to setup 3D Device:\n"
                            "%s\n"
                            "\n"
                            "Windowed mode or requested screen resolution may not be supported.\n"
                            "Consult the readme file for more information.",
                            DeviceDisplayName(dev));
                } else {
                    sprintf(
                            msg,
                            "Failed to setup 3D Device:\n"
                            "%s\n"
                            "\n"
                            "Screen resolution may not be supported.\n"
                            "Consult the readme file for more information.",
                            DeviceDisplayName(dev));
                }
                MessageBoxA(g_Window, msg, NULL, MB_ICONWARNING);
                exit = true;
            } else {
                g_ScreenWidth = 640;
                g_ScreenHeight = 480;
                g_ScreenBPP = 16;
                char buf[256];
                sprintf(
                        buf,
                        "The resolution requested will not work with Redline and your 3D device.\n"
                        "The default resolution of 640x480 x16 will be used.");
                if (MessageBoxA(g_Window, buf, NULL, MB_ICONWARNING | MB_OKCANCEL) == 2) {
                    exit = true;
                }
            }

            if (!g_Windowed)
                ShowCursor(false);
            ShowWindow(g_Window, 5);
            if (exit)
                return 0;
        }
        g_Config->Write();
        // TODO
    }
    // TODO
    return 1;
}

// FUNCTION: REDLINE 0x0053E307
bool StateImpl::AV::Init(int next_state) {
    if (g_ConsoleEnabled) {
        ConsoleServerEntry(0);
    } else if (OpenWindow() != 1) {
        return 0;
    }
    if (UnkSomething() != 1)
        return 0;
    while (true) {}
    // TODO
    return true;
}
