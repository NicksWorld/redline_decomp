#pragma once

#include <dsound.h>

struct ListenerInfo {
    // Position
    float x, y, z;
    // Orientation
    float xFront, yFront, zFront, xTop, yTop, zTop;

    char pad[12];

    // Velocity
    float velX, velY, velZ;
};

struct ChannelData {
    char pad[120];
    int unk0;
    int unk1;
    char pad2[12];
};

struct SoundBuffer {
    int unk0;
    char pad[44];
    int bytes_per_sec;
    short bits_per_sample;
    short pad0;
    LPDIRECTSOUNDBUFFER buf;
    char pad1[28];
    int unk;
    char pad2[36];
};

class AudioManager {
    public:
    int init_res;
    int initialized;
    char pad[8];
    LPDIRECTSOUNDBUFFER primary_snd_buf;
    int quality;
    WAVEFORMATEX buf_fmt;
    IDirectSound3DListener* listener;
    ListenerInfo listener_data;
    char pad2[42];
    ChannelData* channel_data;
    short channel_count;
    short channel_count_other;
    SoundBuffer sound_buffers[32];
    char pad3[10016];

    LPDIRECTSOUND dsound;

    AudioManager();

    int Init(unsigned short channels);
    void InitSoundBuffer(SoundBuffer* buf, int first_half);
    void SetMasterVolume(float volume);
    void UpdateListener(ListenerInfo* info);
};
