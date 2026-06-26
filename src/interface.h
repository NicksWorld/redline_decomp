#pragma once

struct ImageRef {
    short idx;
    short refcount;
    char name[32];
    short unk;
};

class CInterface {
    public:
    char pad0[17304];
    short font_count;
    ImageRef images[375];
    unsigned short image_count;
    int arr[600];
    char pad2[728];

    CInterface();

    int InitGraphics();
    int LoadFont(const char* name);
    short LoadImage(const char* name, short unk);
};
