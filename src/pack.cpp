#include "pack.h"

#include <fstream.h>
#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>

#include "globals.h"
#include "log.h"


// FUNCTION: REDLINE 0x00500930
PackFile::PackFile() {
    this->entries = NULL;
    this->entry_count = 0;
    for (int i = 0; i < 10; i++) {
        this->extensions[i].indicies = NULL;
    }
}

// FUNCTION: REDLINE 0x00500988
void PackFile::Clear() {
    size_t i;
    for (i = 0; i < this->entry_count; ++i) {
        if (this->entries[i].name != NULL) {
            delete this->entries[i].name;
            this->entries[i].name = NULL;
        }
    }
    if (this->entries != NULL) {
        delete this->entries;
        this->entries = NULL;
    }
    this->entry_count = 0;
    for (i = 0; i < 10; i++) {
        if (this->extensions[i].indicies != NULL) {
            delete this->extensions[i].indicies;
            this->extensions[i].indicies = NULL;
        }
    }
    this->filename[0] = '\0';
}

// FUNCTION: REDLINE 0x00500aa7
PackFile::~PackFile() {
    this->Clear();
}

// FUNCTION: REDLINE 0x00500aba
int PackFile::Read(const char* filename) {
    ifstream* stream;
    char path[256];
    strcpy(path, filename);
    this->Clear();
    strcpy(this->filename, path);

    stream = new ifstream(filename, 0xa0, g_protection);
    if (stream != NULL && stream->fail() != 2) {
        unsigned int version;
        stream->read((char*)&version, 4);
        if (version > 2) {
            char msg[128];
            sprintf(msg, "*Error - reading pack file. Unknown version %d", version);
            g_Log.Debug(msg);
            delete stream;
            return false;
        }

        stream->read((char*)&this->entry_count, 4);
        int off = 8;

        if(this->entry_count > 0 && this->entry_count < 50000) {
            this->entries = new Entry[this->entry_count]();
            unsigned int idx;
            for (idx = 0; idx < this->entry_count; ++idx) {
                this->entries[idx].name = NULL;
            }
            int unk = 8;
            if (1 < version) {
                unk += 4;
            }
            for (idx = 0; idx < this->entry_count; ++idx) {
                if (version >= 2) {
                    stream->read((char*)&this->entries[idx].timestamp, 4);
                } else {
                    this->entries[idx].timestamp = 0;
                }
                stream->read((char*)&this->entries[idx].size, 4);
                unsigned int name_len;
                stream->read((char*)&name_len, 4);
                if (0xff < name_len) {
                    return -1;
                }
                this->entries[idx].name = new char[name_len + 1];
                stream->read(this->entries[idx].name, name_len);
                this->entries[idx].name[name_len] = '\0';
                off += name_len + unk;
            }
            for (idx = 0; idx < this->entry_count; ++idx) {
                this->entries[idx].file_off = off;
                off += this->entries[idx].size;
                this->entries[idx].unk = 0;
            }
        } else {
            this->entry_count = 0;
        }
    }

    if (stream != NULL)
        delete stream;

    this->PopulateIndex();


    return this->entry_count;
}

// FUNCTION: REDLINE 0x00501381
void PackFile::PopulateIndex() {
    int ext_idx;
    strcpy(this->extensions[0].name, "geo");
    strcpy(this->extensions[1].name, "btf");
    strcpy(this->extensions[2].name, "tga");
    strcpy(this->extensions[3].name, "wav");
    strcpy(this->extensions[4].name, "anm");
    strcpy(this->extensions[5].name, "mot");
    strcpy(this->extensions[6].name, "skl");
    strcpy(this->extensions[7].name, "gld");
    strcpy(this->extensions[8].name, "ggr");
    strcpy(this->extensions[9].name, "");
    unsigned int idx;
    for (idx = 0; idx < 10; ++idx) {
        this->extensions[idx].entry_count = 0;
        if (this->extensions[idx].indicies != NULL) {
            delete this->extensions[idx].indicies;
            this->extensions[idx].indicies = NULL;
        }
    }
    for (idx = 0; idx < this->entry_count; ++idx) {
        ext_idx = FilenameToExtension(this->entries[idx].name);
        if (ext_idx >= 0) {
            this->extensions[ext_idx].entry_count += 1;
        }
    }
    for (idx = 0; idx < 10; ++idx) {
        if (this->extensions[idx].entry_count != 0) {
            this->extensions[idx].indicies = new int[this->extensions[idx].entry_count]();
            this->extensions[idx].entry_count = 0;
        }
    }
    for (idx = 0; idx < this->entry_count; ++idx) {
        int ent_idx = this->FilenameToExtension(this->entries[idx].name);
        if (ent_idx >= 0) {
            if (ent_idx == 7) {
                short unk = 0;
            }
            this->extensions[ent_idx].indicies[this->extensions[ent_idx].entry_count] = idx;
            this->extensions[ent_idx].entry_count += 1;
        }
    }
}

// FUNCTION: REDLINE 0x00501242
int PackFile::Find(const char* name) {
    char filename_buf[128];
    char* slash = strrchr(name, '\\');
    if (slash != NULL) {
        strcpy(filename_buf, slash + 1);
    } else {
        strcpy(filename_buf, name);
    }

    int ext = this->FilenameToExtension(filename_buf);
    if (ext >= 0) {
        for (unsigned int idx = 0; idx < this->extensions[ext].entry_count; ++idx) {
            int entry_idx = this->extensions[ext].indicies[idx];
            if (strcmpi(this->entries[entry_idx].name, filename_buf) == 0) {
                return entry_idx;
            }
        }
    }

    return -1;
}

// FUNCTION: REDLINE 0x00500ff9
int PackFile::Get(const char* name, char** buffer, int to_read) {
    int entry_idx = this->Find(name);
    if (entry_idx == -1) return 0;

    int entry_size = this->entries[entry_idx].size;
    if (to_read == 0) {
        *buffer = new char[entry_size + 1]();
    }
    if (buffer == NULL) return 0;

    ifstream* stream = new ifstream(this->filename, ios::binary | ios::nocreate, g_protection);
    if (stream == NULL || stream->fail() == ios::failbit) {
        if (stream != NULL)
            delete stream;
        stream = 0;
        return 0;
    }
    stream->seekg(this->entries[entry_idx].file_off, ios::beg);
    if (to_read > 0) {
        stream->read(*buffer, to_read);
    } else {
        stream->read(*buffer, entry_size);
    }
    delete stream;

    return entry_size;
}

// FUNCTION: REDLINE 0x0050165e
int PackFile::FilenameToExtension(const char* name) {
    char* dot = strrchr(name, '.');
    if (dot == NULL) {
        return -1;
    } else {
        dot += 1;
        int idx;
        for (idx = 0;idx < 9; ++idx) {
            if (strcmpi(dot, this->extensions[idx].name) == 0) {
                break;
            }
        }
        return idx;
    }
}

// FUNCTION: REDLINE 0x005011CD
int PackFile::Next(const char* ext, int start_idx) {
    for (unsigned int i = start_idx; i < this->entry_count; ++i) {
        char* dot = strrchr(this->entries[i].name, '.');
        if (dot && !strcmpi(dot + 1, ext))
            return i;
    }
    return -1;
}

// FUNCTION: REDLINE 0x00501355
void PackFile::EntryName(int idx, char* out) {
    strcpy(out, this->entries[idx].name);
}

// FUNCTION: REDLINE 0x004aac77
int AssetManager::Get(const char* name, char** buffer, int to_read) {
    int read = -1;
    for (unsigned int i = 0; i < this->pack_count; ++i) {
        read = this->packs[i].pack->Get(name, buffer, to_read);
        if (read > 0)
            break;
    }
    return read;
}

// FUNCTION: REDLINE 0x004AACD6
int AssetManager::GetEntryIdx(const char* name) {
    struct Locals {
        int entry_idx;
        unsigned int i;
    } l;
    l.entry_idx = -1;
    for ( l.i = 0; l.i < this->pack_count; ++l.i) {
        l.entry_idx = this->packs[l.i].pack->Find(name);
        if (l.entry_idx >= 0) {
            l.entry_idx += this->packs[l.i].base_entry_idx;
            break;
        }
    }
    return l.entry_idx;
}

// FUNCTION: REDLINE 0x004AAD40
int AssetManager::EntrySize(unsigned int idx) {
    if (!this->pack_count)
        return -1;
    unsigned int pack_idx = 0;
    for (pack_idx = 0; pack_idx < this->pack_count - 1; ++pack_idx) {
        if (idx < this->packs[pack_idx + 1].base_entry_idx) {
            break;
        }
    }

    int size = this->packs[pack_idx].pack->entries[idx - this->packs[pack_idx].base_entry_idx].size;
    return size;
}

// FUNCTION: REDLINE 0x004AADC9
int AssetManager::EntryTimestamp(unsigned int idx) {
    if (!this->pack_count)
        return -1;
    unsigned int pack_idx = 0;
    for (pack_idx = 0; pack_idx < this->pack_count - 1; ++pack_idx) {
        if (idx < this->packs[pack_idx + 1].base_entry_idx) {
            break;
        }
    }

    int timestamp = this->packs[pack_idx].pack->entries[idx - this->packs[pack_idx].base_entry_idx].timestamp;
    return timestamp;
}

// FUNCTION: REDLINE 0x004AAECD
int AssetManager::Next(const char* ext, unsigned int idx) {
    if (!this->pack_count)
        return -1;
    unsigned int pack_file = 0;
    for (pack_file = 0; pack_file < this->pack_count - 1; pack_file++) {
        if (idx < this->packs[pack_file + 1].base_entry_idx)
            break;
    }

    int i = idx - this->packs[pack_file].base_entry_idx;
    while (true) {
        i = this->packs[pack_file].pack->Next(ext, i);
        if (i >= 0) {
            i += this->packs[pack_file].base_entry_idx;
            break;
        }
        pack_file++;
        if (pack_file >= this->pack_count)
            break;
        i = 0;
    }

    return i;
}

// FUNCTION: REDLINE 0x004AAE52
void AssetManager::EntryName(unsigned int idx, char* out) {
    if (!this->pack_count)
        return;
    unsigned int pack_file = 0;
    for (pack_file = 0; pack_file < this->pack_count - 1; pack_file++) {
        if (idx < this->packs[pack_file + 1].base_entry_idx)
            break;
    }
    this->packs[pack_file].pack->EntryName(idx - this->packs[pack_file].base_entry_idx, out);
}

// FUNCTION: REDLINE 0x004aaae5
int AssetManager::ReadPack(const char* filename) {
    PackFile* pack = new PackFile();
    if(!pack->Read(filename)) {
        delete pack;
        return false;
    }

    PackIndex* packs = new PackIndex[this->pack_count + 1]();
    if (this->pack_count != 0) {
        memcpy(&packs->pack, &this->packs->pack, this->pack_count * 8);
        int prev_count = (this->packs[this->pack_count - 1].pack)->entry_count;
        packs[this->pack_count].base_entry_idx = prev_count
            + packs[this->pack_count - 1].base_entry_idx;
        delete this->packs;
    } else {
        packs[this->pack_count].base_entry_idx = 0;
    }
    this->packs = packs;
    this->packs[this->pack_count].pack = pack;
    this->pack_count++;

    return true;
}

// GLOBAL: REDLINE 0x005c4078
AssetManager g_Assets;

// FUNCTION: REDLINE 0x005523e1
bool LoadPack(const char* name, int unk1, int unk2) {
    if (g_Assets.ReadPack(name) == false) {
        return false;
    }

    SYSTEMTIME system_time;
    struct _stat buffer;
    int exists = _stat(name, &buffer);
    if (exists == 0) {
        if ((unk2 > 0 && unk1 > 0) && (unk1 != buffer.st_mtime || unk2 != buffer.st_size)) {
            return false;
        }
        FILETIME last_write_local;
        HANDLE file = CreateFileA(name, GENERIC_READ, 1, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != (HANDLE)-1) {
            FILETIME last_write;
            GetFileTime(file, NULL, NULL, &last_write);
            FileTimeToLocalFileTime(&last_write, &last_write_local);
            FileTimeToSystemTime(&last_write_local, &system_time);
            CloseHandle(file);

            char msg[128];
            sprintf(msg, "Pack: %s  (Date: %d/%02d/%02d %d:%02d:%02d  Size: %d)", name, system_time.wMonth, system_time.wDay, system_time.wYear, system_time.wHour, system_time.wMinute, system_time.wSecond, buffer.st_size);
            g_Log.Debug(msg);
        }
    }

    return true;
}

// FUNCTION: REDLINE 0x00417A7E
bool AssetInfo(const char* name, int* timestamp, int* size) {
    *timestamp = 0;
    *size = 0;
    bool res = false;
    if (g_unk == 0) {
        struct _stat st;
        int stat_res = _stat(name, &st);
        if (stat_res == 0) {
            // FIXME: This is backwards in the original
            *size = st.st_mtime;
            *timestamp = st.st_size;
            res = true;
        }
    } else {
        int idx = g_Assets.GetEntryIdx(name);
        if (idx >= 0) {
            *size = g_Assets.EntrySize(idx);
            *timestamp = g_Assets.EntryTimestamp(idx);
            res = true;
        }
    }
    return res;
}
