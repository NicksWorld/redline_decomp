#pragma once

struct RendUnkStruct {
    char pad[32];
};

struct ShortStruct {
    short v[6];
};

// TODO: Name is a guess, no clue what it really does
class RenderHelper {
    public:
    RendUnkStruct* unk_arr;
    RendUnkStruct* unk_arrs2[4];
    char pad[6240];
    ShortStruct* shorts;
    char pad2[8];
    short unk;
    short unk_len;
    char pad3[68];

    RenderHelper();

    void DrawScreenOverlay(int flags, unsigned char opacity, unsigned char unk3);
};


void DrawScreenOverlay(unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned char force);
