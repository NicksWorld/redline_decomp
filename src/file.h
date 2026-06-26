#pragma once

class FileContainer {
    public:
    char* data;
    int size;
    int cursor;
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
    void ReadBytes(void* data, size_t len);
};

// TODO: Remove, for reference only
// struct TgaHeader {
//     char  idlength; // 0
//     char  cmap_type; // 1
//     char  data_type; // 2
//     short cmap_origin; // 3
//     short cmap_length; // 5
//     char  cmap_depth; // 7
//     short x_origin; // 8
//     short y_origin; // 10
//     short width; // 12
//     short height; // 14
//     char  bpp; // 16
//     char  desc; // 17
// };

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
