#pragma once

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>


struct Device {
    GUID* guid;
    char name[96];
    char display_name[128];
    bool supports_mipmap;
    bool delete_flag;
};

struct DisplayMode {
    unsigned short width;
    unsigned short height;
    unsigned short bpp;
    int unk;
};

struct DisplayModes {
    short mode_count;
    DisplayMode* modes;
};

struct DeviceMeta {
    char name[64]; // Length is a guess
    char pad[32];
    D3DDEVICEDESC desc;
    GUID* guid;
    DDPIXELFORMAT* zbuffer_formats;

    // How good of a choice this device is
    unsigned short device_score;

    short unk2;
    short supports_perspective;
    short has_z_depth;
    short tmp; // unk
    short can_render_after_flip;
    short texture_nlvram;
    short texture_vram; // 14
    short aa_edges;
    short aa_sortdependent;
    short aa_sortindependent;
    short fogtable;
    short fogvertex;
    short fogrange;
    short dither; // 28
    short tex_squareonly;
    short tex_pow2;
    short subpixel;
    short zbuf_less_hsr; // 36
    short wbuffer;

    short has_linearmiplinear; // 40
    short zbuffer_format_count;
    short supports_blend;
    short supports_blend2;
    

    short max_blend_stages; // 48
                            //
    int tex_blend_caps;
    int src_blend_caps;
    int dst_blend_caps;

    int z_bit_depth; // 64
    int render_bit_depth;
};

struct DevThing {
    DeviceMeta* a; // Pointer to array of 428-byte structs
    unsigned short count;
};

#define FLAGS_WINDOWED 1

class Renderer {
    public:
    virtual void Reset() = 0;
    virtual void DeviceThing() = 0;
    virtual void VtablePad() = 0;
    virtual int QueryDevices() = 0;
    virtual void PruneDevices(short remaining) = 0;

    Renderer();

    Device* devices;
    short device_count;
    short devices_inited;
    short unk;
    unsigned int flags;
    DisplayModes* display_modes;
    char pad1[52];

    unsigned int vram_total;
    unsigned int vram_free;
    unsigned int lvram_total;
    unsigned int lvram_free;
    unsigned int nlvram_total;
    unsigned int nlvram_free;
    unsigned int texmem_total;
    unsigned int texmem_free;

    // TODO
    char pad1_1[12];
    LPDIRECTDRAW4 ddraw;
    char pad2[412];
};

class D3dRenderer : public Renderer {
    public:
    virtual void Reset(); // vtable[0]
    virtual void DeviceThing(); // vtable[1]
    virtual void VtablePad(); // vtable[2]
    virtual int QueryDevices(); // vtable[3]
    virtual void PruneDevices(short remaining); // vtable[4]
    
    D3dRenderer();

    DevThing* device_thing;
    int zbuffer_fmt_count;
    char pad3[1376]; // was 1704
    LPDIRECT3D3 d3d;
    char pad4[320];

    int Initialize(int flags);
    void QueryMemory();
    int DeviceInit();
    int AddD3dDevice(GUID* guid, char* name, LPD3DDEVICEDESC desc, short unk);
    int AddMode(short width, short height, short unk);
    int AddDevice(GUID* guid, const char* device_name, const char* device_name_full);
    int AddZBufferFormat(LPDDPIXELFORMAT fmt);

    int SupportsResolution(unsigned short width, unsigned short height, unsigned short bpp);

    void ResetDevThing();
    void ResetDevices();

    char* DeviceDisplayName(short idx);
    short DeviceByName(char* name);
    short DeviceByDisplayName(char* name);

    char* DeviceName(short idx);

    short DeviceModeCount(short idx);
    short DeviceModeResolution(short idx, short mode, short* width, short* height, short* bpp);
    short SupportsBitDepth(short dev, short bpp);
};

int DeviceCount();
char* DeviceDisplayName(short idx);
short DeviceByDisplayName(char* name);
short DeviceByName(char* name);
char* DeviceName(short idx);
short DeviceModeCount(short idx);
short DeviceModeResolution(short idx, short mode, short* width, short* height, short* bpp);
short SupportsBitDepth(short dev, short bpp);

short FormatResolution(short dev, short mode, char* out);
short ResolutionToMode(short dev, short width, short height, short bpp);

short BestDevice();
