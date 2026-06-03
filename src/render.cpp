#include "render.h"

#include "globals.h"
#include "log.h"
#include "registry.h"

#include <d3d.h>
#include <d3dcaps.h>
#include <d3dtypes.h>
#include <ddraw.h>
#include <stdio.h>
#include <windows.h>

// GLOBAL: REDLINE 0x005A7FE0
unsigned int g_VramBudget = 0;
// GLOBAL: REDLINE 0x005A7FE4
int g_FramebufferCount = 0; // uncertain if name is correct

void D3dRenderer::VtablePad() {}

// FUNCTION: REDLINE 0x00455770
Renderer::Renderer() {
    this->device_count = 0;
    this->devices_inited = -1;
    // *(_DWORD *)&this->pad1[20] = 0;
    // *(_WORD *)this->pad1 = 0;
    // *(_WORD *)&this->pad1[2] = 0;
    // *(_WORD *)&this->pad1[4] = 0;
    this->devices = NULL;
    this->display_modes = NULL;
    this->ddraw = NULL;
    // *(_DWORD *)&this->pad2[16] = 0;
    // *(_DWORD *)&this->pad2[8] = 0;
    // *(_DWORD *)this->pad2 = 0;
    // *(_DWORD *)&this->pad2[12] = 0;
    // *(_DWORD *)&this->pad2[4] = 0;
    // *(_DWORD *)&this->pad1[24] = 0;
}

// FUNCTION: REDLINE 0x00450940
D3dRenderer::D3dRenderer() : Renderer() {
    this->unk = -1;
    this->device_thing = NULL;
    this->d3d = NULL;
    // *(_DWORD *)&this->pad3[1376] = 0;
    // *(_DWORD *)&this->pad4[44] = 0;
    // *(_DWORD *)&this->pad4[304] = 0;
    // *(_WORD *)&this->pad4[316] = 1;
    // *(_DWORD *)&this->pad3[4] = 0;
    // memset(&this->pad4[240], 0, 0x40u);
    // *(_DWORD *)&this->pad4[240] = 1065353216;
    // *(_DWORD *)&this->pad4[260] = 1065353216;
    // *(_DWORD *)&this->pad4[280] = 1065353216;
    // *(_DWORD *)&this->pad4[300] = 1065353216;
}

// FUNCTION: REDLINE 0x00450A64
void D3dRenderer::DeviceThing() {
    if (this->device_count)
        this->device_thing = new DevThing[this->device_count];
    for (short i = 0; i < this->device_count; ++i) {
        this->device_thing[i].a = 0;
        this->device_thing[i].count = 0;
    }
}

// FUNCTION: REDLINE 0x00456946
int D3dRenderer::SupportsResolution(unsigned short width, unsigned short height, unsigned short bpp) {
    for (short i = 0; i < this->display_modes[this->devices_inited].mode_count; ++i) {
        if (this->display_modes[this->devices_inited].modes[i].width == width
            && this->display_modes[this->devices_inited].modes[i].height == height
            && this->display_modes[this->devices_inited].modes[i].bpp == bpp) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: REDLINE 0x0045A517
int D3dRenderer::AddMode(short width, short height, short bpp) {
    short i = this->display_modes[this->devices_inited].mode_count;
    this->display_modes[this->devices_inited].modes[i].width = width;
    this->display_modes[this->devices_inited].modes[i].height = height;
    this->display_modes[this->devices_inited].modes[i].bpp = bpp;
    this->display_modes[this->devices_inited].modes[i].unk = 0;
    ++this->display_modes[this->devices_inited].mode_count;
    return 1;
}

// FUNCTION: REDLINE 0x0045A5E8
int D3dRenderer::AddDevice(GUID *guid, const char *device_name, const char *device_name_full) {
    strcpy(this->devices[this->device_count].name, device_name);
    sprintf(this->devices[this->device_count].display_name,
            "%s (%s)",
            device_name_full,
            device_name);

    if (this->devices[this->device_count].guid != NULL) {
        delete this->devices[this->device_count].guid;
        this->devices[this->device_count].guid = NULL;
    }
    if (guid) {
        this->devices[this->device_count].guid = new GUID;
        *this->devices[this->device_count].guid = *guid;
    }
    ++this->device_count;
    return 1;
}

// FUNCTION: REDLINE 0x00491511
int HexToInt(const char *hex, short chars) {
    char c;
    int res = 0;
    while (true) {
        c = *hex;
        hex++;
        if (!c || !chars)
            break;
        if (iswdigit(c)) {
            c -= '0';
        } else if (c >= 'A' && c <= 'F') {
            c += 10 - 'A';
        } else if (c >= 'a' && c <= 'f') {
            c += 10 - 'a';
        } else {
            c = 0;
        }
        res = (res << 4) + c;
        --chars;
    }
    return res;
}

// FUNCTION: REDLINE 0x00491411
void GuidFromString(GUID *guid, const char *src) {
    guid->Data1 = HexToInt(src, 8);
    guid->Data2 = HexToInt(&src[9], 4);
    guid->Data3 = HexToInt(&src[14], 4);
    guid->Data4[0] = HexToInt(&src[19], 2);
    guid->Data4[1] = HexToInt(&src[21], 2);
    guid->Data4[2] = HexToInt(&src[24], 2);
    guid->Data4[3] = HexToInt(&src[26], 2);
    guid->Data4[4] = HexToInt(&src[28], 2);
    guid->Data4[5] = HexToInt(&src[30], 2);
    guid->Data4[6] = HexToInt(&src[32], 2);
    guid->Data4[7] = HexToInt(&src[34], 2);
}

// GLOBAL: REDLINE 0x005A7FDC
short g_D3dDeviceCount = 0;

// FUNCTION: REDLINE 0x0045a712
int CALLBACK AddDeviceCb(GUID *guid, char *name, char *src, void *userdata) {
    if (userdata == NULL) {
        ++g_D3dDeviceCount;
        return 1;
    } else {
        if (((D3dRenderer *)userdata)->AddDevice(guid, src, name)) {
            return 1;
        }
        return 0;
    }
}

// FUNCTION: REDLINE 0x0045a757
HRESULT WINAPI EnumDisplayModeCb(LPDDSURFACEDESC2 desc, void *userdata) {
    if (desc->ddpfPixelFormat.dwRGBBitCount < 16)
        return 1;

    int framebuffer_size =
        (desc->dwWidth * desc->dwHeight * desc->ddpfPixelFormat.dwRGBBitCount) /
        8;
    framebuffer_size *= g_FramebufferCount;
    float aspect = (float)desc->dwHeight / (float)desc->dwWidth;
    if (desc->dwWidth < 640 || desc->dwHeight < 480 ||
        !(aspect == 3.0f / 4.0f || aspect == 4.0f / 5.0f))
        return 1;
    if (!userdata) {
        ++g_D3dDeviceCount;
        return 1;
    }
    if (((D3dRenderer *)userdata)
            ->AddMode((short)desc->dwWidth,
                      (short)desc->dwHeight,
                      (short)desc->ddpfPixelFormat.dwRGBBitCount))
        return 1;
    return 0;
}

// FUNCTION: REDLINE 0x00456C35
void D3dRenderer::QueryMemory() {
    HRESULT res;
    DWORD total = 0;
    DWORD free = 0;

    DDSCAPS2 caps;
    memset(&caps, 0, sizeof(caps));
    caps.dwCaps = DDSCAPS_VIDEOMEMORY;
    res = this->ddraw->GetAvailableVidMem(&caps, &total, &free);
    if (res != 0) {
        total = 0;
        free = 0;
    }
    this->vram_total = total;
    this->vram_free = free;
    caps.dwCaps = DDSCAPS_LOCALVIDMEM;
    res = this->ddraw->GetAvailableVidMem(&caps, &total, &free);
    if (res != 0) {
        total = 0;
        free = 0;
    }
    this->lvram_total = total;
    this->lvram_free = free;
    caps.dwCaps = DDSCAPS_NONLOCALVIDMEM;
    res = this->ddraw->GetAvailableVidMem(&caps, &total, &free);
    if (res != 0) {
        total = 0;
        free = 0;
    }
    this->nlvram_total = total;
    this->nlvram_free = free;
    caps.dwCaps = DDSCAPS_TEXTURE;
    res = this->ddraw->GetAvailableVidMem(&caps, &total, &free);
    if (res != 0) {
        total = 0;
        free = 0;
    }
    this->texmem_total = total;
    this->texmem_free = free;
}

// GLOBAL: REDLINE 0x005A7FD8
unsigned int g_EnumDevicesFlags;

// FUNCTION: REDLINE 0x004535BB
int D3dRenderer::AddD3dDevice(GUID* guid, char* name, LPD3DDEVICEDESC desc, short unk) {
    memset(&this->device_thing[this->devices_inited].a[this->unk], 0, sizeof(DeviceMeta));
    strcpy(this->device_thing[this->devices_inited].a[this->unk].name, name);
    
    if (this->device_thing[this->devices_inited].a[this->unk].guid) {
        delete this->device_thing[this->devices_inited].a[this->unk].guid;
        this->device_thing[this->devices_inited].a[this->unk].guid = NULL;
    }
    if (guid) {
        this->device_thing[this->devices_inited].a[this->unk].guid = new GUID();
        *this->device_thing[this->devices_inited].a[this->unk].guid = *guid;
    }
    memcpy(&this->device_thing[this->devices_inited].a[this->unk].desc, desc, sizeof(D3DDEVICEDESC));

    this->device_thing[this->devices_inited].a[this->unk].unk2 = unk;
    this->device_thing[this->devices_inited].a[this->unk].supports_perspective =
        (desc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE) != 0;

    // FIXME: Original developers used OR instead of AND, which is wrong...
    if (desc->dwFlags | (DWORD)D3DDD_DEVICEZBUFFERBITDEPTH) {
        this->device_thing[this->devices_inited].a[this->unk].has_z_depth =
            desc->dwDeviceZBufferBitDepth != 0;
        this->device_thing[this->devices_inited].a[this->unk].z_bit_depth = desc->dwDeviceZBufferBitDepth;
    }

    if (desc->dwFlags | D3DDD_DEVICERENDERBITDEPTH) {
        this->device_thing[this->devices_inited].a[this->unk].render_bit_depth = desc->dwDeviceRenderBitDepth;
    }
    
    short score = 0;
    if (desc->dwFlags | D3DDD_DEVCAPS) {
        this->device_thing[this->devices_inited].a[this->unk].can_render_after_flip =
            (desc->dwDevCaps & D3DDEVCAPS_CANRENDERAFTERFLIP) != 0;
        this->device_thing[this->devices_inited].a[this->unk].texture_nlvram =
            (desc->dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM) != 0;
        this->device_thing[this->devices_inited].a[this->unk].texture_vram =
            (desc->dwDevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY) != 0;
        score += (desc->dwDevCaps & D3DDEVCAPS_FLOATTLVERTEX) != 0;
    }

    this->device_thing[this->devices_inited].a[this->unk].max_blend_stages =
        desc->wMaxTextureBlendStages;
    short supports_blend = 0;
    short supports_blend2 = 0;
    if (desc->dwFlags | D3DDD_TRICAPS) {
        this->device_thing[this->devices_inited].a[this->unk].has_linearmiplinear =
            (desc->dpcTriCaps.dwTextureFilterCaps & D3DFILTER_LINEARMIPLINEAR) != 0;
        this->device_thing[this->devices_inited].a[this->unk].aa_edges =
        (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASEDGES) != 0;
        this->device_thing[this->devices_inited].a[this->unk].aa_sortdependent =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT) != 0;
        this->device_thing[this->devices_inited].a[this->unk].aa_sortindependent =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT) != 0;
        this->device_thing[this->devices_inited].a[this->unk].dither =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_DITHER) != 0;
        this->device_thing[this->devices_inited].a[this->unk].fogtable =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGTABLE) != 0;
        this->device_thing[this->devices_inited].a[this->unk].fogvertex =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX) != 0;
        this->device_thing[this->devices_inited].a[this->unk].fogrange =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGRANGE) != 0;
        this->device_thing[this->devices_inited].a[this->unk].zbuf_less_hsr =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR) != 0;

        this->device_thing[this->devices_inited].a[this->unk].subpixel =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_SUBPIXEL) != 0;
        this->device_thing[this->devices_inited].a[this->unk].wbuffer =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_WBUFFER) != 0;

        this->device_thing[this->devices_inited].a[this->unk].tex_squareonly =
            (desc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) != 0;
        this->device_thing[this->devices_inited].a[this->unk].tex_pow2 =
            (desc->dpcTriCaps.dwRasterCaps & D3DPTEXTURECAPS_POW2) != 0;

        this->device_thing[this->devices_inited].a[this->unk].tex_blend_caps =
            desc->dpcTriCaps.dwTextureBlendCaps;
        this->device_thing[this->devices_inited].a[this->unk].src_blend_caps =
            desc->dpcTriCaps.dwSrcBlendCaps;

        int srcblend = this->device_thing[this->devices_inited].a[this->unk].src_blend_caps;
        this->device_thing[this->devices_inited].a[this->unk].dst_blend_caps =
            desc->dpcTriCaps.dwDestBlendCaps;
        
        if ((srcblend & D3DPBLENDCAPS_ONE) != 0
                && (this->device_thing[this->devices_inited].a[this->unk].dst_blend_caps
                    & D3DPBLENDCAPS_SRCALPHA) != 0) {
            supports_blend = 1;
            ++score;
        }
        if ((srcblend & D3DPBLENDCAPS_BOTHINVSRCALPHA) != 0) {
            supports_blend2 = 1;
            ++score;
        }
    }

    score +=
        (this->device_thing[this->devices_inited].a[this->unk].has_linearmiplinear != 0)
        + (this->device_thing[this->devices_inited].a[this->unk].aa_edges != 0)
        + (this->device_thing[this->devices_inited].a[this->unk].aa_sortindependent != 0)
        + (this->device_thing[this->devices_inited].a[this->unk].dither != 0)
        + (this->device_thing[this->devices_inited].a[this->unk].fogtable != 0)
        + (this->device_thing[this->devices_inited].a[this->unk].subpixel != 0)
        + (this->device_thing[this->devices_inited].a[this->unk].wbuffer != 0);

    score += (this->SupportsResolution(1024, 768, 16) != 0);
    score += (this->SupportsResolution(1280, 1024, 16) != 0);
    score +=    this->SupportsResolution(640, 480, 32);
    this->device_thing[this->devices_inited].a[this->unk].device_score = score;

    this->device_thing[this->devices_inited].a[this->unk].supports_blend2 = supports_blend2;
    this->device_thing[this->devices_inited].a[this->unk].supports_blend = supports_blend;

    ++this->unk;
    return 1;
}

// FUNCTION: REDLINE 0x0045565B
HRESULT CALLBACK EnumDevicesCb(GUID *guid, char *deviceDesc, char *deviceName, LPD3DDEVICEDESC hwdesc, LPD3DDEVICEDESC heldesc, void *userdata) {
    LPD3DDEVICEDESC desc = hwdesc;
    unsigned short supports_rgb = 0;
    if ((hwdesc->dcmColorModel & D3DCOLOR_RGB) != 0) {
        supports_rgb = 1;
        desc = hwdesc;
    } else if (g_EnumDevicesFlags & 2) { // TODO: Flag meaning
        return 1;
    }

    if (!userdata) {
        ++g_D3dDeviceCount;
        return 1;
    }

    if (!((D3dRenderer*)userdata)->AddD3dDevice(guid, deviceName, desc, supports_rgb)) {
        return 0;
    }
    return 1;
}

// FUNCTION: REDLINE 0x00454024
int D3dRenderer::AddZBufferFormat(LPDDPIXELFORMAT fmt) {
    memcpy(&this->device_thing[this->devices_inited].a[this->unk].zbuffer_formats[this->zbuffer_fmt_count],
            fmt,
            sizeof(DDPIXELFORMAT));
    this->zbuffer_fmt_count++;
    return 1;
}

// FUNCTION: REDLINE 0x00455709
HRESULT CALLBACK EnumZBufferFormatsCb(LPDDPIXELFORMAT fmt, void* userdata) {
    if (!userdata) {
        ++g_D3dDeviceCount;
        return 1;
    }
    if (((D3dRenderer*) userdata)->AddZBufferFormat(fmt)) {
        return 1;
    }
    return 0;
}

// FUNCTION: REDLINE 0x004531C4
int D3dRenderer::QueryDevices() {
    if (this->d3d) {
        this->d3d->Release();
        this->d3d = NULL;
    }
    int res = this->ddraw->QueryInterface(IID_IDirect3D3, (void**)&this->d3d);
    if (res) {
        g_Log.DxErr("This App Requires DirectX 6 or above", res);
        return -1;
    }

    g_EnumDevicesFlags = this->flags;
    g_D3dDeviceCount = 0;
    g_Log.Print("Enumerating D3D Devices");
    res = this->d3d->EnumDevices(EnumDevicesCb, NULL);
    if (res) {
        g_Log.D3dErr("EnumDevices", res);
        return 0;
    }
    if (!g_D3dDeviceCount) return 0;

    this->device_thing[this->devices_inited].a = new DeviceMeta[g_D3dDeviceCount]();
    this->device_thing[this->devices_inited].count = g_D3dDeviceCount;
    for (short i = 0; i < g_D3dDeviceCount; ++i) {
        this->device_thing[this->devices_inited].a[i].guid = 0;
        this->device_thing[this->devices_inited].a[i].zbuffer_formats = 0;
    }
    this->unk = 0;
    res = this->d3d->EnumDevices(EnumDevicesCb, this);
    if (res) {
        g_Log.D3dErr("EnumDevices", res);
        return 0;
    }

    for (this->unk = 0; this->unk < this->device_thing[this->devices_inited].count; ++this->unk) {
        g_D3dDeviceCount = 0;
        res = this->d3d->EnumZBufferFormats((REFCLSID)*this->device_thing[this->devices_inited].a[this->unk].guid, EnumZBufferFormatsCb, NULL);
        if (res) {
            g_Log.D3dErr("EnumZBufferFormats counts", res);
            return 0;
        }
        this->device_thing[this->devices_inited].a[this->unk].zbuffer_format_count = g_D3dDeviceCount;
        this->zbuffer_fmt_count = 0;
        if (g_D3dDeviceCount > 0) {
            this->device_thing[this->devices_inited].a[this->unk].zbuffer_formats = new DDPIXELFORMAT[g_D3dDeviceCount];
            res = this->d3d->EnumZBufferFormats((REFCLSID)*this->device_thing[this->devices_inited].a[this->unk].guid, EnumZBufferFormatsCb, this);
            if (res) {
                g_Log.D3dErr("EnumZbufferFormats", res);
                return 0;
            }
        }
    }
    this->unk = -1;
    if (this->d3d) {
        this->d3d->Release();
        this->d3d = NULL;
    }

    return 1;
}

// FUNCTION: REDLINE 0x00459FDE
int D3dRenderer::DeviceInit() {
    if (this->display_modes || !this->device_count)
        return 0;
    this->DeviceThing();
    this->display_modes = new DisplayModes[this->device_count]();
    for (short i = 0; i < this->device_count; ++i) {
        this->display_modes[i].mode_count = 0;
        this->display_modes[i].modes = NULL;
    }

    if (this->ddraw) {
        this->ddraw->Release();
        this->ddraw = NULL;
    }

    int res;
    for (this->devices_inited = 0; this->devices_inited < this->device_count;
         ++this->devices_inited) {
        LPDIRECTDRAW ddraw;
        res = DirectDrawCreate(
            this->devices[this->devices_inited].guid, &ddraw, NULL);
        if (res) {
            g_Log.DxErr("Creating DirectDraw Interface", res);
            return 0;
        }

        ddraw->QueryInterface(IID_IDirectDraw4, (void **)&this->ddraw);
        ddraw->Release();

        DDCAPS driver_caps;
        DDCAPS hel_caps;
        driver_caps.dwSize = sizeof(DDCAPS);
        hel_caps.dwSize = sizeof(DDCAPS);
        this->ddraw->GetCaps(&driver_caps, &hel_caps);

        bool can_window = false;
        if (driver_caps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED)
            can_window = true;
        bool has_3d = false;
        if (driver_caps.dwCaps & DDCAPS_3D)
            has_3d = true;
        if ((this->flags & FLAGS_WINDOWED) == 0) {
            this->QueryMemory();
            if (!this->nlvram_total) {
                if (g_VramBudget <= this->texmem_total) {
                    if (g_VramBudget > 0x200000)
                        g_VramBudget -= 0x200000;
                } else {
                    g_VramBudget -= this->texmem_total;
                }
            }

            if (!this->devices[this->devices_inited].guid) {
                DDSURFACEDESC2 surface_desc;
                surface_desc.dwSize = sizeof(DDSURFACEDESC2);
                res = this->ddraw->GetDisplayMode(&surface_desc);
                if (res)
                    g_Log.DxErr("Setting Display Mode", res);
                else
                    g_VramBudget +=
                        (surface_desc.ddpfPixelFormat.dwRGBBitCount *
                         surface_desc.dwHeight * surface_desc.dwWidth) /
                        8;
            }

            this->devices[this->devices_inited].supports_mipmap =
                (driver_caps.ddsCaps.dwCaps & DDSCAPS_MIPMAP) != 0;
            g_FramebufferCount = 3;
            g_D3dDeviceCount = 0;
            res =
                this->ddraw->EnumDisplayModes(0, NULL, NULL, EnumDisplayModeCb);
            if (g_D3dDeviceCount == 0 || res) {
                g_Log.DxErr("EnumDisplayModes count", res);
                return 0;
            }
            this->display_modes[this->devices_inited].modes =
                new DisplayMode[g_D3dDeviceCount];
            res = this->ddraw->EnumDisplayModes(0, NULL, this, EnumDisplayModeCb);
            if (res) {
                g_Log.DxErr("EnumDisplayModes", res);
                return 0;
            }
        }

        int v6 = 0;
        if (has_3d && (can_window || (this->flags & FLAGS_WINDOWED) == 0)) {
            this->QueryDevices();
            if (v6 == -1)
                return -1;
        }

        if (!v6)
            this->devices[this->devices_inited].delete_flag = 1;

        if (this->ddraw) {
            this->ddraw->Release();
            this->ddraw = NULL;
        }
    }
    this->devices_inited = -1;
    return 1;
}

// FUNCTION: REDLINE 0x00450CF4
void D3dRenderer::PruneDevices(short remaining) {
    DevThing* replacement = new DevThing[remaining]();
    short replacement_idx = 0;
    for (short i = 0; i < this->device_count; ++i) {
        if (this->devices[i].delete_flag) {
            for (short j = 0; j < this->device_thing[i].count; ++j) {
                if (this->device_thing[i].a[j].guid) {
                    delete this->device_thing[i].a[j].guid;
                    this->device_thing[i].a[j].guid = NULL;
                }
                if (this->device_thing[i].a[j].zbuffer_formats) {
                    delete[] this->device_thing[i].a[j].zbuffer_formats;
                    this->device_thing[i].a[j].zbuffer_formats = NULL;
                }
            }
            if (this->device_thing[i].a) {
                delete this->device_thing[i].a;
                this->device_thing[i].a = NULL;
            }
        } else {
            replacement[replacement_idx] = this->device_thing[i];
            replacement_idx++;
        }
    }
    if (this->device_thing) {
        delete this->device_thing;
        this->device_thing = NULL;
    }
    this->device_thing = replacement;
}

// FUNCTION: REDLINE 0x00455848
void D3dRenderer::ResetDevices() {
    short i;
    for (i = 0; i < this->device_count; ++i) {
        if (this->display_modes[i].modes) {
            delete this->display_modes[i].modes;
            this->display_modes[i].modes = NULL;
        }
    }
    if (this->display_modes) {
        delete this->display_modes;
        this->display_modes = NULL;
    }
    for (i = 0; i < this->device_count; ++i) {
        if (this->devices[i].guid) {
            delete this->devices[i].guid;
            this->devices[i].guid = NULL;
        }
    }
    if (this->devices) {
        delete this->devices;
        this->devices = NULL;
    }
    this->device_count = 0;
}

// FUNCTION: REDLINE 0x00450B06
void D3dRenderer::ResetDevThing() {
    if (!this->device_thing) return;

    for (short i = 0; i < this->device_count; i++) {
        for (short j = 0; j < this->device_thing[i].count; j++) {
            if (this->device_thing[i].a[j].guid) {
                delete this->device_thing[i].a[j].guid;
                this->device_thing[i].a[j].guid = NULL;
            }
            if (this->device_thing[i].a[j].zbuffer_formats) {
                delete this->device_thing[i].a[j].zbuffer_formats;
                this->device_thing[i].a[j].zbuffer_formats = NULL;
            }
        }
        if (this->device_thing[i].a) {
            delete this->device_thing[i].a;
            this->device_thing[i].a = NULL;
        }
    }
    if (this->device_thing) {
        delete this->device_thing;
        this->device_thing = NULL;
    }
}

// FUNCTION: REDLINE 0x00450AEB
void D3dRenderer::Reset() {
    this->ResetDevThing();
    this->ResetDevices();
}

// FUNCTION: REDLINE 0x004598D9
int D3dRenderer::Initialize(int flags) {
    this->flags = flags;
    this->Reset();
    g_D3dDeviceCount = 0;

    char device_name[128];
    char device_name_full[128];
    char device_guid[64];
    GUID guid;
    ReadRegistry("Device Name", "", device_name, NULL);
    if ((flags & 4) != 0 && *device_name) {
        ReadRegistry("Device Name Full", "", device_name_full, NULL);
        ReadRegistry("Device GUID", "", device_guid, NULL);
        if (*device_guid) {
            g_D3dDeviceCount = 1;
            this->devices = new Device[g_D3dDeviceCount]();
            this->devices->guid = NULL;
            *this->devices->display_name = NULL;
            if (strcmp(device_guid, "NULL")) {
                GuidFromString(&guid, device_guid);
                this->AddDevice(&guid, device_name, device_name_full);
            } else {
                this->AddDevice(NULL, device_name, device_name_full);
            }
        }
    }
    if (!g_D3dDeviceCount) {
        int res = DirectDrawEnumerateA(AddDeviceCb, NULL);
        if (res) {
            g_Log.DxErr("DirectDrawEnumerate", res);
            return false;
        }
        if (!g_D3dDeviceCount)
            return false;
        this->devices = new Device[g_D3dDeviceCount]();
        for (short i = 0; i < g_D3dDeviceCount; ++i) {
            this->devices[i].guid = NULL;
            *this->devices[i].display_name = NULL;
        }
        res = DirectDrawEnumerateA(AddDeviceCb, this);
        if (res) {
            g_Log.DxErr("DirectDrawEnumerate", res);
            return false;
        }
    }
    char msg[64];
    if (this->device_count) {
        sprintf(msg, "[%d] DirectDraw Device(s) Found:", this->device_count);
        g_Log.Print(msg);
        for (short j = 0; j < this->device_count; ++j) {
            sprintf(msg, "  %d: %s", j + 1, this->devices[j].display_name);
            g_Log.Print(msg);
        }
        int init_res = this->DeviceInit();
        if (init_res <= 0)
            return init_res;
        short to_delete = 0;
        for (short k = 0; k < this->device_count; ++k) {
            if (this->devices[k].delete_flag)
                ++to_delete;
        }
        if (to_delete) {
            short remaining = this->device_count - to_delete;
            if (!remaining)
                return 0;
            this->PruneDevices(remaining);
            DisplayModes* replacement_dismodes = new DisplayModes[remaining]();
            Device* replacement_dev = new Device[remaining]();
            short replacement_idx = 0;
            for (short k = 0; k < this->device_count; k++) {
                if (this->devices[k].delete_flag) {
                    if (this->display_modes[k].modes) {
                        delete this->display_modes[k].modes;
                        this->display_modes[k].modes = NULL;
                    }
                    if (this->devices[k].guid) {
                        delete this->devices[k].guid;
                        this->devices[k].guid = NULL;
                    }
                } else {
                    replacement_dismodes[replacement_idx] = this->display_modes[k];
                    replacement_dev[replacement_idx] = this->devices[k];
                }
            }
            if (this->display_modes) {
                delete this->display_modes;
                this->display_modes = NULL;
            }
            this->display_modes = replacement_dismodes;
            if (this->devices) {
                delete this->devices;
                this->devices = NULL;
            }
            this->devices = replacement_dev;
            this->device_count = remaining;
        }
    }
    return this->device_count;
}

// FUNCTION: REDLINE 0x0048e9a5
int DeviceCount() {
    return g_Direct3d->device_count;
}

// FUNCTION: REDLINE 0x00456A50
char* D3dRenderer::DeviceDisplayName(short idx) {
    if (idx < 0 || idx > this->device_count)
        return 0;
    return this->devices[idx].display_name;
}

// FUNCTION: REDLINE 0x0048e9e1
char* DeviceDisplayName(short idx) {
    return g_Direct3d->DeviceDisplayName(idx);
}

// FUNCTION: REDLINE 0x00456A8C
short D3dRenderer::DeviceByName(char* name) {
    for (short i = 0; i < this->device_count; ++i) {
        if (!strcmpi(name, this->devices[i].name))
            return i;
    }
    return -1;
}

// FUNCTION: REDLINE 0x0048ea45
short DeviceByName(char* name) {
    return g_Direct3d->DeviceByName(name);
}

// FUNCTION: REDLINE 0x00456AEF
short D3dRenderer::DeviceByDisplayName(char* name) {
    for (short i = 0; i < this->device_count; ++i) {
        if (!strcmpi(name, this->devices[i].display_name))
            return i;
    }
    return -1;
}

// FUNCTION: REDLINE 0x0048E9F8
short DeviceByDisplayName(char* name) {
    return g_Direct3d->DeviceByDisplayName(name);
}

// FUNCTION: REDLINE 0x00456A14
char* D3dRenderer::DeviceName(short idx) {
    if (idx < 0 || idx > this->device_count)
        return 0;
    return this->devices[idx].name;
}

// FUNCTION: REDLINE 0x0048E9CA
char* DeviceName(short idx) {
    return g_Direct3d->DeviceName(idx);
}


// FUNCTION: REDLINE 0x00456B52
short D3dRenderer::DeviceModeCount(short idx) {
    if (idx < 0 || idx > this->device_count)
        return -1;
    return this->display_modes[idx].mode_count;
}

// FUNCTION: REDLINE 0x0048eb10
short DeviceModeCount(short idx) {
    return g_Direct3d->DeviceModeCount(idx);
}

// FUNCTION: REDLINE 0x00456B8A
short D3dRenderer::DeviceModeResolution(short idx, short mode, short* width, short* height, short* bpp) {
    if (idx < 0 || idx > this->device_count || mode < 0 || mode > this->display_modes[idx].mode_count)
        return -1;
    *width = this->display_modes[idx].modes[mode].width;
    *height = this->display_modes[idx].modes[mode].height;
    *bpp = this->display_modes[idx].modes[mode].bpp;
    return 1;
}

// FUNCTION: REDLINE 0x0048EB27
short DeviceModeResolution(short idx, short mode, short* width, short* height, short* bpp) {
    return g_Direct3d->DeviceModeResolution(idx, mode, width, height, bpp);
}

// FUNCTION: REDLINE 0x00451E67
short D3dRenderer::SupportsBitDepth(short dev, short bpp) {
    int flag = 0;
    switch (bpp) {
        case 8:
            flag = DDBD_8;
            break;
        case 16:
            flag = DDBD_16;
            break;
        case 24:
            flag = DDBD_24;
            break;
        case 32:
            flag = DDBD_32;
            break;
    }
    if (flag != 0)
        if  (flag & this->device_thing[dev].a->render_bit_depth)
            return 1;
    return 0;
}

// FUNCTION: REDLINE 0x0048ebb0
short SupportsBitDepth(short dev, short bpp) {
    return g_Direct3d->SupportsBitDepth(dev, bpp);
}

// FUNCTION: REDLINE 0x0048EB51
short FormatResolution(short dev, short mode, char* out) {
    short width;
    short height;
    short bpp;
    if (g_Direct3d->DeviceModeResolution(dev, mode, &width, &height, &bpp) < 0)
        return 0;
    sprintf(out, "%4dx%4dx%2d", width, height, bpp);
    return 1;
}

// FUNCTION: REDLINE 0x0048EC0E
short ResolutionToMode(short dev, short width, short height, short bpp) {
    short mode_width;
    short mode_height;
    short mode_bpp;
    for (short i = 0; i < DeviceModeCount(dev); ++i) {
        if (g_Direct3d->DeviceModeResolution(dev, i, &mode_width, &mode_height, &mode_bpp) < 0)
            return 0;
        if (width == mode_width && height == mode_height && bpp == mode_bpp)
            return i;
    }
    return -1;
}

// Doesn't really match...
// FUNCTION: REDLINE 0x0048ea59
short BestDevice() {
    short best_dev = 0;
    unsigned short score = g_Direct3d->device_thing->a[0].device_score;
    for (short i = 1; i < DeviceCount(); ++i) {
        if (g_Direct3d->device_thing[i].a->device_score > score) {
            score = g_Direct3d->device_thing[i].a->device_score;
            best_dev = i;
        }
    }
    return best_dev;
}
