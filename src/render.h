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
    virtual int QueryDevices() = 0;
    virtual void ReleaseDdraw() = 0;
    virtual void PruneDevices(short remaining) = 0;
    virtual int InitializeDevice(HWND window, short dev_idx, short d3d_idx, unsigned short width, unsigned short height, unsigned short bpp, int flags) = 0;
    virtual int GetCaps() = 0;

    Renderer();

    Device* ddraw_devices;
    short device_count;
    short sel_ddraw;
    short sel_d3d;
    unsigned int flags;
    DisplayModes* display_modes;
    unsigned short mode_width;
    unsigned short mode_height;
    unsigned short mode_bpp;
    char pad1[18];

    int fullscreen;
    unsigned short width;
    unsigned short height;
    unsigned short bpp;

    char pad_1_0[10];

    int origin_screen_x;
    int origin_screen_y;

    unsigned int vram_total;
    unsigned int vram_free;
    unsigned int lvram_total;
    unsigned int lvram_free;
    unsigned int nlvram_total;
    unsigned int nlvram_free;
    unsigned int texmem_total;
    unsigned int texmem_free;

    // TODO
    short supports_gamma;
    float width_norm;
    float height_norm;
    LPDIRECTDRAW4 ddraw;
    LPDIRECTDRAWSURFACE4 backbuffer;
    LPDIRECTDRAWSURFACE4 render_surf; // unsure if correct usage
    LPDIRECTDRAWSURFACE4 primary_surface;
    LPDIRECTDRAWSURFACE4 zbuffer;
    LPDIRECTDRAWPALETTE palette;

    HWND window;
    short backbuffer_count;
    char pad2_0[6];
    DDCAPS caps;
};

struct TextureFormat {
    DDPIXELFORMAT fmt;
    int paletted;
    short has_alpha;
    short has_luminance;
    int bpp;
    int red_bitmask;
    int green_bitmask;
    int blue_bitmask;
    int alpha_bitmask;
    int luminance_bitmask;
    int red_bits;
    int green_bits;
    int blue_bits;
    int alpha_bits;
    int luminance_bits;
};

class D3dRenderer : public Renderer {
    public:
    virtual void Reset(); // vtable[0]
    virtual void DeviceThing(); // vtable[1]
    virtual int QueryDevices(); // vtable[2]
    virtual void ReleaseDdraw(); // vtable[3]
    virtual void PruneDevices(short remaining); // vtable[4]
    virtual int InitializeDevice(HWND window, short dev_idx, short d3d_idx, unsigned short width, unsigned short height, unsigned short bpp, int flags); // vtable[5]
    virtual int GetCaps(); // vtable[6]
    
    D3dRenderer();

    DevThing* d3d_devices;
    int zbuffer_fmt_count;
    // char pad3[4];
    unsigned int texfmt_count;
    TextureFormat texfmts[16];
    TextureFormat* fmt_8bpp_palette;
    TextureFormat* fmt_16bpp;
    TextureFormat* fmt_16bpp_minalpha;
    TextureFormat* fmt_16bpp_maxalpha;
    TextureFormat* fmt_32bpp_noalpha;
    TextureFormat* fmt_32bpp_alpha;
    LPDIRECT3DDEVICE3 d3d_device;
    LPDIRECT3D3 d3d;
    D3DVIEWPORT2 viewport_desc;
    LPDIRECT3DVIEWPORT3 viewport;
    D3DMATRIX proj_matrix;
    D3DMATRIX view_matrix;
    D3DMATRIX world_matrix;
    D3DMATRIX default_matrix;
    LPDIRECT3DMATERIAL3 viewport_mat;
    D3DMATERIALHANDLE viewport_mat_handle;
    int some_flags;
    char pad4_0[4];

    int Initialize(int flags);
    int InitializeDirectDraw(short dev_idx);
    int InitializeD3d();
    int SetDisplayMode(unsigned short width, unsigned short height, unsigned short bpp);
    int CreateBackBuffers(HWND window, unsigned int width, unsigned int height, unsigned int bpp, int flags);
    int CreateZBuffer(unsigned short format_idx);
    int CreateD3dDevice(short d3d_idx);
    void ChooseTextureFormats();
    void ClearPreferredTextureFormats();
    int SetupZBuffer();
    void ReleaseViewport();
    void ReleaseD3d();
    void DeinitD3d();
    int CreateViewport(int flags);
    void ClearViewport(short mode);
    int CreateViewportMaterials();
    int SetViewportMaterial(unsigned char r, unsigned char b, unsigned char g);
    void SetRenderState();
    void QueryMemory();
    int DeviceInit();
    int GetSurfaceDesc(LPDDSURFACEDESC2 desc, LPDIRECTDRAWSURFACE4 surf);
    int AddD3dDevice(GUID* guid, char* name, LPD3DDEVICEDESC desc, short unk);
    int AddTextureFormat(LPDDPIXELFORMAT fmt);
    int AddMode(short width, short height, short unk);
    int AddDevice(GUID* guid, const char* device_name, const char* device_name_full);
    int AddZBufferFormat(LPDDPIXELFORMAT fmt);
    short BeginScene();
    short EndScene();

    int SupportsResolution(unsigned short width, unsigned short height, unsigned short bpp);

    void ResetDevThing();
    void ResetDevices();

    char* DeviceDisplayName(short idx);
    short DeviceByName(char* name);
    short DeviceByDisplayName(char* name);

    short D3dDeviceByName(short idx, const char* name);

    char* DeviceName(short idx);

    short DeviceModeCount(short idx);
    short DeviceModeResolution(short idx, short mode, short* width, short* height, short* bpp);
    short SupportsBitDepth(short dev, short bpp);

    int SetWindowOrigin(int screen_x, int screen_y);
    int BitCount(unsigned int in);

    int SetCooperativeLevel();

    void RenderText(short x, short y, const char* str);

    bool RestoreSurfaces();
    int FlipDisplay();
};

int DeviceCount();
char* DeviceDisplayName(short idx);
short DeviceByDisplayName(char* name);
short DeviceByName(char* name);
short D3dDeviceByName(short idx, const char* name);
char* DeviceName(short idx);
short DeviceModeCount(short idx);
short DeviceModeResolution(short idx, short mode, short* width, short* height, short* bpp);
short SupportsBitDepth(short dev, short bpp);
void ClearViewport(short mode);
short BeginScene();
short EndScene();

short FormatResolution(short dev, short mode, char* out);
short ResolutionToMode(short dev, short width, short height, short bpp);

void SetWindowOrigin(int screen_x, int screen_y);

short BestDevice();

int FlipDisplay();
void RenderText(short x, short y, const char* str);

int InitWrapper(int flags);
int ConstructGraphicsGlobals();
char InitializeGraphics(int a);
void SetCapGlobals();

class UnknownRender {
    public:
    char pad[148];
    D3dRenderer* renderer;

    UnknownRender();
};
