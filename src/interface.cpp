#include "interface.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "texture_mgr.h"
#include "log.h"
#include "globals.h"

// GLOBAL: REDLINE 0x005C3F50
CInterface* g_Interface = NULL;

// STUB: REDLINE 0x004A352A
CInterface::CInterface() {
    // TODO
    short i;
    for ( i = 0; i < 600; ++i )
        this->controls[i] = NULL;
    for ( i = 0; i < 375; ++i ) {
        this->images[i].idx = -1;
        this->images[i].refcount = 0;
        this->images[i].name[0] = 0;
    }
    this->font_count = 0;
    this->image_count = 0;
}

// FUNCTION: REDLINE 0x004A40F2
int CInterface::InitGraphics() {
    // TODO: There is some dead code (inlined?) present that makes this not match
    if (!this->LoadFont("mini8-NM"))
        return 0;
    if (!this->LoadFont("newCop16-NM"))
        return 0;
    if (!this->LoadFont("mini8Blue-NM"))
        return 0;
    this->unk_graphic = new GraphicWidget(this);
    this->mouse_graphic = new GraphicWidget(this);
    this->mouse_graphic->SetImage("mouse", 0, 0, 16, 16, 2, 0, 0);

    this->unk2 = 624;
    this->unk3 = 464;
    this->unk0 = 0;

    this->bg_graphic = new GraphicWidget(this);

    this->bg_graphic->unk_122 = 0;
    this->bg_graphic->SetUnkFloats(0, 0, 0);
    this->bg_graphic->SetDescription("Background Image");
    this->bg_graphic->unk_92 = 0;
    this->bg_graphic->unk_120 = 0;

    this->unk1 = 0; // Weird alignment, maybe supposed to be string null term with wrong index?
    return 1;
}

// FUNCTION: REDLINE 0x00471F80
bool LoadFontDef(const char* name, Font* font) {
    FileContainer file;
    if (!font)
        return false;

    for (int i = 0; i < 128; ++i) {
        font->glyphs[i].idx = -1;
    }

    char fontdef_filename[128];
    strcpy(fontdef_filename, name);
    strcat(fontdef_filename, ".def");

    char fontdef_path[128];
    GetAssetFilesystemPath(fontdef_filename, g_GameData->data_dir, fontdef_path);
    if (!file.ReadTextAsset(fontdef_path)) {
        Warn("Unable to load Font Def file: %s", fontdef_path);
        return 0;
    }

    char buf[128];
    int tile_width, tile_height, bitmap_dimension;
    short cols;
    int line_spacing;
    int upper_start, lower_start, last_alpha;

    int off = 0;
    int new_off = file.ReadLine(off, buf);
    if (*buf == '#') {
        sscanf(&buf[1], "%d %d %d %d ", &tile_height, &tile_width, &cols, &line_spacing);
        off = new_off;
        new_off = file.ReadLine(new_off, buf);
        sscanf(&buf[1], "%d %d %d", &upper_start, &lower_start, &last_alpha);
        off = new_off;
        new_off = file.ReadLine(new_off, buf);
        sscanf(&buf[1], "%d", &bitmap_dimension);
    } else {
        sscanf(buf, "%d %d %d", &tile_height, &tile_width, &cols);
        line_spacing = 0;
        upper_start = 26;
        lower_start = 0;
        last_alpha = 52;
        bitmap_dimension = 128;
    }

    bool case_insensitive = upper_start == lower_start;
    font->tile_width = tile_width;
    font->tile_height = tile_height;
    font->cols = cols;
    font->v23 = upper_start;
    font->v16 = lower_start;
    font->v18 = last_alpha;
    font->line_spacing = line_spacing;
    font->fraction_height = (float)tile_width / (float)bitmap_dimension;
    font->fraction_height = (float)tile_height / (float)bitmap_dimension;

    off = new_off;
    new_off = file.ReadLine(new_off, buf);
    short idx = 0;
    while(buf[0] && buf[1]) {
        char ch = buf[0];

        int width;
        char y_off;

        int matched = sscanf(&buf[1], "%d %d", &width, &y_off);
        int v9 = 0;
        for (int j = 0; buf[j] != NULL; ++j) {
            if (isspace(buf[j])) {
                ++v9;
                while (isspace(buf[j]))
                    ++j;
            }
        }
        if (v9 == 1) {
            ch = ' ';
            matched = sscanf(buf, "%d %d", &width, &y_off);
        }
        if (matched < 2 || ch < 0)
            return 0;

        short k = case_insensitive;
        while (true) {
            font->glyphs[ch].ch = ch;
            font->glyphs[ch].width = width;
            font->glyphs[ch].y_off = y_off;
            font->glyphs[ch].idx = idx;
            font->glyphs[ch].row = font->glyphs[ch].idx / font->cols;
            font->glyphs[ch].col = font->glyphs[ch].idx - font->cols * font->glyphs[ch].row;
            font->glyphs[ch].fraction_width = (float)width / (float)bitmap_dimension;
            if (!k)
                break;
            char orig_ch = ch;
            if (ch >= 'a' && ch <= 'z') {
                ch -= 32;
            } else if (ch >= 'A' && ch <= 'Z') {
                ch += 32;
            }

            if (orig_ch == ch)
                break;
            k = 0;
        }
        ++idx;
        if (ch == ' ')
            font->glyphs[ch].idx = 0;
        off = new_off;
        new_off = file.ReadLine(new_off, buf);
    }
    if (font->glyphs[' '].idx == -1) {
        font->glyphs[' '].idx = 0;
        font->glyphs[' '].width = font->tile_width;
    }
    return 1;
}


// Darn you windows!
#undef LoadImage

// FUNCTION: REDLINE 0x004A4307
int CInterface::LoadFont(const char* name) {
    if (this->font_count >= 10)
        return 0;
    short slot = this->LoadImage(name, 2);
    if (slot < 0)
        return 0;

    memset(&this->fonts[this->font_count], 0, sizeof(Font));
    this->fonts[this->font_count].bitmap = slot;

    if (!LoadFontDef(name, &this->fonts[this->font_count]))
        return 0;

    ++this->font_count;
    return 1;
}

// FUNCTION: REDLINE 0x004A45FA
void CInterface::ReleaseImage(short slot) {
    // Reduce refcount (and possibly free) an image
    if (slot < 0 || slot > 375)
        return;
    if (this->images[slot].refcount == 1) {
        RedlineUnloadImage(this->images[slot].idx);
        this->images[slot].idx = -1;
        this->images[slot].name[0] = 0;
        --this->image_count;
    }
    if (this->images[slot].refcount > 0)
        --this->images[slot].refcount;
}

// FUNCTION: REDLINE 0x004A43EF
short CInterface::LoadImage(const char* name, short unk) {
    short idx = -1;

    char name_upper[32];
    strcpy(name_upper, name);
    strupr(name_upper);

    for (short i = 0; i < 375; ++i) {
        if (!strcmp(this->images[i].name, name_upper) && this->images[i].unk == unk) {
            ++this->images[i].refcount;
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        if (this->image_count >= 375) {
            SomeDebug("*ERROR: CInterface::LoadImage failed > MAX_IMAGES");
            return -1;
        }
        for (idx = 0; idx < 375; ++idx) {
            if (this->images[idx].idx < 0)
                break;
        }
        if (idx == 375)
            return -1;

        this->images[idx].idx = RedlineLoadImage(name_upper, unk);
        if (this->images[idx].idx < 0)
            return -1;
        ++this->image_count;
        strcpy(this->images[idx].name, name_upper);
        this->images[idx].unk = unk;
    }

    return idx;
}

// FUNCTION: REDLINE 0x004A45CE
short CInterface::GetImageHandle(short img) {
    if (img < 0)
        return 0;
    return this->images[img].idx;
}

// FUNCTION: REDLINE 0x004A4AE8
short CInterface::AddControl(Widget* widget) {
    if (this->control_count < 600) {
        for (short i = 0; i < 600; ++i) {
            if (!this->controls[i]) {
                this->controls[i] = widget;
                this->control_count++;
                return i;
            }
        }
    }
    SomeDebug("*ERROR - Interface - Maximum controls reached");
    return -1;
}

// FUNCTION: REDLINE 0x004A481D
void CInterface::RemoveControl(Widget* widget) {
    if (!widget)
        return;

    for (short i = 0; i < 600; ++i) {
        if (this->controls[i] == widget) {
            if (this->controls[i]) {
                delete this->controls[i]; // TODO: Might have had an additional null check?
            }
            this->controls[i] = NULL;
            --this->control_count;
            return;
        }
    }
}

// FUNCTION: REDLINE 0x0049FF26
Widget* CreateWidget(int type, CInterface* gui) {
    Widget* widget = NULL;
    if (!gui)
        gui = g_Interface;
    switch(type) {
        // TODO: 0-6 are valid
        case WIDGET_GRAPHIC:
            widget = new GraphicWidget(gui);
            break;
    }
    return widget;
}

// FUNCTION: REDLINE 0x004A0163
Widget::Widget() {
    // this->vtable = (void **)&off_58660C;
    this->unk_4 = 0;
    this->unk_0 = 0;
    this->unk_20 = 0;
    this->desc[0] = 0;
    this->unk_88 = 0;
    this->unk_90 = 0;
    this->unk_92 = 0;
    this->unk_100 = 0;
    this->unk_104 = 0;
    this->unk_108 = 0;
    this->unk_112 = 0;
    this->unk_94 = 5;
    this->unk_114 = 0;
    this->unk_116 = 0;
    this->unk_120 = 1;
    this->unk_122 = 1;
    // this->unk_124 = (int)sub_49FF10; Plays button click sound?
    this->unk_128 = 0;
}

// FUNCTION: REDLINE 0x004A02B1
void Widget::SetDescription(const char* desc) {
    short len = strlen(desc);
    if (len > 63)
        len = 63;
    memcpy(this->desc, desc, len);
    this->desc[len] = 0;
}

// FUNCTION: REDLINE 0x004A0242
void Widget::SetUnkFloats(short a, short b, short c) {
    this->unk_0 = a;
    this->unk_4 = b;
    this->unk_8 = c;
}

// FUNCTION: REDLINE 0x004A15DB
GraphicWidget::GraphicWidget(CInterface* ui) {
    this->ui = ui;
    this->slots = 0;
    this->slot_capacity = 0;
    this->slot_count = 0;
    this->fil_4 = 2;
}

// FUNCTION: REDLINE 0x004A59EE
void DrawInterfaceImage(short x_off, short y_off, RECT* rect, short img_idx, CInterface* ui) {
    CInterface* gui;
    if (!ui) {
        gui = g_Interface;
    } else {
        gui = ui;
    }

    short handle = gui->GetImageHandle(img_idx);
    BitmapHolderDraw(handle, x_off, y_off, rect);
}

// FUNCTION: REDLINE 0x004A1B73
short GraphicWidget::Render(int unk) {
    if (!unk && !this->unk_92 && !this->unk_90)
        return 0;
    if (this->unk_114 < 0 || this->slots[this->unk_114].img == -1)
        return 0;

    DrawInterfaceImage(
            this->slots[this->unk_114].unk6 + this->unk_0, // x_off (atlas?)
            this->slots[this->unk_114].unk7 + this->unk_4, // y_off (atlas?)
            &this->slots[this->unk_114].rect,
            this->slots[this->unk_114].img,
            this->ui);
    this->unk_12 = this->slots[this->unk_114].unk6 + this->unk_0;
    this->unk_14 = this->slots[this->unk_114].unk7 + this->unk_4;

    this->width = LOWORD(this->slots[this->unk_114].rect.right) + this->unk_12;
    this->height = LOWORD(this->slots[this->unk_114].rect.bottom) + this->unk_14;
    return this->unk_90;
}

// FUNCTION: REDLINE 0x004A1636
void GraphicWidget::AllocImageSlots(short count) {
    struct Locals {
        short wtf;
        char pad[2];
        short i;
        GraphicImageSlot * alloc;
    } l;

    l.alloc = new GraphicImageSlot[this->slot_capacity + count];
    if (l.alloc == (void*)0x3ea4e50) {
        l.wtf = 0;
    }
    for ( l.i = this->slot_capacity; l.i < this->slot_capacity + count; ++l.i) {
        l.alloc[l.i].img = -1;
        l.alloc[l.i].rect.top = 0;
        l.alloc[l.i].rect.bottom = 0;
        l.alloc[l.i].rect.left = 0;
        l.alloc[l.i].rect.right = 0;
        l.alloc[l.i].unk6 = 0;
        l.alloc[l.i].unk7 = 0;
    }
    if (this->slot_capacity > 0) {
        memcpy(l.alloc, this->slots, sizeof(GraphicImageSlot) * this->slot_capacity);
        delete this->slots;
    }
    this->slots = l.alloc;
    this->slot_capacity += count;
}

// FUNCTION: REDLINE 0x004A18B9
bool GraphicWidget::SetImage(const char* name, short x, short y, short width, short height, unsigned short flags, short unk4, short unk5) {
    this->UnloadImages();
    if (this->slot_capacity <= 0) {
        this->AllocImageSlots(4);
    }

    short img = this->ui->LoadImage(name, flags);
    if (img < 0)
        return 0;

    this->slots[this->slot_count].rect.left = x;
    this->slots[this->slot_count].rect.top = y;
    this->slots[this->slot_count].rect.right = width;
    this->slots[this->slot_count].rect.bottom = height;
    this->slots[this->slot_count].unk6 = unk4;
    this->slots[this->slot_count].unk7 = unk5;
    this->slots[this->slot_count].img = img;
    
    this->slot_count++;

    this->unk_12 = x;
    this->unk_14 = y;
    this->width = width;
    this->height = height;
    return true;
}

// FUNCTION: REDLINE 0x004A181E
void GraphicWidget::UnloadImages() {
    for (short i = 0; i < this->slot_count; ++i) {
        if (this->slots[i].img >= 0) {
            this->ui->ReleaseImage(this->slots[i].img);
            this->slots[i].img = -1;
        }
    }
    this->unk_114 = 0;
    this->slot_count = 0;
}


// FUNCTION: REDLINE 0x004A4CC9
void CInterface::Render(short unk) {
    if (this->unk0) {
        this->bg_graphic->Render(1);
    }

    // TODO: This isn't actually correct
    for (int i = 0; i < this->control_count; ++i) {
        this->controls[i]->Render(1);
    }
    // TODO
}
