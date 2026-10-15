#pragma once

#include <windows.h>

struct ImageRef {
    short idx;
    short refcount;
    char name[32];
    short unk;
};

struct Glyph {
    float fraction_width;
    char ch;
    char idx;
    char width;
    char y_off;
    char row;
    char col;

    char pad[2];
};

struct Font {
    // height/width divided by total dimension
    float fraction_height;
    float fraction_width;
    short bitmap;

    short tile_width;
    short tile_height;
    short cols;
    short v23;
    short v16;
    short v18;
    short line_spacing;

    Glyph glyphs[128];
};

class Widget;
class GraphicWidget;

#undef LoadImage

class CInterface {
    public:
    GraphicWidget* unk_graphic;
    GraphicWidget* mouse_graphic;
    GraphicWidget* bg_graphic;
    char bg_active;
    char bg_graphic_name[64];
    char pad0[67];
    Font fonts[11];
    short font_count;
    ImageRef images[375];
    unsigned short image_count;
    Widget* controls[600];
    short control_count;
    char pad2[94];
    short unk2;
    short unk3;
    char pad3[628];

    CInterface();

    int InitGraphics();

    int LoadFont(const char* name);
    Font* GetFontHandle(short idx);

    short LoadImage(const char* name, short unk);
    short GetImageHandle(short img);

    bool SetBackground(const char* name, short x, short y, short width, short height, unsigned short unk);
    bool ActivateBackground(short x, short y, short width, short height, unsigned short unk);

    void ReleaseImage(short slot);

    short AddControl(Widget* widget);
    void RemoveControl(Widget* widget);

    void Render(short unk);
};

class Widget {
    public:
    // TODO: 10 virtual methods
    virtual short Render(int unk) = 0; // TODO vtable[1]
    virtual void SetText(const char* desc); // vtable[5];
    Widget();
    float pos_x;
    float pos_y;
    float unk_8;

    // Possibly left/top/right/bottom
    short unk_12;
    short unk_14;
    short width;
    short height;

    int unk_20;
    char text[64];
    short unk_88;
    short unk_90;
    short active;
    short unk_94;
    CInterface* ui;
    int unk_100;
    int unk_104;
    int unk_108;
    int unk_112;
    char pad3[2];
    short unk_114;
    int unk_116;
    char unk_120;
    unsigned short unk_122;
    int unk_124;
    int unk_128;

    void SetPosition(short a, short b, short c);
};

struct GraphicImageSlot {
    RECT rect;
    short img;
    short unk6;
    short unk7;
    short pad;
};

class GraphicWidget : public Widget {
    public:
    GraphicWidget(CInterface* ui);
    char filler[4];
    short fil_4;
    char filler2[6];
    GraphicImageSlot* slots;
    short slot_capacity;
    short slot_count;

    virtual short Render(int unk); // vtable[1]

    bool SetImage(const char* name, short unk, short unk2, short width, short height, unsigned short unk3, short unk4, short unk5);
    void AllocImageSlots(short count);
    void UnloadImages();
    bool LoadImage(const char* name, short left, short top, short right, short bottom, unsigned short unk2, short unk3, short unk4);
};

// VTABLE: REDLINE 0x005866B4
class ButtonWidget : public GraphicWidget {
    public:
    short unk0;
    short unk1;
    short unk2;
    short unk3;

    ButtonWidget(CInterface* ui);

    virtual short Render(int unk); // vtable[1]
                                   //
    void SetButtonUnk(short unk0, short unk1, short unk2);
};

// VTABLE: REDLINE 0x00586634
class LabelWidget : public Widget {
    public:
    LabelWidget(CInterface* ui);

    virtual short Render(int unk); // vtable[1]

    char pad[4];
    short unk0;
    char pad2[6];
    short unk1;
    char pad3[2];
};

enum WidgetType {
    WIDGET_LABEL = 0,
    WIDGET_BUTTON = 1,
    WIDGET_GRAPHIC = 2,
};

Widget* CreateWidget(int type, CInterface* gui);
