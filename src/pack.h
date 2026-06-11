#pragma once

#include "file.h"

struct Entry {
    char* name;
    int size;
    int timestamp;
    int file_off;
    int unk;
};

struct PackExtension {
    char name[4];
    unsigned int entry_count;
    int* indicies;
};

class PackFile {
    public:
    char filename[256];
    unsigned int entry_count;
    Entry* entries;
    PackExtension extensions[10];

    public:
    PackFile();
    ~PackFile();
    int Read(const char* filename);
    void Clear();
    int FilenameToExtension(const char* name);
    void PopulateIndex();
    int Find(const char* name);
    int Get(const char* name, char** buffer, int flags);
    int Next(const char* ext, int start_idx);
    void EntryName(int idx, char* out);
};

class PackIndex {
    public:
    PackFile* pack;
    unsigned int base_entry_idx;
};

class AssetManager {
    unsigned int pack_count;
    PackIndex* packs;
    public:
    int ReadPack(const char* name);
    int Get(const char* name, char** buffer, int to_read);
    int GetEntryIdx(const char* name);
    int EntrySize(unsigned int idx);
    int EntryTimestamp(unsigned int idx);

    int Next(const char* ext, unsigned int idx);
    void EntryName(unsigned int idx, char* out);
};

bool LoadPack(const char* name, int unk1, int unk2);
bool AssetInfo(const char* name, int* timestamp, int* size);
