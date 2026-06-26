#pragma once

#include <d3d.h>
#include <ddraw.h>

#include "render.h"
#undef LoadImage

struct Tmp {
    short a;
    short b;
};

struct TextureSlot {
    short count;
    short unk2;
    short unk1;
    short unk11;
    short height;
    short width;
    short bpp;
    short alpha;
    short unk22;
    short unk23;
    short unk3;
    short unk4;
    short unk5;
    char pad[2];
    int size_bytes;
    short* ptr;
    void** ptr1;
    Tmp* ptr2;
    IDirect3DTexture2** texture_int;
    LPDIRECTDRAWSURFACE4* surfaces;
};

struct UnkStruct {
    int unk0;
    short unk;
    short unk2;
    char pad2[10];
};

class TextureMgr {
    public:
    UnkStruct* unk_struct;
    TextureSlot slots[10];
    int unk;
    int unk1;
    short pad2;
    short unk_count;
    short use_dxtexmgr;
    short use_mip;
    int flags;
    int free_texmem;
    int consumed_bytes;
    short detail_flags;
    short unk2;
    short mip_enabled;
    short texdetail;
    short slot_count;
    short pad4;
    int unk3;
    short unk5;
    int unk6;
    
    TextureMgr();
    bool Init(int flags);
    bool AllocTextureSlots(int flags);
    short MakeSlots(TextureSlot* dst, short unk, short width, short height, short bpp, short unk2);
    void RemoveSlots();
    void FreeSlot(short slot);

    void Unknown();
    void Unknown2();
    void SetMip(short use_mip);

    void LoadTextures();
};

struct BitmapSlot {
    char name[64];
    LPDIRECTDRAWSURFACE4 surf;
    LPDIRECTDRAWPALETTE palette;
    short height;
    short width;
    short bpp; // unsure
    char pad2[4];
    short unk;
    short unk2;
    char pad3[2];
};

class BitmapHolder {
    public:
    char base_path[128];
    BitmapSlot* slots;
    int unk2;
    int unk3;
    short unk4;
    short unk5;
    short slot_count;
    D3dRenderer* renderer;
    
    BitmapHolder();
    short SetRenderer(D3dRenderer* renderer);
    void SetAssetPath(const char* path);
    void AllocSlots(short count);

    short FindImage(const char* path);
    short LoadImageInnerInner(const char* path, short slot, short unk);
    short LoadImageInner(const char* path, short unk);
    short LoadImage(const char* path, short unk);
};

void SetBitmapAssetPath(const char* path);

short RedlineLoadImage(const char* path, short unk);
