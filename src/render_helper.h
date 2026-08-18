#pragma once

// TODO: Name is a guess, no clue what it really does
class RenderHelper {
    public:
    // TODO: Has a constructor and data
    char pad[0x18c8];

    void DrawScreenOverlay(int flags, unsigned char opacity, unsigned char unk3);
};


void DrawScreenOverlay(unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned char force);
