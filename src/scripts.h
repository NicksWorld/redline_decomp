#pragma once

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
