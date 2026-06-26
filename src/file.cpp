#include "file.h"
#include "globals.h"
#include "pack.h"
#include "log.h"
#include <fstream.h>

// FUNCTION: REDLINE 0x004169d0
FileContainer::FileContainer() {
    this->stream = NULL;
    this->out = NULL;
    this->data = NULL;
    this->unk2 = 0;
    this->size = 0;
    this->unk3 = 0;
}

// FUNCTION: REDLINE 0x00416a1c
FileContainer::~FileContainer() {
    if (this->stream != NULL)
        delete this->stream;
    if (this->out != NULL)
        delete this->out;
    this->Clear();
}

// FUNCTION: REDLINE 0x00417036
void FileContainer::Clear() {
    if (this->unk3 == 0) {
        if (this->data != NULL) {
            delete this->data;
            this->data = NULL;
        }
    } else {
        this->data = NULL;
    }
}

// FUNCTION: REDLINE 0x00416b7f
int FileContainer::ReadOpen(const char* name) {
    if (this->out != NULL) {
        delete this->out;
        this->out = NULL;
    }
    if (this->stream != NULL) {
        delete this->stream;
        this->stream = NULL;
    }
    this->stream = new ifstream(name, ios::binary | ios::nocreate, g_protection);
    if (this->stream == NULL) return false;
    if (this->stream->fail() == ios::failbit) return false;
    return true;
}

// FUNCTION: REDLINE 0x00417844
int FileContainer::ReadFile(const char* name) {
    this->Clear();
    this->unk2 = 0;
    if (this->ReadOpen(name) == 0) return false;

    this->stream->seekg(0, ios::end);
    int len = this->stream->tellg();
    if (len < 1) return false;

    this->size = len;
    this->data = new char[this->size + 1]();
    this->unk3 = 0;
    if (this->data == NULL) return false;

    this->stream->seekg(0, ios::beg);
    this->stream->read(this->data, this->size);
    this->stream->close();
    this->cursor = 0;
    return true;
}

// FUNCTION: REDLINE 0x004175ea
int FileContainer::ReadAsset(const char* name){
    this->Clear();
    this->unk2 = 0;
    if (g_unk == 0) {
        if(this->ReadOpen(name) == 0) {
            return false;
        }

        this->stream->seekg(0, ios::end);
        int len = this->stream->tellg();
        if (len < 1) {
            return false;
        }

        this->size = len;
        this->data = new char[this->size + 1]();
        this->unk3 = 0;
        if (this->data == NULL){
            return false;
        }

        this->stream->seekg(0, ios::beg);
        this->stream->read(this->data, this->size);
        this->stream->close();
    } else {
        this->size = g_Assets.Get(name, &this->data, 0);
        if (this->size == 0) {
            return 0;
        }
        this->unk3 = 0;
    }

    this->cursor = 0;
    return true;
}

// FUNCTION: REDLINE 0x00417709
int FileContainer::Read(const char* name, char* buf, int size) {
    int asset_size;
    int timestamp;
    if (!AssetInfo(name, &timestamp, &asset_size))
        return false;
    // FIXME: This was likely a mistake in the original
    if (size > timestamp)
        return false;

    if (g_unk == 0) {
        if (!this->ReadOpen(name))
            return false;
        this->stream->read(buf, size);
        this->stream->close();
    } else {
        if (!g_Assets.Get(name, &buf, size))
            return false;
    }
    this->Clear();
    return true;
}


// FUNCTION: REDLINE 0x00417B1A
void GetAssetFilesystemPath(const char* filename, const char* unk, char* out) {
    struct Locals {
        char path[512];
        char *last_slash;
        char *dot;
        char dir[64];
        char ext[4];
        bool unpacked;
    } l;
    l.unpacked = false;
    *l.dir = 0;
    l.dot = strrchr(filename, '.');
    if (l.dot) {
        memcpy(l.ext, l.dot + 1, 3);
        l.ext[3] = NULL;
        strupr(l.ext);
        if (!strcmp(l.ext, "GEO") || !strcmp(l.ext, "GLD")) {
            strcpy(l.dir, "geo\\");
        } else if (!strcmp(l.ext, "BTF") || !strcmp(l.ext, "TGA")) {
            strcpy(l.dir, "textures\\");
        } else if (!strcmp(l.ext, "RSG")) {
            strcpy(l.dir, "saved games\\");
            l.unpacked = 1;
        } else if (!strcmp(l.ext, "WAV")) {
            strcpy(l.dir, "wav\\");
        } else if (!strcmp(l.ext, "WLD")) {
            strcpy(l.dir, "wld\\");
        } else if (!strcmp(l.ext, "ANM")) {
            strcpy(l.dir, "anm\\");
        } else if (!strcmp(l.ext, "GGR")) {
            strcpy(l.dir, "geo\\");
        } else if (!strcmp(l.ext, "MOT") || !strcmp(l.ext, "SKL")) {
            strcpy(l.dir, "motion\\");
        } else if (!strcmp(l.ext, "THG")) {
            // ???
        } else if (!strcmp(l.ext, "EVT")) {
            strcpy(l.dir, "events\\");
        } else if (!strcmp(l.ext, "BMP")) {
            strcpy(l.dir, "textures\\");
        }
    }

    strcpy(out, unk);
    if (out[strlen(out) - 1] != '\\')
        strcat(out, "\\");
    strcat(out, l.dir);
    strcat(out, filename);
    if (l.unpacked) {
        strcpy(l.path, out);
        l.last_slash = strrchr(l.path, '\\');
        if (l.last_slash)
            *l.last_slash = NULL;

        // Create directory if non-existent (original passes a zero-length string instead of dir path)
        if (GetFileAttributesA(l.last_slash) == -1)
            CreateDirectoryA(l.path, NULL);
    }
}


// FUNCTION: REDLINE 0x0041799F
void FileContainer::ReadBytes(void* data, size_t len) {
    memcpy(data, &this->data[this->cursor], len);
    this->cursor += len;
}

// FUNCTION: REDLINE 0x004915D0
ImageFileContainer::ImageFileContainer() {
    image_data = NULL;
    image_cmap = NULL;
}

// FUNCTION: REDLINE 0x00491600
ImageFileContainer::~ImageFileContainer() {
    this->ClearImageData();
}

// FUNCTION: REDLINE 0x00491648
void ImageFileContainer::ClearImageData() {
    if (this->image_data) {
        delete this->image_data;
        this->image_data = NULL;
    }
    if (this->image_cmap) {
        delete this->image_cmap;
        this->image_cmap = NULL;
    }

    this->mips = 0;
}

// FUNCTION: REDLINE 0x004916C3
int ImageFileContainer::LoadTGA(const char* path) {
    if (!this->ReadAsset(path))
        return 0;

    char header[18];
    this->ReadBytes(&header, sizeof(header));
    // this->pad[10] = 0;
    this->height = (header[14] << 8) + header[15];
    this->width = (header[12] << 8) + header[13];
    this->bpp = header[16];
    switch(header[16]) {
        case 32:
            this->ClearImageData();
            this->image_data = new char[4 * this->height * this->width];
            if (!this->image_data)
                return 0;
            this->ReadBytes(this->image_data, 4 * this->height * this->width);
            this->max_palette_size = 0;
            this->cmap_len = 0;
            this->alpha = 1;
            return 1;
        case 24:
            this->ClearImageData();
            this->image_data = new char[3 * this->height * this->width];
            if (!this->image_data)
                return 0;
            this->ReadBytes(this->image_data, 4 * this->height * this->width);
            this->max_palette_size = 0;
            this->cmap_len = 0;
            this->alpha = 0;
            return 1;
        case 8:
            this->ClearImageData();
            this->max_palette_size = 256;
            this->cmap_len = (header[5] << 8) + header[6];
            this->alpha = 0;
            if (this->cmap_len > 0) {
                this->image_cmap = new char[4 * this->cmap_len];
                char* cmap = this->image_cmap;
                for (int i = 0; i < this->cmap_len; ++i) {
                    int* color = (int*)cmap;
                    cmap += 4;
                    this->ReadBytes(color, 3);
                }
            } else {
                this->image_cmap = new char[4 * 256];
                char *color = this->image_cmap;
                // FIXME: This doesn't do anything... as we just confirmed
                // cmap_len <= 0
                for (int j = 0; j < this->cmap_len; ++j) {
                    color[0] = 0;
                    color[1] = 0;
                    color[2] = -1;
                    color += 4;
                }
            }
            this->image_data = new char[this->height * this->width];
            this->ReadBytes(this->image_data, this->height * this->width);
            return 1;
    }
    return 0;
}

// FUNCTION: REDLINE 0x00491A62
int ImageFileContainer::LoadBTF(const char* path) {
    if (!this->ReadAsset(path))
        return 0;
    short version;
    this->ReadBytes(&version, 2);
    if (version > 2)
        return 0; // Unsupported
    this->ClearImageData();

    this->ReadBytes(&this->height, 2);
    this->ReadBytes(&this->width, 2);
    this->ReadBytes(&this->bpp, 2);
    this->ReadBytes(&this->cmap_len, 2);
    this->ReadBytes(&this->mips, 2);
    this->alpha = 0;

    int image_bytes = this->height * this->width;
    int mipsize = image_bytes;
    for (short i = 1; i < this->mips; ++i) {
        mipsize /= 4;
        image_bytes += mipsize;
    }
    image_bytes *= this->bpp / 8;
    this->image_data = new char[image_bytes];
    switch (this->bpp) {
        case 32:
            this->ReadBytes(this->image_data, image_bytes);
            this->max_palette_size = 0;
            this->cmap_len = 0;
            this->alpha = 1;
            return 1;
        case 24:
            if (!this->image_data)
                return 0;
            this->ReadBytes(this->image_data, image_bytes);
            this->max_palette_size = 0;
            this->cmap_len = 0;
            this->alpha = 0;
            return 1;
        case 8:
            this->max_palette_size = 256;
            this->alpha = 0;
            this->image_cmap = new char[4 * this->cmap_len];
            char* color_iter = this->image_cmap;
            for (int j = 0; j < this->cmap_len; ++j) {
                char* color = color_iter;
                color_iter += 4;
                this->ReadBytes(color, 3);
            }
            this->ReadBytes(this->image_data, image_bytes);
            return 1;
    }
    return 0;
}

// FUNCTION: REDLINE 0x004920D6
void ImageFileContainer::FlipVertical() {
    // TODO: Does this really only flip mip0, and none of the others?
    if (!this->image_data)
        return;

    short bytes_per_pixel = 1;
    if (this->bpp == 24) {
        bytes_per_pixel = 3;
    } else if (this->bpp == 32) {
        bytes_per_pixel = 4;
    }
    int row_bytes = this->width * bytes_per_pixel;
    char* row_tmp = new char[row_bytes];
    short bottom = 0;
    for (short top = this->height - 1; bottom < top; --top) {
        memcpy(row_tmp, &this->image_data[row_bytes * bottom], row_bytes);
        memcpy(&this->image_data[row_bytes * bottom], &this->image_data[row_bytes * top], row_bytes);
        memcpy(&this->image_data[row_bytes * top], row_tmp, row_bytes);
        ++bottom;
    }
    delete[] row_tmp;
}

// FUNCTION: REDLINE 0x004931E8
char* ImageFileContainer::GetMip(short level) {
    if (this->mips < level)
        return 0;
    int off = 0;
    int level_size = (this->bpp / 8) * this->width * this->height;
    for (int i = 0; i < level; ++i) {
        off += level_size;
        level_size /= 4;
    }
    return &this->image_data[off];
}
