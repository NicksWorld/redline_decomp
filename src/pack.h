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
    int Get(const char* name, FileContainer* container, int flags);
};

class PackIndex {
    public:
    PackFile* pack;
    int base_entry_idx;
};

class AssetManager {
    unsigned int pack_count;
    PackIndex* packs;
    public:
    int ReadPack(const char* name);
    int Get(const char* name, FileContainer* container, int to_read);
};

bool LoadPack(const char* name, int unk1, int unk2);
