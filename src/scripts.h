#pragma once

enum ScriptKind {
    SK_Cars = 0x0,
    SK_Sky = 0x1,
    SK_Includes = 0x2,
    SK_AIs = 0x3, // Foot or Car
    SK_Surfaces = 0x4,
    SK_Impacts = 0x5,
    SK_Weapons = 0x6, // Unverified
    SK_Persons = 0x7,
    SK_Projectiles = 0x8, // Unverified - very uncertain
    SK_Objects = 0x9,
    SK_Dashboard = 0xA,
    SK_Instruments = 0xB,
    SK_AnimDesc = 0xC,
    SK_SoundLists = 0xD,
    SK_Sounds = 0xE,
    SK_Particles = 0xF,
    SK_EventSequenceList = 0x10,
    SK_EmitterArray = 0x11, // EventSequences
    SK_Emitter = 0x12,      // EventFrames
    SK_SubEmitter = 0x13,   // Events
    SK_MiscData = 0x14,
    SK_Blasts = 0x15,
    SK_Trails = 0x16,
    SK_Item = 0x17,
    SK_Light = 0x18,
    SK_GeneralFloat = 0x19,
    SK_Motion = 0x1A,
    SK_DamageState = 0x1B,
    SK_Bushes = 0x1C,
    // Unhandled = 0x1D
    SK_Critter = 0x1E,
    SK_CritterStateWander = 0x1F,
    SK_CritterStateRest = 0x20,
    SK_CritterStateHit = 0x21,
    SK_CritterStateFlee = 0x22,
    SK_CritterStateAttack = 0x23,
    SK_CritterStateFollow = 0x24,
    SK_CritterStateMerge = 0x25,
    SK_CameraShake = 0x26,
};

struct ScriptDescriptor {
    short kind;
    short offset;
    short has_description;
    char description[32];
};

struct ScriptSection {
    short descriptor_count;
    short entry_count;
    short entry_len;
    ScriptDescriptor* descriptors;
    void* scripts;
};

class ScriptManager {
    ScriptSection sections[0x27];
    int unk1[100];
    int unk2[100];
    int unk3;
    int unk4;
    int unk5;
    int unk6;
    int unk7;
    int unk8;
    int unk9;
    char* raw;
    public:
    void Unknown();
    bool Read(char* buffer);
    void Clear();
    void* Lookup(short kind, const char* name, short* out_idx);
    bool InitScriptFields(unsigned short kind, char* raw);
};

bool LoadScripts(const char* path);
