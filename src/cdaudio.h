#pragma once

#include <windows.h>
#include <mmsystem.h>

class CdAudio {
    public:
    int unk0;
    int initialized;
    int unk2;
    int unk3;
    short unk4;
    short unk5;

    MCI_OPEN_PARMS open_params;

    CdAudio();
    int Init();
};
