#pragma once

class FileContainer {
    public:
    char* data;
    int size;
    int unk1;
    int unk2;
    int unk3;
    class ifstream* stream;
    char unk_pad[264];
    class ofstream* out; // Why?

    public:
    FileContainer();
    ~FileContainer();
    void Clear();
    int ReadOpen(const char* name);
    int ReadFile(const char* name);
    int ReadAsset(const char* name);

    int Read(const char* name, char* buf, int size);
};
