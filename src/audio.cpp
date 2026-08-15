#include "audio.h"

#include <dsound.h>
#include <windows.h>

#include "globals.h"
#include "log.h"

// GLOBAL: REDLINE 0x005CD00C
AudioManager* g_Audio = NULL;

// FUNCTION: REDLINE 0x00533DD0
AudioManager::AudioManager() {
    // *(_DWORD *)&this->pad3[9988] = 0;
    // *(_DWORD *)&this->pad3[9992] = 0;
    this->dsound = NULL;
    this->init_res = 0;
    // *(_DWORD *)&this->pad[4] = 0;
    this->initialized = 0;
    // *(_DWORD *)this->pad = 0;
    // *(_WORD *)&this->pad2[44] = 0;
    // *(_WORD *)&this->pad2[46] = 0;
    // *(_DWORD *)&this->pad2[28] = 0;
    // *(_DWORD *)&this->pad2[32] = 0;
    // this->pad2[24] = 0;
    this->channel_count = 32;
    // *(_WORD *)&this->pad2[36] = 1;
    // *(_DWORD *)&this->pad2[40] = 0;
    // *(_WORD *)&this->pad3[2] = -1;
    // *(_DWORD *)&this->pad3[10004] = 0;
    // *(_WORD *)&this->pad3[10014] = -1;
    for ( short i = 0; i < 32; ++i )
        this->sound_buffers[i].unk = 0;
    // *(_DWORD *)&this->pad3[10008] = operator new(0x34u);
    // for ( j = 0; j < 26; ++j )
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 2 * j) = -1;
    // **(_WORD **)&this->pad3[10008] = 0;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 2) = 1;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 4) = 2;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 6) = 3;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 8) = 4;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 10) = 5;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 12) = 6;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 14) = 7;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 16) = 8;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 18) = 9;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 20) = 10;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 22) = 11;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 24) = 12;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 26) = 13;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 28) = 14;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 30) = 15;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 32) = 16;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 34) = 17;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 36) = 18;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 38) = 19;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 40) = 20;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 42) = 21;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 44) = 22;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 46) = 23;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 48) = 24;
    // *(_WORD *)(*(_DWORD *)&this->pad3[10008] + 50) = 25;
}

// Packed format specifiers in declining quality
// GLOBAL: REDLINE 0x00597CDC
const int g_AudioFormats[16] = {
    44216, 44116, 44208, 44108, 22216, 22208, 22108, 11216, 
    11116, 11208, 11108, 8216, 8116, 8208, 8108
};

// FUNCTION: REDLINE 0x005394C7
int AudioQualityToFormat(int quality, WAVEFORMATEX *fmt) {
    if (!fmt)
        return 0;
    int sample_rate;
    if (quality / 1000 == 8)
        sample_rate = 8000;
    else
        sample_rate = 11025 * (quality / 1000 / 11);

    fmt->nSamplesPerSec = sample_rate;
    fmt->wBitsPerSample = quality % 100;
    fmt->nChannels = quality % 1000 / 100;
    fmt->nBlockAlign = fmt->wBitsPerSample / 8 * fmt->nChannels;
    fmt->nAvgBytesPerSec = fmt->nSamplesPerSec * fmt->nBlockAlign;
    return 1;
}

// FUNCTION: REDLINE 0x0053454D
int AudioManager::Init(unsigned short channels) {
    struct Locals {
        int res;
        DSBUFFERDESC desc;
        int j;
        unsigned short i;
        ListenerInfo listener;
        char unk[24];
    } l;
    if (channels > 32)
        channels = 32;
    this->init_res = DirectSoundCreate(NULL, &this->dsound, NULL) == 0;
    this->pad3[10000] = 1; // TODO
    if (this->init_res && this->dsound) {
        if (this->dsound->SetCooperativeLevel(g_Window, 3)) {
            g_Log.Debug("DirectSound: failed to set exclusive mode");
        }
        this->channel_count = channels;
        this->channel_count_other = this->channel_count;
        if (this->channel_count) {
            this->channel_data = new ChannelData[this->channel_count];
        }
        *(short*)this->pad3 = 32; // TODO
        for (l.i = 0; l.i < this->channel_count; ++l.i) {
            this->channel_data[l.i].unk0 = 0;
            this->channel_data[l.i].unk1 = -1;
        }
        for (l.i = 0; l.i < 32; ++l.i) {
            this->sound_buffers[l.i].unk = 0;
            this->InitSoundBuffer(&this->sound_buffers[l.i], l.i < 16);
        }
        this->listener = NULL;
        l.listener.x = l.listener.y = l.listener.z = 999999.0;
        l.listener.xFront = l.listener.yFront = l.listener.zFront = 0;
        l.listener.velX = l.listener.velY = l.listener.velZ = 0;
        this->UpdateListener(&l.listener);

        memset(&l.desc, 0, sizeof(l.desc));
        memset(&this->buf_fmt, 0, sizeof(this->buf_fmt));
        this->buf_fmt.wFormatTag = WAVE_FORMAT_PCM;
        this->quality = g_AudioFormats[0];
        AudioQualityToFormat(this->quality, &this->buf_fmt);
        l.desc.dwSize = sizeof(l.desc);
        l.desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
        l.res = this->dsound->CreateSoundBuffer(&l.desc, &this->primary_snd_buf, NULL);
        if (l.res) {
            g_Log.DsErr("Creating Sound Buffer", l.res);
            this->primary_snd_buf = NULL;
            this->initialized = 1;
            return 1;
        }

        if (this->primary_snd_buf->SetFormat((LPWAVEFORMATEX)&this->buf_fmt)) {
            l.j = 1;
            while (l.j < 16) {
                this->quality = g_AudioFormats[l.j];
                AudioQualityToFormat(this->quality, &this->buf_fmt);
                if (!this->primary_snd_buf->SetFormat(&this->buf_fmt))
                    break;
                l.j++;
            }
            if (l.j >= 16) {
                this->primary_snd_buf->Release();
                this->primary_snd_buf = NULL;
                this->initialized = 1;
                this->init_res = 0;
                return 0;
            }
        }

        if (this->primary_snd_buf->QueryInterface(IID_IDirectSound3DListener, (void**)&this->listener)) {
            this->primary_snd_buf->Release();
            this->primary_snd_buf = NULL;
            this->listener = NULL;
            this->initialized = 1;
            this->init_res = 0;
            return 0;
        }

        if (this->primary_snd_buf->Play(0, 0, 1)) {
            this->primary_snd_buf->Release();
            this->primary_snd_buf = NULL;
            this->listener->Release();
            this->listener = NULL;
        }
        this->SetMasterVolume(g_Master_Volume);
        this->listener->SetRolloffFactor(0.05, 0);
        this->listener->SetDistanceFactor(0.1, 0);
        // TODO: Unk subroutine

        this->initialized = 1;
        return 1;
    } else {
        this->initialized = 0;
        return 0;
    }
}

// FUNCTION: REDLINE 0x00538C03
void AudioManager::InitSoundBuffer(SoundBuffer* buf, int first_half) {
    PCMWAVEFORMAT fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.wf.wFormatTag = WAVE_FORMAT_PCM;
    fmt.wf.nChannels = 1;
    fmt.wf.nSamplesPerSec = 22050;
    if (first_half == 1) {
        fmt.wBitsPerSample = 8;
        fmt.wf.nBlockAlign = 1;
        buf->bits_per_sample = 8;
    } else {
        fmt.wBitsPerSample = 16;
        fmt.wf.nBlockAlign = 2;
        buf->bits_per_sample = 16;
    }
    buf->bytes_per_sec = 22050 * buf->bits_per_sample / 8;
    fmt.wf.nAvgBytesPerSec = fmt.wf.nBlockAlign * fmt.wf.nSamplesPerSec;

    DSBUFFERDESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
    desc.dwBufferBytes = buf->bytes_per_sec;
    desc.lpwfxFormat = (LPWAVEFORMATEX)&fmt;

    int res = this->dsound->CreateSoundBuffer(&desc, &buf->buf, NULL);
    if (res) {
        g_Log.DsErr("Creating Sound Buffer", res);
        return;
    }
    
    void* audio_ptr1, *audio_ptr2;
    unsigned long audio_bytes1, audio_bytes2;
    res = buf->buf->Lock(0, buf->bytes_per_sec, &audio_ptr1, &audio_bytes1, &audio_ptr2, &audio_bytes2, DSBLOCK_FROMWRITECURSOR);
    if (res != 0) {
        int fill_val;
        switch(buf->bits_per_sample) {
            case 8:
                fill_val = 128;
            case 16:
                fill_val = 0;
        }
        if (audio_bytes1)
            memset(audio_ptr1, fill_val, audio_bytes1);
        if (audio_bytes2)
            memset(audio_ptr2, fill_val, audio_bytes2);
        res = buf->buf->Unlock(audio_ptr1, audio_bytes1, audio_ptr2, audio_bytes2);
    }
    buf->unk0 = 0;
}

// GLOBAL: REDLINE 0x005873CC
const float g_VolumeScalar = 10000.0;
// GLOBAL: REDLINE 0x00585918
const float g_VolumeDivisor = 100.0;

// FUNCTION: REDLINE 0x00534A07
void AudioManager::SetMasterVolume(float volume) {
    struct Locals {
        long vol;
        int res;
        int tmp;
    } l;
    l.vol = (long)(g_VolumeScalar * volume / g_VolumeDivisor) - 10000;
    l.res = this->primary_snd_buf->SetVolume(l.vol);
    // TODO: This isn't quite right, but it has no affect
    if (l.res != 0) {
        switch (l.res) {
            case DSERR_CONTROLUNAVAIL:
                l.tmp = l.res;
                l.res = l.tmp;
                break;
            case DSERR_GENERIC:
                l.tmp = l.res;
                l.res = l.tmp;
                break;
            case DSERR_INVALIDPARAM:
                l.tmp = l.res;
                l.res = l.tmp;
                break;
            case DSERR_PRIOLEVELNEEDED:
                l.tmp = l.res;
                l.res = l.tmp;
                break;
        }
    }
}

// FUNCTION: REDLINE 0x0053549C
void AudioManager::UpdateListener(ListenerInfo* info) {
    if (this->init_res) {
        this->listener_data = *info;
        if (this->listener) {
            this->listener->SetPosition(this->listener_data.x, this->listener_data.y, this->listener_data.z, 1);
            this->listener->SetOrientation(
                    this->listener_data.xFront,
                    this->listener_data.yFront,
                    this->listener_data.zFront,
                    this->listener_data.xTop,
                    this->listener_data.yTop,
                    this->listener_data.zTop,
                    1);
            this->listener->SetVelocity(this->listener_data.velX, this->listener_data.velY, this->listener_data.velZ, 1);
        }

        short i;
        for (i = 0; i < this->channel_count_other; ++i) {
            if (this->channel_data[i].unk0 == 1) {
                // TODO: Update distance from audio source
            }
        }
        for (i = 0; i < 32; ++i) {
            // if (this->sound_buffers[i].unk == 1 && ???)
                // TODO: Update distance from audio source
        }
        if (this->listener)
            this->listener->CommitDeferredSettings();
    }
}
