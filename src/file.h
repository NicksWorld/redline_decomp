#pragma once

class FileContainer {
    public:
    char* data;
    unsigned int size;
    int cursor;
    int line_ending;
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

    int ReadTextAsset(const char* name);
    int ReadLine(unsigned int unk, char* out);

    int Read(const char* name, char* buf, int size);
    void ReadBytes(void* data, size_t len);
};

class ImageFileContainer : FileContainer {
    public:
    short height;
    short width;
    short max_palette_size;
    short cmap_len;
    short bpp;
    short alpha;
    short mips;
    char pad3[2];
    char* image_data;
    char* image_cmap;

    ImageFileContainer();
    ~ImageFileContainer();
    
    int LoadTGA(const char* path);
    int LoadBTF(const char* path);
    void FlipVertical();

    char* GetMip(short level);

    void ClearImageData();
};

void GetAssetFilesystemPath(const char* filename, const char* unk, char* out);
