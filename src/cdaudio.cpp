#include "cdaudio.h"

#include "globals.h"
#include "log.h"

// GLOBAL: REDLINE 0x005A478C
CdAudio* g_CdAudio = NULL;

// FUNCTION: REDLINE 0x0043AEF0
CdAudio::CdAudio() {
    this->unk0 = 0;
    this->unk3 = 0;
    this->initialized = 0;
    this->unk2 = 0;
    this->unk4 = 0;
    this->unk5 = 1;
    this->Init();
}

// FUNCTION: REDLINE 0x0043AF52
int CdAudio::Init() {
    if (!this->initialized) {
        this->open_params.wDeviceID = 0;
        this->open_params.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_CD_AUDIO;
        if (mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (ULONG)&this->open_params)) {
            g_Log.Debug("warning - can't open cd audio device.");
            return 0;
        }

        MCI_SET_PARMS set;
        set.dwTimeFormat = 10;

        if(mciSendCommandA(this->open_params.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (ULONG)&set)) {
            g_Log.Debug("warning - can't set cd audio device time format.");
            mciSendCommandA(this->open_params.wDeviceID, MCI_CLOSE, 0, 0);
            return 0;
        }
        this->initialized = 1;
    }
    return 1;
}
