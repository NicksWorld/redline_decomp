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
#include <wingdi.h>

// GLOBAL: REDLINE 0x005A7FE0
unsigned int g_VramBudget = 0;
// GLOBAL: REDLINE 0x005A7FE4
int g_FramebufferCount = 0; // uncertain if name is correct

// FUNCTION: REDLINE 0x00455770
Renderer::Renderer() {
    this->device_count = 0;
    this->sel_ddraw = -1;
    // *(_DWORD *)&this->pad1[14] = 0;
    this->mode_width = 0;
    this->mode_height = 0;
    this->mode_bpp = 0;
    this->ddraw_devices = NULL;
    this->display_modes = NULL;
    this->ddraw = NULL;
    this->palette = NULL;
    this->primary_surface = NULL;
    this->backbuffer = NULL;
    this->zbuffer = NULL;
    this->render_surf = NULL;
    this->fullscreen = 0;
}

// FUNCTION: REDLINE 0x00450940
D3dRenderer::D3dRenderer() : Renderer() {
      this->sel_d3d = -1;
      this->d3d_devices = NULL;
      this->d3d = NULL;
      this->d3d_device = NULL;
      this->viewport = NULL;
      this->viewport_mat = NULL;
      // *(_WORD *)this->pad4_0 = 1;
      this->texfmt_count = 0;
      memset(&this->default_matrix, 0, sizeof(this->default_matrix));
      this->default_matrix._11 = 1.0;
      this->default_matrix._22 = 1.0;
      this->default_matrix._33 = 1.0;
      this->default_matrix._44 = 1.0;
}

// FUNCTION: REDLINE 0x00451FD9
int D3dRenderer::GetCaps() {
    return this->some_flags;
}

// FUNCTION: REDLINE 0x00450A64
void D3dRenderer::DeviceThing() {
    if (this->device_count)
        this->d3d_devices = new DevThing[this->device_count];
    for (short i = 0; i < this->device_count; ++i) {
        this->d3d_devices[i].a = 0;
        this->d3d_devices[i].count = 0;
    }
}

// FUNCTION: REDLINE 0x00456946
int D3dRenderer::SupportsResolution(unsigned short width, unsigned short height, unsigned short bpp) {
    for (short i = 0; i < this->display_modes[this->sel_ddraw].mode_count; ++i) {
        if (this->display_modes[this->sel_ddraw].modes[i].width == width
            && this->display_modes[this->sel_ddraw].modes[i].height == height
            && this->display_modes[this->sel_ddraw].modes[i].bpp == bpp) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: REDLINE 0x0045A517
int D3dRenderer::AddMode(short width, short height, short bpp) {
    short i = this->display_modes[this->sel_ddraw].mode_count;
    this->display_modes[this->sel_ddraw].modes[i].width = width;
    this->display_modes[this->sel_ddraw].modes[i].height = height;
    this->display_modes[this->sel_ddraw].modes[i].bpp = bpp;
    this->display_modes[this->sel_ddraw].modes[i].unk = 0;
    ++this->display_modes[this->sel_ddraw].mode_count;
    return 1;
}

// FUNCTION: REDLINE 0x0045A5E8
int D3dRenderer::AddDevice(GUID *guid, const char *device_name, const char *device_name_full) {
    strcpy(this->ddraw_devices[this->device_count].name, device_name);
    sprintf(this->ddraw_devices[this->device_count].display_name,
            "%s (%s)",
            device_name_full,
            device_name);

    if (this->ddraw_devices[this->device_count].guid != NULL) {
        delete this->ddraw_devices[this->device_count].guid;
        this->ddraw_devices[this->device_count].guid = NULL;
    }
    if (guid) {
        this->ddraw_devices[this->device_count].guid = new GUID;
        *this->ddraw_devices[this->device_count].guid = *guid;
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
    struct Locals {
        HRESULT res;
        DDSCAPS2 caps;
        DWORD free;
        DWORD total;
    } l;
    l.total = 0;
    l.free = 0;
    memset(&l.caps, 0, sizeof(l.caps));
    l.caps.dwCaps = DDSCAPS_VIDEOMEMORY;
    l.res = this->ddraw->GetAvailableVidMem(&l.caps, &l.total, &l.free);
    if (l.res != 0) {
        l.total = 0;
        l.free = 0;
    }
    this->vram_total = l.total;
    this->vram_free = l.free;
    l.caps.dwCaps = DDSCAPS_LOCALVIDMEM;
    l.res = this->ddraw->GetAvailableVidMem(&l.caps, &l.total, &l.free);
    if (l.res != 0) {
        l.total = 0;
        l.free = 0;
    }
    this->lvram_total = l.total;
    this->lvram_free = l.free;
    l.caps.dwCaps = DDSCAPS_NONLOCALVIDMEM;
    l.res = this->ddraw->GetAvailableVidMem(&l.caps, &l.total, &l.free);
    if (l.res != 0) {
        l.total = 0;
        l.free = 0;
    }
    this->nlvram_total = l.total;
    this->nlvram_free = l.free;
    l.caps.dwCaps = DDSCAPS_TEXTURE;
    l.res = this->ddraw->GetAvailableVidMem(&l.caps, &l.total, &l.free);
    if (l.res != 0) {
        l.total = 0;
        l.free = 0;
    }
    this->texmem_total = l.total;
    this->texmem_free = l.free;
}

// GLOBAL: REDLINE 0x005A7FD8
unsigned int g_EnumDevicesFlags;

// FUNCTION: REDLINE 0x004535BB
int D3dRenderer::AddD3dDevice(GUID* guid, char* name, LPD3DDEVICEDESC desc, short unk) {
    struct Locals {
        int srcblend;
        int dstblend;
        short supports_blend2;
        short unk2;
        short supports_blend;
        short unk3;
        short score;
    } l;

    memset(&this->d3d_devices[this->sel_ddraw].a[this->sel_d3d], 0, sizeof(DeviceMeta));
    strcpy(this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].name, name);

    if (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid) {
        delete this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid = NULL;
    }
    if (guid) {
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid = new GUID();
        *this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid = *guid;
    }
    memcpy(&this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].desc, desc, sizeof(D3DDEVICEDESC));

    this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].unk2 = unk;
    this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].supports_perspective =
        (desc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE) != 0;

    // FIXME: Original developers used OR instead of AND, which is wrong...
    if (desc->dwFlags | (DWORD)D3DDD_DEVICEZBUFFERBITDEPTH) {
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].has_z_depth =
            desc->dwDeviceZBufferBitDepth != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].z_bit_depth = desc->dwDeviceZBufferBitDepth;
    }

    if (desc->dwFlags | D3DDD_DEVICERENDERBITDEPTH) {
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].render_bit_depth = desc->dwDeviceRenderBitDepth;
    }
    l.score = 0;
    if (desc->dwFlags | D3DDD_DEVCAPS) {
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].can_render_after_flip =
            (desc->dwDevCaps & D3DDEVCAPS_CANRENDERAFTERFLIP) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].texture_nlvram =
            (desc->dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].texture_vram =
            (desc->dwDevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY) != 0;
        l.score += (desc->dwDevCaps & D3DDEVCAPS_FLOATTLVERTEX) != 0;
    }

    this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].max_blend_stages =
        desc->wMaxTextureBlendStages;
    l.supports_blend = 0;
    l.supports_blend2 = 0;
    if (desc->dwFlags | D3DDD_TRICAPS) {
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].has_linearmiplinear =
            (desc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].aa_edges =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASEDGES) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].aa_sortdependent =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].aa_sortindependent =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].dither =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_DITHER) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].fogtable =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGTABLE) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].fogvertex =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].fogrange =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGRANGE) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].zbuf_less_hsr =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR) != 0;

        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].subpixel =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_SUBPIXEL) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].wbuffer =
            (desc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_WBUFFER) != 0;

        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].tex_squareonly =
            (desc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) != 0;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].tex_pow2 =
            (desc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) != 0;

        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].tex_blend_caps =
            desc->dpcTriCaps.dwTextureBlendCaps;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].src_blend_caps =
            desc->dpcTriCaps.dwSrcBlendCaps;
        l.srcblend = this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].src_blend_caps;
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].dst_blend_caps =
            desc->dpcTriCaps.dwDestBlendCaps;
        l.dstblend = this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].dst_blend_caps;
        if ((l.srcblend & D3DPBLENDCAPS_ONE) != 0 && (l.dstblend & D3DPBLENDCAPS_SRCALPHA) != 0) {
            l.supports_blend = 1;
            ++l.score;
        }
        if ((l.srcblend & D3DPBLENDCAPS_BOTHINVSRCALPHA) != 0) {
            l.supports_blend2 = 1;
            ++l.score;
        }
    }
    l.score +=
        (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].has_linearmiplinear != 0)
        + (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].aa_edges != 0)
        + (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].aa_sortindependent != 0)
        + (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].dither != 0)
        + (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].fogtable != 0)
        + (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].subpixel != 0)
        + (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].wbuffer != 0);
    l.score += (this->SupportsResolution(1024, 768, 16) != 0);
    l.score += (this->SupportsResolution(1280, 1024, 16) != 0);
    l.score += this->SupportsResolution(640, 480, 32) != 0;
    this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].device_score = l.score;
    this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].supports_blend2 = l.supports_blend2;
    this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].supports_blend = l.supports_blend;

    ++this->sel_d3d;
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
    memcpy(&this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].zbuffer_formats[this->zbuffer_fmt_count],
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
    this->d3d_devices[this->sel_ddraw].a = new DeviceMeta[g_D3dDeviceCount]();
    this->d3d_devices[this->sel_ddraw].count = g_D3dDeviceCount;
    for (short i = 0; i < g_D3dDeviceCount; ++i) {
        this->d3d_devices[this->sel_ddraw].a[i].guid = 0;
        this->d3d_devices[this->sel_ddraw].a[i].zbuffer_formats = 0;
    }
    this->sel_d3d = 0;
    res = this->d3d->EnumDevices(EnumDevicesCb, this);
    if (res) {
        g_Log.D3dErr("EnumDevices", res);
        return 0;
    }

    for (this->sel_d3d = 0; this->sel_d3d < this->d3d_devices[this->sel_ddraw].count; ++this->sel_d3d) {
        g_D3dDeviceCount = 0;
        res = this->d3d->EnumZBufferFormats((REFCLSID)*this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid, EnumZBufferFormatsCb, NULL);
        if (res) {
            g_Log.D3dErr("EnumZBufferFormats counts", res);
            return 0;
        }
        this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].zbuffer_format_count = g_D3dDeviceCount;
        this->zbuffer_fmt_count = 0;
        if (g_D3dDeviceCount > 0) {
            this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].zbuffer_formats = new DDPIXELFORMAT[g_D3dDeviceCount];
            res = this->d3d->EnumZBufferFormats((REFCLSID)*this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid, EnumZBufferFormatsCb, this);
            if (res) {
                g_Log.D3dErr("EnumZbufferFormats", res);
                return 0;
            }
        }
    }
    this->sel_d3d = -1;
    if (this->d3d) {
        this->d3d->Release();
        this->d3d = NULL;
    }

    return 1;
}

// FUNCTION: REDLINE 0x00459FDE
int D3dRenderer::DeviceInit() {
    struct Locals {
        DDSURFACEDESC2 surface_desc;
        DDCAPS driver_caps;
        bool can_window;
        char _pad2[3];
        int v6;
        bool has_3d;
        char _pad4[3];
        int res;
        LPDIRECTDRAW ddraw;
        DDCAPS hel_caps;
        short i;
        char _pad1[2];
    } l;
    if (this->display_modes || !this->device_count)
        return 0;
    this->DeviceThing();
    this->display_modes = new DisplayModes[this->device_count]();
    for ( l.i = 0; l.i < this->device_count; ++l.i) {
        this->display_modes[l.i].modes = NULL;
        this->display_modes[l.i].mode_count = 0;
    }

    if (this->ddraw) {
        this->ddraw->Release();
        this->ddraw = NULL;
    }

    for (this->sel_ddraw = 0; this->sel_ddraw < this->device_count;
            ++this->sel_ddraw) {
        l.res = DirectDrawCreate(
                this->ddraw_devices[this->sel_ddraw].guid, &l.ddraw, NULL);
        if (l.res) {
            g_Log.DxErr("Creating DirectDraw Interface", l.res);
            return 0;
        }
        l.ddraw->QueryInterface(IID_IDirectDraw4, (void **)&this->ddraw);
        l.ddraw->Release();
        l.driver_caps.dwSize = sizeof(DDCAPS);
        l.hel_caps.dwSize = sizeof(DDCAPS);
        this->ddraw->GetCaps(&l.driver_caps, &l.hel_caps);
        l.can_window = false;
        if (l.driver_caps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED)
            l.can_window = true;
        l.has_3d = false;
        if (l.driver_caps.dwCaps & DDCAPS_3D)
            l.has_3d = true;
        if ((this->flags & FLAGS_WINDOWED) == 0) {
            this->QueryMemory();
            g_VramBudget = this->lvram_total;
            if (!this->nlvram_total) {
                if (g_VramBudget > this->texmem_total) {
                    g_VramBudget -= this->texmem_total;
                } else if (g_VramBudget > 0x200000) {
                    g_VramBudget -= 0x200000;
                }
            }

            if (!this->ddraw_devices[this->sel_ddraw].guid) {
                l.surface_desc.dwSize = sizeof(DDSURFACEDESC2);
                l.res = this->ddraw->GetDisplayMode(&l.surface_desc);
                if (l.res)
                    g_Log.DxErr("Setting Display Mode", l.res);
                else
                    g_VramBudget +=
                        (l.surface_desc.ddpfPixelFormat.dwRGBBitCount *
                         l.surface_desc.dwHeight * l.surface_desc.dwWidth) /
                        8;
            }

            this->ddraw_devices[this->sel_ddraw].supports_mipmap =
                (l.driver_caps.ddsCaps.dwCaps & DDSCAPS_MIPMAP) != 0;
            g_FramebufferCount = 3;
            g_D3dDeviceCount = 0;
            l.res =
                this->ddraw->EnumDisplayModes(0, NULL, NULL, EnumDisplayModeCb);
            if (g_D3dDeviceCount == 0 || l.res) {
                g_Log.DxErr("EnumDisplayModes count", l.res);
                return 0;
            }
            this->display_modes[this->sel_ddraw].modes =
                new DisplayMode[g_D3dDeviceCount];
            l.res = this->ddraw->EnumDisplayModes(0, NULL, this, EnumDisplayModeCb);
            if (l.res) {
                g_Log.DxErr("EnumDisplayModes", l.res);
                return 0;
            }
        }
        l.v6 = 0;
        if (l.has_3d && (l.can_window || (this->flags & FLAGS_WINDOWED) == 0)) {
            l.v6 = this->QueryDevices();
            if (l.v6 == -1)
                return l.v6;
        }
        if (!l.v6)
            this->ddraw_devices[this->sel_ddraw].delete_flag = 1;

        if (this->ddraw) {
            this->ddraw->Release();
            this->ddraw = NULL;
        }
    }
    this->sel_ddraw = -1;
    return 1;
}

// FUNCTION: REDLINE 0x00450CF4
void D3dRenderer::PruneDevices(short remaining) {
    DevThing* replacement = new DevThing[remaining]();
    short replacement_idx = 0;
    for (short i = 0; i < this->device_count; ++i) {
        if (this->ddraw_devices[i].delete_flag) {
            for (short j = 0; j < this->d3d_devices[i].count; ++j) {
                if (this->d3d_devices[i].a[j].guid) {
                    delete this->d3d_devices[i].a[j].guid;
                    this->d3d_devices[i].a[j].guid = NULL;
                }
                if (this->d3d_devices[i].a[j].zbuffer_formats) {
                    delete[] this->d3d_devices[i].a[j].zbuffer_formats;
                    this->d3d_devices[i].a[j].zbuffer_formats = NULL;
                }
            }
            if (this->d3d_devices[i].a) {
                delete this->d3d_devices[i].a;
                this->d3d_devices[i].a = NULL;
            }
        } else {
            replacement[replacement_idx] = this->d3d_devices[i];
            replacement_idx++;
        }
    }
    if (this->d3d_devices) {
        delete this->d3d_devices;
        this->d3d_devices = NULL;
    }
    this->d3d_devices = replacement;
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
        if (this->ddraw_devices[i].guid) {
            delete this->ddraw_devices[i].guid;
            this->ddraw_devices[i].guid = NULL;
        }
    }
    if (this->ddraw_devices) {
        delete this->ddraw_devices;
        this->ddraw_devices = NULL;
    }
    this->device_count = 0;
}

// FUNCTION: REDLINE 0x00450B06
void D3dRenderer::ResetDevThing() {
    if (!this->d3d_devices) return;

    for (short i = 0; i < this->device_count; i++) {
        for (short j = 0; j < this->d3d_devices[i].count; j++) {
            if (this->d3d_devices[i].a[j].guid) {
                delete this->d3d_devices[i].a[j].guid;
                this->d3d_devices[i].a[j].guid = NULL;
            }
            if (this->d3d_devices[i].a[j].zbuffer_formats) {
                delete this->d3d_devices[i].a[j].zbuffer_formats;
                this->d3d_devices[i].a[j].zbuffer_formats = NULL;
            }
        }
        if (this->d3d_devices[i].a) {
            delete this->d3d_devices[i].a;
            this->d3d_devices[i].a = NULL;
        }
    }
    if (this->d3d_devices) {
        delete this->d3d_devices;
        this->d3d_devices = NULL;
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
            this->ddraw_devices = new Device[g_D3dDeviceCount]();
            this->ddraw_devices->guid = NULL;
            *this->ddraw_devices->display_name = NULL;
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
        this->ddraw_devices = new Device[g_D3dDeviceCount]();
        for (short i = 0; i < g_D3dDeviceCount; ++i) {
            this->ddraw_devices[i].guid = NULL;
            *this->ddraw_devices[i].display_name = NULL;
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
            sprintf(msg, "  %d: %s", j + 1, this->ddraw_devices[j].display_name);
            g_Log.Print(msg);
        }
        int init_res = this->DeviceInit();
        if (init_res <= 0)
            return init_res;
        short to_delete = 0;
        for (short k = 0; k < this->device_count; ++k) {
            if (this->ddraw_devices[k].delete_flag)
                ++to_delete;
        }
        if (to_delete) {
            short remaining = this->device_count - to_delete;
            if (!remaining) {
                return 0;
            }
            this->PruneDevices(remaining);
            DisplayModes* replacement_dismodes = new DisplayModes[remaining]();
            Device* replacement_dev = new Device[remaining]();
            short replacement_idx = 0;
            for (short k = 0; k < this->device_count; k++) {
                if (this->ddraw_devices[k].delete_flag) {
                    if (this->display_modes[k].modes) {
                        delete this->display_modes[k].modes;
                        this->display_modes[k].modes = NULL;
                    }
                    if (this->ddraw_devices[k].guid) {
                        delete this->ddraw_devices[k].guid;
                        this->ddraw_devices[k].guid = NULL;
                    }
                } else {
                    replacement_dismodes[replacement_idx] = this->display_modes[k];
                    replacement_dev[replacement_idx] = this->ddraw_devices[k];
                }
            }
            if (this->display_modes) {
                delete this->display_modes;
                this->display_modes = NULL;
            }
            this->display_modes = replacement_dismodes;
            if (this->ddraw_devices) {
                delete this->ddraw_devices;
                this->ddraw_devices = NULL;
            }
            this->ddraw_devices = replacement_dev;
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
    return this->ddraw_devices[idx].display_name;
}

// FUNCTION: REDLINE 0x0048e9e1
char* DeviceDisplayName(short idx) {
    return g_Direct3d->DeviceDisplayName(idx);
}

// FUNCTION: REDLINE 0x00456A8C
short D3dRenderer::DeviceByName(char* name) {
    for (short i = 0; i < this->device_count; ++i) {
        if (!strcmpi(name, this->ddraw_devices[i].name))
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
        if (!strcmpi(name, this->ddraw_devices[i].display_name))
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
    return this->ddraw_devices[idx].name;
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
        if  (flag & this->d3d_devices[dev].a->render_bit_depth)
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

// FUNCTION: REDLINE 0x004566E7
int D3dRenderer::SetWindowOrigin(int screen_x, int screen_y) {
    this->origin_screen_x = screen_x;
    this->origin_screen_y = screen_y;
    return 1;
}

// FUNCTION: REDLINE 0x0048ECE3
void SetWindowOrigin(int screen_x, int screen_y) {
    if (g_Direct3d)
        g_Direct3d->SetWindowOrigin(screen_x, screen_y);
}

// Doesn't really match...
// FUNCTION: REDLINE 0x0048ea59
short BestDevice() {
    short best_dev = 0;
    unsigned short score = g_Direct3d->d3d_devices->a[0].device_score;
    for (short i = 1; i < DeviceCount(); ++i) {
        if (g_Direct3d->d3d_devices[i].a->device_score > score) {
            score = g_Direct3d->d3d_devices[i].a->device_score;
            best_dev = i;
        }
    }
    return best_dev;
}

// FUNCTION: REDLINE 0x00451DD6
short D3dRenderer::D3dDeviceByName(short idx, const char* name) {
    if (idx >= 0 && idx < this->device_count) {
        for (short i = 0; i < this->d3d_devices[idx].count; ++i) {
            if (!strcmpi(name, this->d3d_devices[idx].a[i].name))
                return i;
        }
    }
    return -1;
}

// FUNCTION: REDLINE 0x0048EA2A
short D3dDeviceByName(short idx, const char* name) {
    return g_Direct3d->D3dDeviceByName(idx, name);
}

// FUNCTION: REDLINE 0x0048E988
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
    if (!g_RenderFlagUnk || (g_RenderFlagUnk == 1 && (a & 4) == 0)) {
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

// FUNCTION: REDLINE 0x00456573
int D3dRenderer::SetCooperativeLevel() {
    if (!this->ddraw)
        return 0;

    int res;
    if (this->fullscreen) {
        g_Log.Print("  Setting Cooperative Level: Exclusive FullScreen");
        res = this->ddraw->SetCooperativeLevel(this->window, DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FPUSETUP);
    } else {
        g_Log.Print("  Setting Cooperative Level: Normal");
        res = this->ddraw->SetCooperativeLevel(this->window, DDSCL_NORMAL | DDSCL_FPUSETUP);
    }
    if (res) {
        g_Log.DxErr(" ", res);
        return 0;
    }
    return 1;
}

// FUNCTION: REDLINE 0x00455AAF
int D3dRenderer::InitializeDirectDraw(short dev_idx) {
    struct Locals {
        char buf[128];
        DDCAPS helcaps;
        int res;
        LPDIRECTDRAW ddraw;
    } l;


    if (dev_idx < 0 || dev_idx > this->device_count)
        return 0;
    this->ReleaseDdraw();
    sprintf(l.buf, "Creating DirectDraw Interface: (%s)", this->ddraw_devices[dev_idx].name);
    g_Log.Print(l.buf);
    l.res = DirectDrawCreate(this->ddraw_devices[dev_idx].guid, &l.ddraw, NULL);
    if (l.res) {
        g_Log.DxErr(" ", l.res);
        return 0;
    }
    l.ddraw->QueryInterface(IID_IDirectDraw4, (LPVOID*)&this->ddraw);
    l.ddraw->Release();
    this->SetCooperativeLevel();

    memset(&this->caps, 0, sizeof(this->caps));
    this->caps.dwSize = sizeof(this->caps);
    memset(&l.helcaps, 0, sizeof(l.helcaps));
    l.helcaps.dwSize = sizeof(l.helcaps);
    this->ddraw->GetCaps(&this->caps, &l.helcaps);

    supports_gamma = 0;
    if ((this->caps.dwCaps2 & DDCAPS2_PRIMARYGAMMA) != 0) {
        if (this->fullscreen)
            supports_gamma = 1;
    }
    this->sel_ddraw = dev_idx;
    return 1;
}

// FUNCTION: REDLINE 0x0045132D
int D3dRenderer::InitializeD3d() {
    if (!this->ddraw)
        return 0;
    if (this->d3d_devices[this->sel_ddraw].count <= this->sel_d3d) {
        g_Log.Print("*Error: driver request out of range");
        return 0;
    }

    g_Log.Print("Creating Direct3D Interface");
    int res = this->ddraw->QueryInterface(IID_IDirect3D3, (LPVOID*)&this->d3d);
    if (res) {
        g_Log.DxErr("This App Requires DirectX 6 or greater", res);
        return 0;
    }

    D3DFINDDEVICERESULT result;
    D3DFINDDEVICESEARCH search;
    memset(&result, 0, sizeof(result));
    memset(&search, 0, sizeof(search));
    result.dwSize = sizeof(result);
    search.dwSize = sizeof(search);
    search.dwFlags = D3DFDS_GUID;

    memcpy(&search.guid, this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid, sizeof(search.guid));
    if (this->d3d->FindDevice(&search, &result) < 0) {
        g_Log.Print("Couldn't find the specified device");
        return 0;
    }

    if (result.ddHwDesc.dwFlags == 0) {
        this->some_flags = 2048;
    } else {
        this->some_flags = 0x4000;
    }
    return 1;
}

// FUNCTION: REDLINE 0x00456623
int D3dRenderer::SetDisplayMode(unsigned short width, unsigned short height, unsigned short bpp) {
    if (!this->ddraw || !this->fullscreen)
        return 0;
    if (!this->SupportsResolution(width, height, bpp)) {
        g_Log.Print("*Error: Invalid Display mode requested");
        return 0;
    }

    int res = this->ddraw->SetDisplayMode(width, height, bpp, 0, 0);
    if (res) {
        g_Log.DxErr("Setting Display Mode", res);
        return 0;
    }

    this->mode_width = width;
    this->mode_height = height;
    this->mode_bpp = bpp;
    return 1;
}

// FUNCTION: REDLINE 0x00456910
int D3dRenderer::GetSurfaceDesc(LPDDSURFACEDESC2 desc, LPDIRECTDRAWSURFACE4 surf) {
    memset(desc, 0, sizeof(DDSURFACEDESC2));
    desc->dwSize = sizeof(DDSURFACEDESC2);
    return surf->GetSurfaceDesc(desc);
}

// GLOBAL: REDLINE 0x00585AAC
const float NORM_WIDTH = 1.0f / 640.0f;
// GLOBAL: REDLINE 0x00585AA8
const float NORM_HEIGHT = 1.0f / 480.0f;

// FUNCTION: REDLINE 0x00455C6E
int D3dRenderer::CreateBackBuffers(HWND window, unsigned int width, unsigned int height, unsigned int bpp, int flags) {
    struct Locals {
        tagPALETTEENTRY palette2[256];
        int j;
        char msg[64];
        int res2;
        LPDIRECTDRAWCLIPPER clipper;
        DDSCAPS2 caps;
        tagPALETTEENTRY palette[256];
        int i;
        char buf[64];
        int res;
        DDSURFACEDESC2 surf_desc;
    } l;
    g_Log.Print("Setting display buffers");
    if (!this->ddraw)
        return 0;

    this->width_norm = (float)(int)width * NORM_WIDTH;
    this->height_norm = (float)(int)height * NORM_HEIGHT;

    this->backbuffer_count = ((flags & 1) != 0) + 1;
    if ((flags & 0x10) == 0)
        this->backbuffer_count = 0;
    memset(&l.surf_desc, 0, sizeof(l.surf_desc));
    l.surf_desc.dwSize = sizeof(l.surf_desc);

    if (this->fullscreen) {
        sprintf(l.buf,
                "  FullScreen setting display mode to: %dx%d x%d",
                width, height, bpp);
        g_Log.Print(l.buf);
        if (!this->SetDisplayMode(width, height, bpp)) {
            g_Log.Print("  *Failed");
            return 0;
        }
        sprintf(l.buf, "  Primary surface with %d back buffer(s)", this->backbuffer_count);
        g_Log.Print(l.buf);
        if ((flags & 0x10) != 0) {
            l.surf_desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
            l.surf_desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
            l.surf_desc.dwBackBufferCount = this->backbuffer_count;
        } else {
            l.surf_desc.dwFlags = DDSD_CAPS;
            l.surf_desc.ddsCaps.dwCaps = this->GetCaps() | DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE;
        }
        l.res = this->ddraw->CreateSurface(&l.surf_desc, &this->primary_surface, NULL);
        if (l.res) {
            g_Log.DxErr(" ", l.res);
            return 0;
        }
        if (this->mode_bpp == 8) {
            l.palette[0].peFlags = 64;
            for ( l.i = 1; l.i < 255; ++l.i)
                l.palette[l.i].peFlags = 1;
            l.palette[255].peFlags = 64;
            l.res = this->ddraw->CreatePalette(4, l.palette, &this->palette, NULL);
            if (l.res) {
                g_Log.DxErr("Creating palette", l.res);
                return 0;
            }
            this->primary_surface->SetPalette(this->palette);
        }
        if ((flags & 0x10) != 0) {
            l.caps.dwCaps = DDSCAPS_BACKBUFFER;
            l.res = this->primary_surface->GetAttachedSurface(&l.caps, &this->backbuffer); 
            if (l.res) {
                g_Log.DxErr("Get Back Buffer attatched surface", l.res);
                return 0;
            }
            this->render_surf = this->backbuffer;
            this->render_surf->AddRef();
        } else {
            this->render_surf = this->primary_surface;
            this->render_surf->AddRef();
        }
    } else {
        this->backbuffer_count = 1;
        sprintf(l.msg, "  Window size %dx%d", width, height);
        g_Log.Print(l.msg);
        l.surf_desc.dwFlags = DDSD_CAPS;
        l.surf_desc.ddsCaps.dwCaps = this->GetCaps() | DDSCAPS_PRIMARYSURFACE;
        if ((flags & 0x10) == 0)
            l.surf_desc.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
        g_Log.Print("  Creating Primary Surface");
        l.res2 = this->ddraw->CreateSurface(&l.surf_desc, &this->primary_surface, 0);
        if (l.res2) {
            g_Log.DxErr(" ", l.res2);
            return 0;
        }
        if (this->mode_bpp == 8) {
            for (l.j = 0; l.j < 10; ++l.j)
                l.palette2[l.j].peFlags = 64;
            for (l.j = 10; l.j < 246; ++l.j)
                l.palette2[l.j].peFlags = 1;
            for (l.j = 246; l.j < 256; ++l.j)
                l.palette2[l.j].peFlags = 64;
            l.res2 = this->ddraw->CreatePalette(4, l.palette2, &this->palette, NULL);
            if (l.res2) {
                g_Log.DxErr("Creating palette", l.res2);
                return 0;
            }
            this->primary_surface->SetPalette(this->palette);
            l.res2 = this->GetSurfaceDesc(&l.surf_desc, this->backbuffer);
            if (l.res2) {
                g_Log.DxErr("Getting surface description for back buffer", l.res2);
                return 0;
            }
            if ((l.surf_desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0)
                g_Log.Print("  Back buffer in Video Memory");
            else
                g_Log.Print("  Back buffer in System Memory");
        }

        g_Log.Print("  Creating Clipper Device");
        l.clipper = NULL;
        l.res2 = this->ddraw->CreateClipper(0, &l.clipper, NULL);
        if (l.res2) {
            g_Log.DxErr(" ", l.res2);
            return 0;
        }
        l.res2 = l.clipper->SetHWnd(0, window);
        if (l.res2) {
            g_Log.DxErr("Clipper HWnd", l.res2);
            return 0;
        }
        l.res2 = this->primary_surface->SetClipper(l.clipper);
        if (l.res2) {
            g_Log.DxErr("Attatching Clipper Device to Primary Buffer", l.res2);
            return 0;
        }
        if (l.clipper) {
            l.clipper->Release();
            l.clipper = NULL;
        }

        if((flags & 0x10) != 0) {
            l.surf_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
            l.surf_desc.dwWidth = width;
            l.surf_desc.dwHeight = height;
            l.surf_desc.ddsCaps.dwCaps = this->GetCaps() | DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN;
            if (this->GetCaps() == DDSCAPS_VIDEOMEMORY)
                g_Log.Print("  Creating Back Surface in Video Memory");
            else
                g_Log.Print("  Creating Back Surface in System Memory");
            l.res2 = this->ddraw->CreateSurface(&l.surf_desc, &this->backbuffer, NULL);
            if (l.res2) {
                g_Log.DxErr(" ", l.res2);
                return 0;
            }
            if (this->mode_bpp == 8)
                this->backbuffer->SetPalette(this->palette);
            this->render_surf = this->backbuffer;
            this->render_surf->AddRef();
        } else {
            this->render_surf = this->primary_surface;
            this->render_surf->AddRef();
        }
    }

    return 1;
}

// FUNCTION: REDLINE 0x00451AE4
int D3dRenderer::CreateZBuffer(unsigned short format_idx) {
    struct Locals {
        DDPIXELFORMAT * orig;
        char buf[256];
        DDSURFACEDESC2 surf_desc;
        int res;
        size_t unk;
    } l;
    l.orig = &this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].zbuffer_formats[format_idx];
    memset(&l.surf_desc, 0, sizeof(l.surf_desc));
    l.surf_desc.dwSize = sizeof(l.surf_desc);
    l.surf_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    l.surf_desc.ddsCaps.dwCaps = this->GetCaps() | DDSCAPS_ZBUFFER;
    memcpy(&l.surf_desc.ddpfPixelFormat, l.orig, l.unk = sizeof(l.surf_desc.ddpfPixelFormat));
    if (this->fullscreen) {
        l.surf_desc.dwHeight = this->mode_height;
        l.surf_desc.dwWidth = this->mode_width;
    } else {
        l.surf_desc.dwHeight = this->height;
        l.surf_desc.dwWidth = this->width;
    }
    sprintf(l.buf,
            "Creating Z-Buffer surface (%dx%d x%d) in %s memory",
            (int)l.surf_desc.dwWidth,
            (int)l.surf_desc.dwHeight,
            (int)l.orig->dwRGBBitCount,
            this->GetCaps() == DDSCAPS_VIDEOMEMORY ? "Video" : "System");
    g_Log.Print(l.buf);
    l.res = this->ddraw->CreateSurface(&l.surf_desc, &this->zbuffer, NULL);
    if (l.res) {
        g_Log.DxErr(" ", l.res);
        return 0;
    }
    l.res = this->render_surf->AddAttachedSurface(this->zbuffer);
    if (l.res) {
        if (this->zbuffer) {
            this->zbuffer->Release();
            this->zbuffer = NULL;
        }
        g_Log.DxErr("Attatching Z-Buffer to Back Buffer ", l.res);
        return 0;
    }
    return 1;
}

// FUNCTION: REDLINE 0x00451965
int D3dRenderer::SetupZBuffer() {
    short zbuf_format_count = this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].zbuffer_format_count;
    if (zbuf_format_count == 1) {
        return this->CreateZBuffer(0);
    }

    short format_opts[3];
    format_opts[0] = format_opts[1] = format_opts[2] = -1;

    short i;
    for (i = 0; i < zbuf_format_count; ++i) {
        if (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].zbuffer_formats[i].dwRGBBitCount == 32) {
            format_opts[0] = i;
        } else if (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].zbuffer_formats[i].dwRGBBitCount == 24) {
            format_opts[1] = i;
        } else {
            format_opts[2] = i;
        }
    }

    if (this->bpp == 16) {
        format_opts[0] = format_opts[1] = -1;
    }

    for (i = 0; i < 3; ++i) {
        if (format_opts[i] >= 0 && this->CreateZBuffer(format_opts[i])) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: REDLINE 0x00454c06
int D3dRenderer::BitCount(unsigned int in) {
    struct Locals {
        int count;
        int bit;
        int i;
    } l;
    l.bit = 1;
    l.count = 0;
    for ( l.i = 0; l.i < 32; ++l.i) {
        if ((in & l.bit) != 0)
            ++l.count;
        l.bit <<= 1;
    }
    return l.count;
}

// FUNCTION: REDLINE 0x0045408f
int D3dRenderer::AddTextureFormat(LPDDPIXELFORMAT fmt) {
    char buf[256];

    memset(&this->texfmts[this->texfmt_count], 0, sizeof(DDPIXELFORMAT));
    memcpy(&this->texfmts[this->texfmt_count], fmt, sizeof(DDPIXELFORMAT));

    if ((fmt->dwFlags & DDPF_RGB) != 0) {
        sprintf(buf, "  %d: bbp:%d  RGBA:%d%d%d%d ",
                this->texfmt_count + 1,
                (int)fmt->dwRGBBitCount,
                this->BitCount(fmt->dwRBitMask),
                this->BitCount(fmt->dwGBitMask),
                this->BitCount(fmt->dwBBitMask),
                this->BitCount(fmt->dwRGBAlphaBitMask)
               );
    } else if ((fmt->dwFlags & DDPF_LUMINANCE) != 0) {
        sprintf(buf, "  %d: bbp:%d LA:%d%d ",
                this->texfmt_count + 1,
                (int)fmt->dwRGBBitCount,
                this->BitCount(fmt->dwRBitMask),
                this->BitCount(fmt->dwRGBAlphaBitMask)
               );
    } else {
        sprintf(buf, "  %d: bbp:%d ", this->texfmt_count + 1, (int)fmt->dwRGBBitCount);
    }

    if ((fmt->dwFlags & DDPF_ALPHAPIXELS) != 0)
        strcat(buf, "ALPHA ");
    if ((fmt->dwFlags & DDPF_COMPRESSED) != 0)
        strcat(buf, "COMPRESSED ");
    if ((fmt->dwFlags & DDPF_PALETTEINDEXED1) != 0)
        strcat(buf, "PALETTEINDEXED1 ");
    if ((fmt->dwFlags & DDPF_PALETTEINDEXED2) != 0)
        strcat(buf, "PALETTEINDEXED2 ");
    if ((fmt->dwFlags & DDPF_PALETTEINDEXED4) != 0)
        strcat(buf, "PALETTEINDEXED4 ");
    if ((fmt->dwFlags & DDPF_PALETTEINDEXED8) != 0)
        strcat(buf, "PALETTEINDEXED8 ");
    if ((fmt->dwFlags & DDPF_PALETTEINDEXEDTO8) != 0)
        strcat(buf, "PALETTEINDEXEDTO8 ");
    if ((fmt->dwFlags & DDPF_RGBTOYUV) != 0)
        strcat(buf, "RGBTOYUV ");
    if ((fmt->dwFlags & DDPF_ALPHAPREMULT) != 0)
        strcat(buf, "ALPHAPREMULT ");
    if ((fmt->dwFlags & DDPF_BUMPLUMINANCE) != 0)
        strcat(buf, "BUMPLUMINANCE ");
    if ((fmt->dwFlags & DDPF_BUMPDUDV) != 0)
        strcat(buf, "BUMPDUDV ");
    if ((fmt->dwFlags & DDPF_FOURCC) != 0)
        strcat(buf, "FOURCC ");
    if ((fmt->dwFlags & DDPF_LUMINANCE) != 0)
        strcat(buf, "LUMINANCE ");
    if ((fmt->dwFlags & DDPF_STENCILBUFFER) != 0)
        strcat(buf, "STENCILBUFFER ");
    if ((fmt->dwFlags & DDPF_YUV) != 0)
        strcat(buf, "YUV ");
    if ((fmt->dwFlags & DDPF_ZBUFFER) != 0)
        strcat(buf, "ZBUFFER ");
    if ((fmt->dwFlags & DDPF_ZPIXELS) != 0)
        strcat(buf, "ZPIXELS ");
    g_Log.Print(buf);

    if ((fmt->dwFlags & DDPF_PALETTEINDEXED8) != 0) {
        this->texfmts[this->texfmt_count].paletted = 1;
        this->texfmts[this->texfmt_count].bpp = 8;
    } else if ((fmt->dwFlags & DDPF_PALETTEINDEXED4) != 0) {
        this->texfmts[this->texfmt_count].paletted = 1;    
        this->texfmts[this->texfmt_count].bpp = 4;
    } else {
        this->texfmts[this->texfmt_count].paletted = 0;
        this->texfmts[this->texfmt_count].bpp = fmt->dwRGBBitCount;

        this->texfmts[this->texfmt_count].red_bitmask = fmt->dwRBitMask;
        this->texfmts[this->texfmt_count].green_bitmask = fmt->dwGBitMask;
        this->texfmts[this->texfmt_count].blue_bitmask = fmt->dwBBitMask;
        this->texfmts[this->texfmt_count].luminance_bitmask = fmt->dwRBitMask;

        this->texfmts[this->texfmt_count].red_bits = this->BitCount(fmt->dwRBitMask);
        this->texfmts[this->texfmt_count].green_bits = this->BitCount(fmt->dwGBitMask);
        this->texfmts[this->texfmt_count].blue_bits = this->BitCount(fmt->dwBBitMask);
        this->texfmts[this->texfmt_count].luminance_bits = this->BitCount(fmt->dwRBitMask);

        if((fmt->dwFlags & DDPF_LUMINANCE) != 0) {
            this->texfmts[this->texfmt_count].has_luminance = true;
        } else {
            this->texfmts[this->texfmt_count].has_luminance = false;
        }
        if ((fmt->dwFlags & DDPF_ALPHAPIXELS) != 0) {
            this->texfmts[this->texfmt_count].has_alpha = 1;
            this->texfmts[this->texfmt_count].alpha_bitmask = fmt->dwRGBAlphaBitMask;
            this->texfmts[this->texfmt_count].alpha_bits = this->BitCount(fmt->dwRGBAlphaBitMask);
        } else {
            this->texfmts[this->texfmt_count].has_alpha = 0;
            this->texfmts[this->texfmt_count].alpha_bitmask = 0;
            this->texfmts[this->texfmt_count].alpha_bits = 0;
        }
    }

    if (this->texfmt_count >= 15)
        return 0;

    ++this->texfmt_count;
    return 1;
}

// FUNCTION: REDLINE 0x004556E9
HRESULT CALLBACK D3dEnumTextureFormatsCb(LPDDPIXELFORMAT fmt, void* userdata) {
    return ((D3dRenderer*)userdata)->AddTextureFormat(fmt);
}

// FUNCTION: REDLINE 0x00454853
void D3dRenderer::ChooseTextureFormats() {
    TextureFormat* fmt = NULL;
    TextureFormat* min_alpha = NULL;
    unsigned int i;

    // Select an 8bpp texture if present
    for (i = 0; i < this->texfmt_count; ++i) {
        if (this->texfmts[i].paletted == 1 && this->texfmts[i].bpp == 8) {
            fmt = &this->texfmts[i];
            break;
        }
    }
    this->fmt_8bpp_palette = fmt;

    fmt = NULL;
    short highest_colorbits = 0;
    for (i = 0; i < this->texfmt_count; ++i) {
        if (this->texfmts[i].bpp == 16
                && !this->texfmts[i].has_alpha
                && !this->texfmts[i].has_luminance) {
            short color_bits = (short) (this->texfmts[i].blue_bits + this->texfmts[i].green_bits + this->texfmts[i].red_bits);
            if (color_bits > highest_colorbits) {
                highest_colorbits = color_bits;
                fmt = &this->texfmts[i];
            }
        }
    }
    this->fmt_16bpp = fmt;

    min_alpha = NULL;
    fmt = NULL;
    bool first = true;
    unsigned short alpha_bits_max;
    unsigned short alpha_bits_min;

    for (i = 0; i < this->texfmt_count; ++i) {
        if (this->texfmts[i].bpp == 16 && this->texfmts[i].has_alpha && !this->texfmts[i].has_luminance) {
            if (first) {
                first = false;
                min_alpha = &this->texfmts[i];
                fmt = min_alpha;
                alpha_bits_max = this->texfmts[i].alpha_bits;
                alpha_bits_min = alpha_bits_max;
            } else {
                if (this->texfmts[i].alpha_bits > alpha_bits_max) {
                    alpha_bits_max = this->texfmts[i].alpha_bits;
                    fmt = &this->texfmts[i];
                }
                if (this->texfmts[i].alpha_bits < alpha_bits_min) {
                    alpha_bits_min = this->texfmts[i].alpha_bits;
                    min_alpha = &this->texfmts[i];
                }
            }
        }
    }
    this->fmt_16bpp_maxalpha = fmt;
    this->fmt_16bpp_minalpha = min_alpha;

    fmt = NULL;
    for (i = 0; i < this->texfmt_count; ++i) {
        if (this->texfmts[i].bpp == 32 && !this->texfmts[i].has_alpha) {
            fmt = &this->texfmts[i];
            break;
        }
    }
    this->fmt_32bpp_noalpha = fmt;

    fmt = NULL;
    for (i = 0; i < this->texfmt_count; ++i) {
        if (this->texfmts[i].bpp == 32 && this->texfmts[i].has_alpha) {
            fmt = &this->texfmts[i];
            break;
        }
    }
    this->fmt_32bpp_alpha = fmt;
}

// FUNCTION: REDLINE 0x004547FA
void D3dRenderer::ClearPreferredTextureFormats() {
    this->fmt_8bpp_palette = NULL;
    this->fmt_16bpp = NULL;
    this->fmt_16bpp_minalpha = NULL;
    this->fmt_16bpp_maxalpha = NULL;
    this->fmt_32bpp_noalpha = NULL;
    this->fmt_32bpp_alpha = NULL;
}

// FUNCTION: REDLINE 0x004514DD
int D3dRenderer::CreateD3dDevice(short d3d_idx) {
    if (!this->render_surf || d3d_idx < 0 || d3d_idx >= this->d3d_devices[this->sel_ddraw].count)
        return 0;
    this->ClearPreferredTextureFormats();
    this->texfmt_count = 0;
    char buf[128];
    sprintf(buf, "Creating D3D Device: (%s)", this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].name);
    g_Log.Print(buf);

    int res = this->d3d->CreateDevice(
        (REFCLSID)*this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].guid,
        this->render_surf,
        &this->d3d_device,
        0
    );
    if (res) {
        g_Log.DxErr("Creating D3D Device ", res);
        return 0;
    }
    
    g_Log.Print("Enumerating Texture Formats");
    res = this->d3d_device->EnumTextureFormats(D3dEnumTextureFormatsCb, this);
    if (res) {
        g_Log.DxErr(" ", res);
        return 0;
    }
    
    D3DDEVICEDESC hw;
    D3DDEVICEDESC hel;
    memset(&hw, 0, sizeof(hw));
    memset(&hel, 0, sizeof(hel));
    hw.dwSize = sizeof(hw);
    hel.dwSize = sizeof(hel);
    this->d3d_device->GetCaps(&hw, &hel);
    this->ChooseTextureFormats();
    this->sel_d3d = d3d_idx;
    return 1;
}

// FUNCTION: REDLINE 0x004524E5
int D3dRenderer::SetViewportMaterial(unsigned char r, unsigned char g, unsigned char b) {
    if (!this->d3d || !this->viewport_mat)
        return 0;

    D3DMATERIAL mat;
    memset(&mat, 0, sizeof(mat));
    mat.dwSize = sizeof(mat);
    mat.diffuse.r = r / 255.0;
    mat.diffuse.g = g / 255.0;
    mat.diffuse.b = b / 255.0;
    mat.diffuse.a = 1.0;
    mat.ambient.r = mat.diffuse.r;
    mat.ambient.g = mat.diffuse.g;
    mat.ambient.b = mat.diffuse.b;
    mat.ambient.a = 1.0;

    mat.emissive.r = 0;
    mat.emissive.g = 0;
    mat.emissive.b = 0;

    mat.specular.r = 0;
    mat.specular.g = 0;
    mat.specular.b = 0;

    mat.power = 0;
    mat.dwRampSize = 0;
    mat.hTexture = 0;

    int res = this->viewport_mat->SetMaterial(&mat);
    if (res) {
        g_Log.D3dErr("Setting Material", res);
        return 0;
    }
    return 1;
}

// FUNCTION: REDLINE 0x00452A58
void D3dRenderer::SetRenderState() {
    if (!this->d3d_device)
        return;

    this->d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, 2);
    this->d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
    this->d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, 4);

    this->d3d_device->SetRenderState(D3DRENDERSTATE_SHADEMODE, 2);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, 1);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_WRAPU, 0);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_WRAPV, 0);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_ZENABLE, 1);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_ZFUNC, 4);

    int filter = 1;
    if (this->GetCaps() == 0x4000)
        filter = 2;
    this->d3d_device->SetTextureStageState(0, D3DTSS_MAGFILTER, filter);

    this->d3d_device->SetTextureStageState(0, D3DTSS_MINFILTER, filter);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_FILLMODE, 3);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_CULLMODE, 3);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 0);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, 0);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
    this->d3d_device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, 0);

    if (this->d3d_devices[this->sel_ddraw].a[this->sel_d3d].subpixel)
        this->d3d_device->SetRenderState(D3DRENDERSTATE_SUBPIXEL, 1);
}

// FUNCTION: REDLINE 0x004522DD
int D3dRenderer::CreateViewportMaterials() {
    memset(&this->proj_matrix, 0, sizeof(this->proj_matrix));
    this->proj_matrix._11 = 2.0;
    this->proj_matrix._22 = 2.0;
    this->proj_matrix._33 = 1.0;
    this->proj_matrix._34 = 1.0;
    this->proj_matrix._43 = -1.0;

    this->view_matrix = this->default_matrix;
    this->world_matrix = this->default_matrix;

    this->d3d_device->SetTransform(D3DTRANSFORMSTATE_VIEW, &this->view_matrix);
    this->d3d_device->SetTransform(D3DTRANSFORMSTATE_WORLD, &this->world_matrix);
    this->d3d_device->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &this->proj_matrix);
    
    if (this->viewport_mat) {
        this->viewport_mat->Release();
        this->viewport_mat = NULL;
    }

    int res = this->d3d->CreateMaterial(&this->viewport_mat, NULL);
    if (res) {
        g_Log.D3dErr("Creating Background Material", res);
        return 0;
    }

    if (this->viewport_mat->GetHandle(this->d3d_device, &this->viewport_mat_handle)) {
        g_Log.D3dErr("Getting Material Handle", res);
        return 0;
    }
    
    if (!this->SetViewportMaterial(0, 0, 0)) {
        return 0;
    }
    this->viewport->SetBackground(this->viewport_mat_handle);
    this->SetRenderState();
    return 1;
}

// FUNCTION: REDLINE 0x0048F137
short BeginScene() {
    return g_Direct3d->BeginScene();
}

// FUNCTION: REDLINE 0x0045313E
short D3dRenderer::BeginScene() {
    return !this->d3d_device
        || !this->d3d_device->BeginScene();
}

// FUNCTION: REDLINE 0x0048F147
short EndScene() {
    return g_Direct3d->EndScene();
}

// FUNCTION: REDLINE 0x00453181
short D3dRenderer::EndScene() {
    return !this->d3d_device
        || !this->d3d_device->EndScene();
}

// FUNCTION: REDLINE 0x0048F046
void ClearViewport(short mode) {
    g_Direct3d->ClearViewport(mode);
}

// FUNCTION: REDLINE 0x0045268A
void D3dRenderer::ClearViewport(short mode) {
    struct Locals {
        int height;
        int width;
        D3DRECT rect;
        int res;
    } l;
    if (!this->viewport)
        return;

    if (this->fullscreen) {
        l.height = this->mode_height;
        l.width = this->mode_width;
    } else {
        l.height = this->height;
        l.width = this->width;
    }
    l.rect.x1 = l.rect.y1 = 0;
    l.rect.x2 = l.width;
    l.rect.y2 = l.height;

    switch (mode) {
        case 0:
            l.res = this->viewport->Clear(1, &l.rect, D3DCLEAR_ZBUFFER);
            break;
        case 1:
            l.res = this->viewport->Clear(1, &l.rect, D3DCLEAR_TARGET);
            break;
        default:
            l.res = this->viewport->Clear(1, &l.rect, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET);
            break;
    }
    if (l.res) {
        g_Log.D3dErr("Clear Viewport", l.res);
    }
}

// FUNCTION: REDLINE 0x00452611
void D3dRenderer::ReleaseViewport() {
    if (!this->viewport)
        return;
    if (this->viewport_mat) {
        this->viewport_mat->Release();
        this->viewport_mat = NULL;
    }
    if (this->viewport) {
        this->viewport->Release();
        this->viewport = NULL;
    }
}

// FUNCTION: REDLINE 0x00452006
int D3dRenderer::CreateViewport(int flags) {
    g_Log.Print("Creating Viewport");
    this->ReleaseViewport();
    int res = this->d3d->CreateViewport(&this->viewport, NULL);
    if (res) {
        g_Log.D3dErr(" ", res);
        return 0;
    }

    res = this->d3d_device->AddViewport(this->viewport);
    if (res) {
        g_Log.D3dErr(" ", res);
        return 0;
    }

    int height, width;
    if (this->fullscreen) {
        height = this->mode_height;
        width = this->mode_width;
    } else {
        height = this->height;
        width = this->width;
    }
    float aspect = (float)height / (float)width;

    memset(&this->viewport_desc, 0, sizeof(this->viewport_desc));
    if ((flags & 0x8) != 0) {
        this->viewport_desc.dwSize = sizeof(this->viewport_desc);
        this->viewport_desc.dwX = 0;
        this->viewport_desc.dwY = 0;
        this->viewport_desc.dwWidth = width;
        this->viewport_desc.dwHeight = height;
        this->viewport_desc.dvClipX = -1.0;

        this->viewport_desc.dvClipY = aspect;
        this->viewport_desc.dvClipWidth = 2.0;
        // TODO: 2.0 float is a constant
        this->viewport_desc.dvClipHeight = 2.0 * aspect;

        this->viewport_desc.dvMinZ = 0.0;
        this->viewport_desc.dvMaxZ = 1.0;
    } else {
        this->viewport_desc.dwSize = sizeof(this->viewport_desc);
        this->viewport_desc.dwX = 0;
        this->viewport_desc.dwY = 0;
        this->viewport_desc.dwWidth = width;
        this->viewport_desc.dwHeight = height;
        this->viewport_desc.dvClipX = -1.0;

        this->viewport_desc.dvClipY = 1.0;
        this->viewport_desc.dvClipWidth = 2.0;
        this->viewport_desc.dvClipHeight = 2.0;

        this->viewport_desc.dvMinZ = 0.0;
        this->viewport_desc.dvMaxZ = 1.0;
    }

    res = this->viewport->SetViewport2(&this->viewport_desc);
    if (res) {
        g_Log.D3dErr("Setting Viewport", res);
        return 0;
    }

    res = this->d3d_device->SetCurrentViewport(this->viewport);
    if (res) {
        g_Log.D3dErr("Setting Current Viewport ", res);
        return 0;
    }

    this->CreateViewportMaterials();
    this->ClearViewport(2);
    return 1;
}

// FUNCTION: REDLINE 0x0045185C
void D3dRenderer::ReleaseD3d() {
    if (this->palette) {
        this->palette->Release();
        this->palette = NULL;
    }
    if (!this->fullscreen) {
        if (this->backbuffer) {
            this->backbuffer->Release();
            this->backbuffer = NULL;
        }
    } else {
        this->backbuffer = NULL;
    }
    if (this->render_surf) {
        this->render_surf->Release();
        this->render_surf = NULL;
    }
    if (this->zbuffer) {
        this->zbuffer->Release();
        this->zbuffer = NULL;
    }
    if (this->primary_surface) {
        this->primary_surface->Release();
        this->primary_surface = NULL;
    }
}

// FUNCTION: REDLINE 0x004517C2
void D3dRenderer::DeinitD3d() {
    this->ReleaseViewport();
    if (this->d3d_device) {
        if (this->d3d_device->Release())
            g_Log.Print("D3D Device ref count not 0");
        this->d3d_device = NULL;
    }
    this->ReleaseD3d();
    if (this->d3d) {
        this->d3d->Release();
        this->d3d = NULL;
    }
}

// FUNCTION: REDLINE 0x00491380
void GuidToString(GUID* guid, char* buf) {
    if (guid) {
        sprintf(
            buf,
            "%8x-%4x-%4x-%2x%2x-%2x%2x%2x%2x%2x%2x",
            (unsigned int)guid->Data1,
            guid->Data2,
            guid->Data3,
            guid->Data4[0],
            guid->Data4[1],
            guid->Data4[2],
            guid->Data4[3],
            guid->Data4[4],
            guid->Data4[5],
            guid->Data4[6],
            guid->Data4[7]);
    } else {
        strcpy(buf, "NULL");
    }
}

// FUNCTION: REDLINE 0x00451729
void D3dRenderer::ReleaseDdraw() {
    this->DeinitD3d();
    if (this->ddraw) {
        int res = this->ddraw->Release();
        this->ddraw = NULL;
        if (res) {
            char buf[128];
            sprintf(buf, "*Error: DD Device still has %d reference(s)", res);
            g_Log.Print(buf);
        }
        this->sel_ddraw = -1;
    }
    this->sel_d3d = -1;
}

// FUNCTION: REDLINE 0x00450F51
int D3dRenderer::InitializeDevice(HWND window, short dev_idx, short d3d_idx, unsigned short width, unsigned short height, unsigned short bpp, int flags) {
    char buf[128];
    if (width > 0x1000 || height > 0x1000) {
        sprintf(buf, "*Error: buffer size out of range (%dx%d x%d)", width, height, bpp);
        g_Log.Print(buf);
        return 0;
    }
    this->fullscreen = (flags & 2) != 0;
    this->window = window;
    this->width = width;
    this->height = height;
    this->bpp = bpp;

    if (this->sel_ddraw != dev_idx) {
        // TODO: Some method on UnkRenderer
        this->ReleaseDdraw();
        if (!this->InitializeDirectDraw(dev_idx))
            return -1;
    }
    if (this->sel_d3d != d3d_idx) {
        this->sel_d3d = d3d_idx;
        this->ReleaseD3d();
        if (!this->InitializeD3d()) {
            this->sel_d3d = -1;
            return -1;
        }
        if (!this->CreateBackBuffers(window, width, height, bpp, flags)) {
            this->sel_d3d = -1;
            return -1;
        }
        if ((flags & 0x4) != 0 && !this->SetupZBuffer()) {
            this->ReleaseDdraw();
            this->sel_d3d = -1;
            return -1;
        }
        if (!this->CreateD3dDevice(d3d_idx)) {
            this->sel_d3d = -1;
            return -1;
        }
        if (!this->CreateViewport(flags)) {
            this->sel_d3d = -1;
            return -1;
        }
    } else {
        this->ReleaseD3d();
        if (!this->CreateBackBuffers(window, width, height, bpp, flags))
            return -1;
        if ((flags & 0x4) != 0 && !this->SetupZBuffer())
            return -1;

        int res = this->d3d_device->SetRenderTarget(this->render_surf, 0);
        if (res) {
            g_Log.DxErr("SetRenderTarget ", res);
            return 0;
        }
        this->CreateViewport(flags);
    }
    
    this->QueryMemory();
    char guid[64];
    GuidToString(this->ddraw_devices[dev_idx].guid, guid);
    WriteRegistry("Device Name", this->ddraw_devices[dev_idx].name); 
    WriteRegistry("Device GUID", guid);
    char dispname[128];
    strcpy(dispname, this->ddraw_devices[dev_idx].display_name);
    char* paren = strrchr(dispname, '(');
    if (paren)
        *(paren - 1) = 0;
    WriteRegistry("Device Name Full", dispname);
    return 1;
}

// FUNCTION: REDLINE 0x0048EE20
void RenderText(short x, short y, const char* str) {
    g_Direct3d->RenderText(x, y, str);
}

// FUNCTION: REDLINE 0x00457AA6
void D3dRenderer::RenderText(short x, short y, const char* str) {
    HDC hdc;
    if (this->render_surf->GetDC(&hdc))
        return;

    SetBkColor(hdc, 0);
    SetTextColor(hdc, 0xFF);
    int len = strlen(str);
    TextOutA(hdc, x, y, str, len);
    this->render_surf->ReleaseDC(hdc);
}

// FUNCTION: REDLINE 0x0045715A
bool D3dRenderer::RestoreSurfaces() {
    bool none_lost = true;
    if (this->primary_surface && this->primary_surface->IsLost()) {
        this->primary_surface->Restore();
        none_lost = false;
    }

    if (this->backbuffer && this->backbuffer->IsLost()) {
        this->backbuffer->Restore();
        none_lost = false;
    }

    if (this->zbuffer && this->zbuffer->IsLost()) {
        this->zbuffer->Restore();
        none_lost = false;
    }

    this->ddraw->RestoreAllSurfaces();
    return none_lost;
}

// FUNCTION: REDLINE 0x0048EDEC
int FlipDisplay() {
    return g_Direct3d->FlipDisplay();
}

// FUNCTION: REDLINE 0x00456FD6
int D3dRenderer::FlipDisplay() {
    if (!this->primary_surface)
        return 1;
    bool v9 = 1;
    if (!this->backbuffer) {
        return this->RestoreSurfaces();
    }
    // TODO: Performance counter call
    if (this->fullscreen) {
        v9 = this->RestoreSurfaces();
        while (this->primary_surface->Flip(NULL, 1) == DDERR_WASSTILLDRAWING) {};
    } else {
        int origin_x, origin_y;
        origin_x = origin_y = 0;
        int width = this->width;
        int height = this->height;

        tagRECT dst;
        dst.left = dst.top = 0;
        dst.right = this->width;
        dst.bottom = this->height;

        tagRECT src;
        src.left = src.top = 0;
        src.right = this->width;
        src.bottom = this->height;

        dst.left = this->origin_screen_x;
        dst.right += this->origin_screen_x;
        dst.bottom += this->origin_screen_y;
        dst.top = this->origin_screen_y;

        int res = this->primary_surface->Blt(
                &dst,
                this->backbuffer,
                &src,
                0x100000,
                0);

        if (res == DDERR_SURFACELOST) {
            return this->RestoreSurfaces();
        }

        if (res) {
            g_Log.DxErr("Blt Back -> Front", res);
            return v9;
        }
    }

    return v9;
}

// GLOBAL: REDLINE 0x005A8F20
short g_SupportsBlend;
// GLOBAL: REDLINE 0x005A8F22
short g_SupportsBlend2;
// GLOBAL: REDLINE 0x005A8F24
short g_SupportsDither;
// GLOBAL: REDLINE 0x005A8F26
short g_SupportsWbuffer;
// GLOBAL: REDLINE 0x005A8F28
short g_SupportsFogTable;

// STUB: REDLINE 0x0048f496
void SetCapGlobals() {
    g_SupportsBlend = g_Direct3d->d3d_devices[g_Direct3d->sel_ddraw].a[g_Direct3d->sel_d3d].supports_blend;
    g_SupportsBlend2 = g_Direct3d->d3d_devices[g_Direct3d->sel_ddraw].a[g_Direct3d->sel_d3d].supports_blend2;
    g_SupportsDither = g_Direct3d->d3d_devices[g_Direct3d->sel_ddraw].a[g_Direct3d->sel_d3d].dither;
    g_SupportsWbuffer = g_Direct3d->d3d_devices[g_Direct3d->sel_ddraw].a[g_Direct3d->sel_d3d].wbuffer;
    g_SupportsFogTable = g_Direct3d->d3d_devices[g_Direct3d->sel_ddraw].a[g_Direct3d->sel_d3d].fogtable;
    // word_5A8F2A = -1;
    // word_5A8F2C = -1;
    // word_5A8F2E = -1;
    // word_5A8F30 = -1;
    // word_5A8F3A = -1;
    // word_5A8F3C = -1;
    // word_5A8F38 = -1;
    // word_5A8F3E = -1;
    // dword_5A8F40 = 0;
    // dword_5A8F44 = 0;
    // dword_5A8F48 = 0;
    // word_5A8F32 = -1;
    // word_5A8F34 = -1;
    // word_5A8F50 = -1;
    // word_5A8F4C = 1;
    // word_5A8F4E = 1;
    // word_5A8F52 = 1;
}
