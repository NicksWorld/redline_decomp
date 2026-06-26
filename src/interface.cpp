#include "interface.h"

#include <stddef.h>
#include <string.h>

#include "texture_mgr.h"
#include "log.h"

// GLOBAL: REDLINE 0x005C3F50
CInterface* g_Interface = NULL;

// STUB: REDLINE 0x004A352A
CInterface::CInterface() {
    // TODO
    short i;
    for ( i = 0; i < 600; ++i )
        this->arr[i] = 0;
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
    if (!this->LoadFont("mini8-NM"))
        return 0;
    if (!this->LoadFont("newCop16-NM"))
        return 0;
    if (!this->LoadFont("mini8Blue-NM"))
        return 0;
    // TODO
    return 1;
}

// FUNCTION: REDLINE 0x004A4307
int CInterface::LoadFont(const char* name) {
    if (this->font_count >= 10)
        return 0;
    this->LoadImage(name, 2);

    // TODO

    ++this->font_count;
    return 1;
}

// Darn you windows!
#undef LoadImage

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
