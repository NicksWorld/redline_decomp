#include "texture_mgr.h"

#include <d3d.h>
#include <stdio.h>

#include "globals.h"
#include "log.h"
#include "render.h"
#include "file.h"

#undef LoadImage

// GLOBAL: REDLINE 0x005CD17C
class TextureMgr *g_TextureMgr = NULL;
// GLOBAL: REDLINE 0x005A0F28
class BitmapHolder *g_BitmapHolder = NULL;

// STUB: REDLINE 0x005426B0
TextureMgr::TextureMgr() {
    this->unk_struct = NULL;
    this->pad2 = 0;
    this->unk_count = 0;
    this->unk3 = 1;
    this->slot_count = 10;
    this->unk = 0;
    this->use_dxtexmgr = 0;
    this->use_mip = 0;
    this->detail_flags = 0;
    this->unk2 = -1;
    this->mip_enabled = -1;
    this->flags = 0;
    this->texdetail = 0;
    this->unk6 = 0;
    for (short i = 0; i < this->slot_count; ++i) {
        this->slots[i].count = 0;
        this->slots[i].texture_int = NULL;
        this->slots[i].surfaces = NULL;
        this->slots[i].ptr1 = NULL;
        this->slots[i].ptr = NULL;
        this->slots[i].ptr2 = NULL;
    }
}

// FUNCTION: REDLINE 0x00544754
bool TextureMgr::Init(int init_flags) {
    struct Locals {
        int flags;
        bool first_iter;
    } l;
    this->free_texmem = g_Direct3d->GetFreeTextureMemory();
    // this->free_texmem = free_texmem;
    l.flags = init_flags;
    l.first_iter = true;
    while (true) {
        if (this->AllocTextureSlots(l.flags))
            return 1;
        if (l.first_iter) {
            this->free_texmem = this->consumed_bytes;
            l.first_iter = false;
        } else if ((l.flags & 8) != 0) {
            l.flags -= 8;
            l.flags |= 4;
        } else if ((l.flags & 4) != 0) {
            l.flags -= 4;
            l.flags -= 1;
        } else {
            l.flags |= 8;
        }
        g_Log.Print("*Warning - failed to create texture slots...trying again with a lower detail.");
    }
    return 0;
}

// FUNCTION: REDLINE 0x0048F480
short GetUsePalettedTextures() {
    return g_UsePalettedTextures;
}

// FUNCTION: REDLINE 0x0048FD6A
short GetUseMips() {
    return g_UseMips;
}

// FUNCTION: REDLINE 0x0054482A
bool TextureMgr::AllocTextureSlots(int flags) {
    this->flags = flags;
    bool success = true;

    this->RemoveSlots();

    this->texdetail = 0;
    if ((flags & 4) != 0) {
        this->texdetail = 2;
    } else if ((flags & 8) != 0) {
        this->texdetail = 1;
    }

    this->use_mip = 0;
    if ((flags & 2) != 0 && g_Direct3d->ddraw_devices[g_Direct3d->sel_ddraw].supports_mipmap)
        this->use_mip = 1;
    this->detail_flags = 0;
    this->use_dxtexmgr = (flags & 1) == 0;
    if (this->use_dxtexmgr) {
        this->detail_flags |= 1;
        g_Log.Debug("Using DirectX texture manager");
        return 1;
    }

    short bpp = 8;
    g_Direct3d->BestTextureFormat(&bpp, 0);
    if (bpp > 8 || g_Direct3d->unk_flag || !GetUsePalettedTextures()) {
        g_Log.Print("Creating texture slots: (no palette support)");
        g_Direct3d->unk_flag = 0;
    } else {
        g_Log.Print("Creating texture slots:");
    }

    int table[100] = {
        0, 0, 132, 32, 32, 0, 0, 36, 16, 12, //
        0, 0, 112, 32, 32, 0, 0, 36, 16, 12, //
        0, 72, 40, 24, 20, 0, 14, 24, 16, 12, //
        0, 69, 32, 16, 16, 0, 0, 32, 16, 12, //
        24, 96, 48, 16, 16, 0, 16, 24, 16, 12, //
        18, 66, 40, 16, 16, 0, 14, 24, 16, 12, //
        32, 96, 48, 32, 32, 0, 16, 24, 16, 16, // 
        32, 96, 48, 32, 32, 0, 16, 24, 16, 16, //
        0, 96, 96, 32, 32, 0, 16, 24, 16, 16, //
        0, 96, 96, 32, 32, 0, 16, 24, 16, 16 //
    };

    int bytes_per_pixel = 1;
    if (bpp == 16)
        bytes_per_pixel = 2;
    else if (bpp > 16)
        bytes_per_pixel = 3;

    int mem_consumption = 0x700000 * bytes_per_pixel;
    int tbl_idx = 60;

    if (this->texdetail == 1) {
        this->detail_flags = 4;
        mem_consumption = bytes_per_pixel * 2048 * 2048;
        if (this->free_texmem < mem_consumption) {
            mem_consumption = bytes_per_pixel * 1024 * 2048;
            tbl_idx = 20;
            if (this->free_texmem < mem_consumption) {
                this->texdetail = 2;
                mem_consumption = bytes_per_pixel * 1024 * 1024;
            }
        } else {
            tbl_idx = 80;
        }
    }
    if (this->texdetail == 2) {
        this->detail_flags = 8;
        tbl_idx = 0;
    }
    
    switch(this->detail_flags) {
        case 0:
            g_Log.Print("   High detail textures");
            break;
        case 4:
            g_Log.Print("   Medium detail textures");
            break;
        case 8:
            g_Log.Print("   Low detail textures");
            break;
    }
    if (this->use_mip) {
        tbl_idx += 10;
        g_Log.Print("   MipMapping on");
    }
    if (this->use_mip)
        this->detail_flags |= 2;

    if (this->unk2 != -1 && this->unk2 != this->detail_flags) {
        // TODO ??? Called on 5A0f28 (not this)
    }
    this->unk2 = this->detail_flags;

    char buf[128];
    sprintf(buf, "Texture holder ram (Available: %d   Used:%d)", this->free_texmem, mem_consumption);
    g_Log.Print(buf);
    this->consumed_bytes = 0;
    if (g_Direct3d->unk_flag) {
        if (this->MakeSlots(&slots[0], table[tbl_idx], 256, 256, 8, 0) < 0) {
            sprintf(buf, "*Error - making slots (256x256x8) created: %d", this->slots[0].count);
            g_Log.Print(buf);
            success = 0;
        }
        if (this->MakeSlots(&slots[1], table[tbl_idx + 1], 128, 128, 8, 0) < 0) {
            sprintf(buf, "*Error - making slots (128x128x8) created: %d", this->slots[1].count);
            g_Log.Print(buf);
            success = 0;
        }
        if (this->MakeSlots(&slots[2], table[tbl_idx + 2], 64, 64, 8, 0) < 0) {
            sprintf(buf, "*Error - making slots (64x64x8) created: %d", this->slots[2].count);
            g_Log.Print(buf);
            success = 0;
        }
        if (this->MakeSlots(&slots[3], table[tbl_idx + 3], 32, 32, 8, 0) < 0) {
            sprintf(buf, "*Error - making slots (32x32x8) created: %d", this->slots[3].count);
            g_Log.Print(buf);
            success = 0;
        }
        if (this->MakeSlots(&slots[4], table[tbl_idx + 4], 16, 16, 8, 0) < 0) {
            sprintf(buf, "*Error - making slots (16x16x8) created: %d", this->slots[4].count);
            g_Log.Print(buf);
            success = 0;
        }
    } else {
        if (this->MakeSlots(&slots[0], table[tbl_idx], 256, 256, 16, 0) < 0) {
            sprintf(buf, "*Error - making slots (256x256x16) created: %d", this->slots[0].count);
            g_Log.Print(buf);
            success = 0;
        }
        if (this->MakeSlots(&slots[1], table[tbl_idx + 1], 128, 128, 16, 0) < 0) {
            sprintf(buf, "*Error - making slots (128x128x16) created: %d", this->slots[1].count);
            g_Log.Print(buf);
            success = 0;
        }
        if (this->MakeSlots(&slots[2], table[tbl_idx + 2], 64, 64, 16, 0) < 0) {
            sprintf(buf, "*Error - making slots (64x64x16) created: %d", this->slots[2].count);
            g_Log.Print(buf);
            success = 0;
        }
        if (this->MakeSlots(&slots[3], table[tbl_idx + 3], 32, 32, 16, 0) < 0) {
            sprintf(buf, "*Error - making slots (32x32x16) created: %d", this->slots[3].count);
            g_Log.Print(buf);
            success = 0;
        }
        if (this->MakeSlots(&slots[4], table[tbl_idx + 4], 16, 16, 16, 0) < 0) {
            sprintf(buf, "*Error - making slots (16x16x16) created: %d", this->slots[4].count);
            g_Log.Print(buf);
            success = 0;
        }
    }

    if (this->MakeSlots(&slots[6], table[tbl_idx + 6], 128, 128, 16, 1) < 0) {
        sprintf(buf, "*Error - making slots (128x128x16A) created: %d", this->slots[5].count);
        g_Log.Print(buf);
        success = 0;
    }
    if (this->MakeSlots(&slots[7], table[tbl_idx + 7], 64, 64, 16, 1) < 0) {
        sprintf(buf, "*Error - making slots (64x64x16A) created: %d", this->slots[6].count);
        g_Log.Print(buf);
        success = 0;
    }
    if (this->MakeSlots(&slots[8], table[tbl_idx + 8], 32, 32, 16, 1) < 0) {
        sprintf(buf, "*Error - making slots (32x32x16A) created: %d", this->slots[7].count);
        g_Log.Print(buf);
        success = 0;
    }
    if (this->MakeSlots(&slots[9], table[tbl_idx + 9], 16, 16, 16, 1) < 0) {
        sprintf(buf, "*Error - making slots (16x16x16A) created: %d", this->slots[8].count);
        g_Log.Print(buf);
        success = 0;
    }

    this->Unknown();
    this->Unknown2();
    this->SetMip(this->use_mip);

    return success;
}

// FUNCTION: REDLINE 0x00543EBD
void TextureMgr::Unknown() {
    short i;
    for (i = 0; i < this->unk_count; ++i) {
        this->unk_struct[i].unk = -1;
    }
    for (short j = 0; j < this->slot_count; ++j) {
        for (i = 0; i < this->slots[j].count; ++i) {
            this->slots[j].ptr1[i] = NULL;
            this->slots[j].ptr[i] = -1;
            this->slots[j].ptr2[i].a = i - 1;
            this->slots[j].ptr2[i].b = i + 1;
        }

        this->slots[j].unk3 = 0;
        this->slots[j].unk4 = this->slots[j].count - 1;
        this->slots[j].unk22 = 0;
    }
    this->unk = 0;
    this->unk3 = 1;
}

// FUNCTION: REDLINE 0x0054402F
void TextureMgr::Unknown2() {
    this->unk5 = 0;
    for (short i = 0; i < this->slot_count; ++i) {
        this->slots[i].unk1 = 0;
        this->slots[i].unk11 = 0;
        this->slots[i].unk5 = this->slots[i].unk4;
    }
    this->unk6 = 0;
    this->unk1 = 0;
}

// FUNCTION: REDLINE 0x005446C4
void TextureMgr::SetMip(short use_mip) {
    if (this->mip_enabled == use_mip)
        return;
    this->mip_enabled = use_mip;

    int setting = 1;
    if (use_mip) {
        setting = 2;
        if (GetUseMips() == 2)
            setting = 3;
    }

    g_Direct3d->D3dDevice()->SetTextureStageState(0, D3DTSS_MIPFILTER, setting);
}

// FUNCTION: REDLINE 0x005427FF
void TextureMgr::RemoveSlots() {
    g_Log.Print("Removing Texture Slots");
    for (short i = 0; i < this->slot_count; ++i) {
        this->FreeSlot(i);
    }
    this->Unknown();
}

// FUNCTION: REDLINE 0x0054285A
void TextureMgr::FreeSlot(short slot) {
    if (slot > this->slot_count)
        return;
    for (short i = 0; i < this->slots[slot].count; ++i) {
        if (this->slots[slot].texture_int[i]) {
            this->slots[slot].texture_int[i]->Release();
            this->slots[slot].texture_int[i] = NULL;
        }
        if (this->slots[slot].surfaces[i]) {
            this->slots[slot].surfaces[i]->Release();
            this->slots[slot].surfaces[i] = NULL;
        }
    }

    if (this->slots[slot].texture_int) {
        delete this->slots[slot].texture_int;
        this->slots[slot].texture_int = NULL;
    }
    if (this->slots[slot].surfaces) {
        delete this->slots[slot].surfaces;
        this->slots[slot].surfaces = NULL;
    }
    if (this->slots[slot].ptr1) {
        delete this->slots[slot].ptr1;
        this->slots[slot].ptr1 = NULL;
    }
    if (this->slots[slot].ptr2) {
        delete this->slots[slot].ptr2;
        this->slots[slot].ptr2 = NULL;
    }
    if (this->slots[slot].ptr) {
        delete this->slots[slot].ptr;
        this->slots[slot].ptr = NULL;
    }

    this->slots[slot].count = 0;
}

// FUNCTION: REDLINE 0x005456A1
short TextureMgr::MakeSlots(TextureSlot* dst, short count, short width, short height, short bpp, short alpha) {
    if (count <= 0)
        return 1;
    LPDIRECTDRAWPALETTE ddraw_palette = NULL;
    short dst_bpp = bpp;
    TextureFormat* fmt = g_Direct3d->BestTextureFormat(&dst_bpp, alpha);
    if (dst_bpp != bpp || !fmt)
        return -1;

    dst->count = 0;
    if (this->use_mip) {
        int texture_bytes = 0;
        switch (height) {
            case 256:
                texture_bytes = 256 * 256;
            case 128:
                texture_bytes += 128 * 128;
            case 64:
                texture_bytes += 64 * 64;
            case 32:
                texture_bytes += 32 * 32;
            case 16:
                texture_bytes += 16 * 16;
        }

        texture_bytes *= bpp / 8;
        if (!texture_bytes)
            texture_bytes = width * height * (bpp / 8);
        dst->size_bytes = texture_bytes;
    } else {
        dst->size_bytes = width * height * (bpp / 8);
    }

    dst->texture_int = new IDirect3DTexture2*[count];
    dst->surfaces = new LPDIRECTDRAWSURFACE4[count];
    dst->ptr1 = new void*[count];
    dst->ptr = new short[count];
    dst->ptr2 = new Tmp[count];

    dst->width = width;
    dst->height = height;
    dst->bpp = bpp;
    dst->alpha = alpha;
    dst->unk2 = 0;

    short i;
    for (i = 0; i < count; ++i) {
        dst->texture_int[i] = NULL;
        dst->surfaces[i] = NULL;
        dst->ptr1[i] = NULL;
        dst->ptr2[i].b = i - 1;
        dst->ptr2[i].a = i + 1;
    }
    dst->unk3 = 0;
    dst->unk4 = count - 1;

    short mip_count = 0;
    if (this->use_mip) {
        switch (height) {
            case 256:
                mip_count = 5;
                break;
            case 128:
                mip_count = 4;
                break;
            case 64:
                mip_count = 3;
                break;
            case 32:
                mip_count = 2;
                break;
        }
    }

    for (i = 0; i < count; ++i) {
        DDSURFACEDESC2 desc;
        memset(&desc, 0, sizeof(desc));
        memcpy(&desc.ddpfPixelFormat, &fmt->fmt, sizeof(desc.ddpfPixelFormat));
        desc.dwSize = sizeof(desc);
        desc.dwFlags = 4103; // TODO
        desc.ddsCaps.dwCaps = g_Direct3d->GetCaps() | 0x4001000;
        short texture_nlvram = g_Direct3d->d3d_devices[g_Direct3d->sel_ddraw].a[g_Direct3d->sel_d3d].texture_nlvram;
        if (texture_nlvram)
            desc.ddsCaps.dwCaps |= 0x20000000;
        desc.dwHeight = height;
        desc.dwWidth = width;
        if (mip_count) {
            desc.dwFlags |= 0x20000;
            desc.dwMipMapCount = mip_count;
            desc.ddsCaps.dwCaps |= 0x400008;
        }
        
        if (!g_Direct3d->CreateSurface(&desc, &dst->surfaces[i]))
            return -1;
        this->consumed_bytes += dst->size_bytes;

        int v18 = 0;
        if ((desc.ddpfPixelFormat.dwFlags & 0x20) != 0) {
            v18 = 68;
        }
        int res;
        if (v18) {
            tagPALETTEENTRY palette[256];
            memset(palette, 0, sizeof(palette));
            if (!g_Direct3d->CreatePalette(v18, palette, &ddraw_palette))
                return -1;
            
            res = dst->surfaces[i]->SetPalette(ddraw_palette);
            if (res) {
                g_Log.DxErr("setting Texture palette", res);
                return -1;
            }
            ddraw_palette->Release();
        }
        
        res = dst->surfaces[i]->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&dst->texture_int[i]);
        if (res) {
            g_Log.DxErr("creating Texture Interface", res);
            return -1;
        }
        ++dst->count;
    }

    char buf[128];
    const char* alpha_spec = alpha == 1 ? "A" : "";
    sprintf(buf, "  Creating %d slots  (%dx%dx%d%s) ", count, width, height, bpp, alpha_spec);
    g_Log.Print(buf);
    return 1;
}

// STUB: REDLINE 0x005431D7
void TextureMgr::LoadTextures() {
    for (short i = 1; i < this->unk_count; ++i) {
        short unk = this->unk_struct[i].unk2;
        if (unk < 0)
            continue;

        short detail_flags = this->detail_flags;
        if (this->unk_struct[i].unk0) {
            // TODO: Something->Release()?
            this->unk_struct[i].unk0 = NULL;
        }
        // TODO: No textures get loaded at this point in the decomp, so delaying impl
    }
}

// FUNCTION: REDLINE 0x00417EA0
BitmapHolder::BitmapHolder() {
    this->slot_count = 0;
    this->bmp_count = 0;
    this->renderer = NULL;
    this->slots = NULL;
    this->base_path[0] = NULL;
    this->used_mem = 0;
    this->unk3 = 0;
    this->unk5 = 0;
}

// FUNCTION: REDLINE 0x00418634
short BitmapHolder::SetRenderer(D3dRenderer* renderer) {
    this->renderer = renderer;
    return 1;
}

void SetBitmapAssetPath(const char* path) {
    g_BitmapHolder->SetAssetPath(path);
}

// FUNCTION: REDLINE 0x00418001
void BitmapHolder::SetAssetPath(const char* path) {
    if (strlen(path) && strlen(path) < 0x80 && path) {
        GetAssetFilesystemPath("*.tga", path, this->base_path);
        char *final_slash = strrchr(this->base_path, '\\');
        if (final_slash)
            final_slash[1] = 0;
    } else {
        strcpy(this->base_path, "\\");
    }
}

// FUNCTION: REDLINE 0x0041807F
void BitmapHolder::AllocSlots(short count) {
    BitmapSlot* new_slots = new BitmapSlot[this->slot_count + count];
    if (this->slot_count) {
        memcpy(new_slots, this->slots, this->slot_count * sizeof(BitmapSlot));
        if (this->slots) {
            delete this->slots;
            this->slots = NULL;
        }
    }
    this->slots = new_slots;
    for (short i = 0; i < count; ++i) {
        this->slots[this->slot_count + i].unk3 = 0;
        this->slots[this->slot_count + i].surf = NULL;
    }
    this->slot_count += count;
}

// STUB: REDLINE 0x004186C7
short BitmapHolder::FindImage(const char* name) {
    for (short i = 0; i < this->slot_count; ++i) {
        if (this->slots[i].unk3 && !this->slots[i].unk && !strcmp(this->slots[i].name, name)) {
            return i;
        }
    }
    return -1;
}

// FUNCTION: REDLINE 0x0045796D
int GetFormattedColor(LPDIRECTDRAWSURFACE4 surf, int color) {
    int ret = -1;

    DDSURFACEDESC2 desc;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);

    HDC hdc;
    int pixel;
    if (color != -1 && !surf->GetDC(&hdc)) {
        pixel = GetPixel(hdc, 0, 0);
        SetPixel(hdc, 0, 0, color);
        surf->ReleaseDC(hdc);
    }

    int res;
    do {
        res = surf->Lock(NULL, &desc, 0, NULL);
    } while ( res == DDERR_WASSTILLDRAWING);

    if (!res) {
        ret = *((int*)desc.lpSurface) & ((1 << desc.ddpfPixelFormat.dwRGBBitCount) - 1);
        surf->Unlock(NULL);
    }
    if (color == -1 && !surf->GetDC(&hdc)) {
        SetPixel(hdc, 0, 0, pixel);
        surf->ReleaseDC(hdc);
    }
    return ret;
}

// FUNCTION: REDLINE 0x0041885E
short BitmapHolder::LoadImageInnerInner(const char* name, short slot, short unk) {
    char path[128];
    strcpy(path, this->base_path);
    strcat(path, name);
    strcat(path, ".tga");

    if (this->renderer->LoadImage(path, &this->slots[slot]) < 0)
        return -1;

    if ((unk & 2) != 0) {
        int color = GetFormattedColor(this->slots[slot].surf, -1);
        DDCOLORKEY colorkey;
        colorkey.dwColorSpaceHighValue = color;
        colorkey.dwColorSpaceLowValue = color;
        int res = this->slots[slot].surf->SetColorKey(8, &colorkey);
    }
    this->slots[slot].flags = unk;

    if (strlen(name) > 63) {
        char buf[128];
        sprintf(buf, "*Error: BitmapHolder: texture name must be <64 characters (%s)", name);
        g_Log.Debug(buf);
    }

    this->used_mem += 4 * this->slots[slot].width * this->slots[slot].height;
    strncpy(this->slots[slot].name, name, 63);
    return slot;
}

// FUNCTION: REDLINE 0x0041875D
short BitmapHolder::LoadImageInner(const char* path, short unk) {
    short free_slot = -1;
    if (this->renderer) {
        for (short i = 0; i < this->slot_count; ++i) {
            if (!this->slots[i].unk3) {
                free_slot = i;
                break;
            }
        }
        if (free_slot < 0) {
            free_slot = this->slot_count;
            this->AllocSlots(100);
        }
        if ((this->LoadImageInnerInner(path, free_slot, unk) & 0x8000) != 0)
            return -1;
        this->slots[free_slot].unk3 = 3;
        this->slots[free_slot].unk = 0;
        ++this->bmp_count;
    }
    return free_slot;
}

// FUNCTION: REDLINE 0x00418651
short BitmapHolder::LoadImage(const char* path, short unk) {
    char name_lower[128];
    strcpy(name_lower, path);
    strlwr(name_lower);
    short idx = this->FindImage(name_lower);
    if (idx < 0)
        idx = this->LoadImageInner(name_lower, unk);
    return idx;
}

// FUNCTION: REDLINE 0x0048EF23
short RedlineLoadImage(const char* path, short unk) {
    if (g_ShouldLoadImages) {
        return g_BitmapHolder->LoadImage(path, unk);
    }
    return 0;
}

// FUNCTION: REDLINE 0x00418ED2
void BitmapHolder::UnloadImage(short slot) {
    if (slot >= 0 && slot < this->slot_count && this->slots[slot].unk3) {
        if (this->slots[slot].surf) {
            this->slots[slot].surf->Release();
            this->slots[slot].surf = NULL;
        }
        if (this->slots[slot].palette) {
            this->slots[slot].palette->Release();
            this->slots[slot].palette = NULL;
        }
        if (this->slots[slot].unk) {
            this->unk3 -= 4 * this->slots[slot].width * this->slots[slot].height;
            --this->unk5;
        } else {
            this->used_mem -= 4 * this->slots[slot].width * this->slots[slot].height;
        }
        this->slots[slot].unk3 = 0;
        --this->bmp_count;
    }
}

// FUNCTION: REDLINE 0x0048EF9E
void RedlineUnloadImage(short slot) {
    g_BitmapHolder->UnloadImage(slot);
}

// FUNCTION: REDLINE 0x0048EFB5
void BitmapHolderDraw(short handle, short x_off, short y_off, RECT* rect) {
    g_BitmapHolder->DrawSlot(handle, x_off, y_off, rect);
}

// FUNCTION: REDLINE 0x00419557
void BitmapHolder::DrawSlot(short handle, short x_off, short y_off, RECT* rect) {
    if (this->renderer && this->slots) {
        int h = rect->bottom - rect->top;
        int w = rect->right - rect->left;
        RECT r;
        r.top = y_off;
        r.bottom = h + y_off;
        r.left = x_off;
        r.right = w + x_off;
        if (!this->renderer->BlitSurface(this->slots[handle].surf, &r, rect, this->slots[handle].flags)) {
            char buf[128];
            sprintf(buf, "  bitmap: %s", this->slots[handle].name);
            g_Log.Debug(buf);
        }
    }
}
