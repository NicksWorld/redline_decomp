#include "scripts.h"

#include <windows.h>
#include <fstream.h>
#include <stdio.h>
#include <winuser.h>

#include "file.h"
#include "log.h"
#include "globals.h"


// STUB: REDLINE 0x0052da40
void ScriptManager::Unknown() {
    for (short section_idx = 0; section_idx < 0x27; ++section_idx) {
        for (short script_idx = 0; script_idx < this->sections[section_idx].entry_count; ++script_idx) {
            *(short*)((char*)this->sections[section_idx].scripts + (script_idx * this->sections[section_idx].entry_len) + 12) = 0;
        }
    }
}

// FUNCTION: REDLINE 0x0052dacd
void ScriptManager::Clear() {
    if (this->raw != NULL) {
        delete this->raw;
        this->raw = NULL;
    }
    for (short i = 0; i < 100; ++i) {
        this->unk1[i] = 0;
        this->unk2[i] = 0;
    }
    this->unk3 = 0;
    this->unk4 = 0;
    this->unk5 = 0;
    this->unk6 = 0;
    this->unk7 = 0;
    this->unk8 = 0;
    this->unk9 = 0;
}

struct ScriptFile {
    int file_length;
    int version;
    short section_count;
};

// GLOBAL: REDLINE 0x005977f0
short g_ScriptSizes[39][2] = {
    {0x0000, 0x0528},
    {0x0001, 0x0036},
    {0x0002, 0x0032},
    {0x0003, 0x0084},
    {0x0004, 0x00B8},
    {0x0005, 0x005C},
    {0x0006, 0x0368},
    {0x0007, 0x0354},
    {0x0008, 0x0210},
    {0x0009, 0x0158},
    {0x000A, 0x0094},
    {0x000B, 0x0054},
    {0x000C, 0x0060},
    {0x000D, 0x001C},
    {0x000E, 0x0050},
    {0x000F, 0x0100},
    {0x0010, 0x001C},
    {0x0011, 0x0020},
    {0x0012, 0x0030},
    {0x0013, 0x0094},
    {0x0014, 0x006C},
    {0x0015, 0x0050},
    {0x0016, 0x0024},
    {0x0017, 0x00FC},
    {0x0018, 0x0050},
    {0x0019, 0x0018},
    {0x001A, 0x0068},
    {0x001B, 0x0020},
    {0x001C, 0x0054},
    {0x001D, 0x0168},
    {0x001E, 0x00E4},
    {0x001F, 0x0060},
    {0x0020, 0x0048},
    {0x0021, 0x0012},
    {0x0022, 0x0060},
    {0x0023, 0x0080},
    {0x0024, 0x0060},
    {0x0025, 0x0012},
    {0x0026, 0x0024},
};

struct ScriptArray {
    short count;
    void* arr;
};

struct ScriptArray2 {
    void* arr;
    short count;
};

// FUNCTION: REDLINE 0x0052dba9
bool ScriptManager::Read(char* buffer) {
    struct Locals {
        ScriptArray* arr;
        ScriptArray* arr2;
        ScriptArray2* arr3;
        ScriptArray2* arr4;
        ScriptArray* arr5;
        ScriptArray* arr6;
        ScriptArray* arr7;
        short desc_idx;
        char _pad1[2];
        short sect_idx;
        char _pad2[2];
        char* scripts;
        char* data;
        short section_count;
        char _pad3[2];
        short idx;
        char _pad4[2];
        short section_id;
        char _pad5[2];
        short stride;
        char _pad6[2];
        int version;
    } l;
    this->Clear();
    this->raw = buffer;
    for (l.idx = 0; l.idx < 0x27; ++l.idx) {
        this->sections[l.idx].descriptor_count = 0;
        this->sections[l.idx].entry_count = 0;
        this->sections[l.idx].scripts = NULL;
        this->sections[l.idx].descriptors = NULL;
    }
    l.data = this->raw + 4;
    l.version = *(int*)l.data;
    if (l.version < 3) {
        g_Log.Debug("**ERROR - script file version not supported - get new file from server!");
        return false;
    }
    l.data += 4;
    l.section_count = *(short*)l.data;
    l.data = l.data + 4;
    for (l.idx = 0; l.idx < l.section_count; ++l.idx) {
        l.section_id = *(short*)(l.data);
        l.data += 2;
        l.section_id = *(short*)(l.data);
        l.data += 2;
        l.stride = *(short*)l.data;
        l.data += 2;
        this->sections[l.section_id].descriptor_count = *(short*)l.data;
        l.data += 2;
        this->sections[l.section_id].descriptors = (ScriptDescriptor*)l.data;
        l.data += l.stride * this->sections[l.section_id].descriptor_count;
        this->sections[l.section_id].entry_len = *(short*)l.data;
        l.data += 2;
        this->sections[l.section_id].entry_count = *(short*)l.data;
        l.data += 2;
        this->sections[l.section_id].scripts = l.data;
        for (l.sect_idx = 0; l.sect_idx < 39; ++l.sect_idx) {
            if (g_ScriptSizes[l.sect_idx][0] == l.section_id) {
                if (g_ScriptSizes[l.sect_idx][1] != this->sections[l.section_id].entry_len) {
                    g_Log.Debug("**ERROR - script file indicates a different size for a script category!");
                    return false;
                }
                break;
            }
        }
        if (l.sect_idx >= 39) {
            g_Log.Debug("**ERROR - no size entry in script structure size table that match");
            return false;
        }
        l.scripts = (char*)this->sections[l.section_id].scripts;
        l.data += this->sections[l.section_id].entry_count * this->sections[l.section_id].entry_len;
        for ( l.sect_idx = 0; l.sect_idx < this->sections[l.section_id].entry_count; ++l.sect_idx) {
            *(short*)(l.scripts + 16) = 0;
            for ( l.desc_idx = 0; l.desc_idx < this->sections[l.section_id].descriptor_count; ++l.desc_idx) {
                switch (this->sections[l.section_id].descriptors[l.desc_idx].kind) {
                    case 0x19:
                        l.arr7 = (ScriptArray*)(l.scripts + this->sections[l.section_id].descriptors[l.desc_idx].offset);
                        if (l.arr7->count > 0) {
                            l.arr7->arr = l.data;
                            l.data += 152 * l.arr7->count;
                        } else {
                            l.arr7->arr = NULL;
                        }
                        break;
                    case 0x17:
                        l.arr6 = (ScriptArray*)(l.scripts + this->sections[l.section_id].descriptors[l.desc_idx].offset);
                        if (l.arr6->count > 0) {
                            l.arr6->arr = l.data;
                            l.data += 40 * l.arr6->count;
                        } else {
                            l.arr6->arr = NULL;
                        }
                        break;
                    case 0xE:
                        l.arr5 = (ScriptArray*)(l.scripts + this->sections[l.section_id].descriptors[l.desc_idx].offset);
                        if (l.arr5->count > 0) {
                            l.arr5->arr = l.data;
                            l.data += 12 * l.arr5->count;
                        } else {
                            l.arr5->arr = NULL;
                        }
                        break;
                    case 0x10:
                        l.arr4 = (ScriptArray2*)(l.scripts + this->sections[l.section_id].descriptors[l.desc_idx].offset);
                        if (l.arr4->count > 0) {
                            l.arr4->arr = l.data;
                            l.data += 16 * l.arr4->count;
                        } else {
                            l.arr4->arr = NULL;
                        }
                        break;
                    case 0xF:
                        l.arr3 = (ScriptArray2*)(l.scripts + this->sections[l.section_id].descriptors[l.desc_idx].offset);
                        if (l.arr3->count > 0) {
                            l.arr3->arr = l.data;
                            l.data += 32 * l.arr3->count;
                        } else {
                            l.arr3->arr = NULL;
                        }
                        break;
                    case 0xC:
                        l.arr2 = (ScriptArray*)(l.scripts + this->sections[l.section_id].descriptors[l.desc_idx].offset);
                        if (l.arr2->count > 0) {
                            l.arr2->arr = l.data;
                            l.data += 20 * l.arr2->count;
                        } else {
                            l.arr2->arr = NULL;
                        }
                        break;
                    case 0xD: // Float array
                        l.arr = (ScriptArray*)(l.scripts + this->sections[l.section_id].descriptors[l.desc_idx].offset);
                        if (l.arr->count > 0) {
                            l.arr->arr = l.data;
                            l.data += 4 * l.arr->count;
                            if (((float*)l.arr->arr)[l.arr->count - 1] == 999.9) {
                                l.arr->count -= 1;
                            }
                        } else {
                            l.arr->arr = NULL;
                        }
                        break;
                }
            }
            l.scripts += this->sections[l.section_id].entry_len;
        }
    }

    return true;
}

// STUB: REDLINE 0x0052e6bb
bool ScriptManager::InitScriptFields(unsigned short kind, char* raw) {
    int x = kind + 1;
    return true;
}

struct ScriptInitializer {
    bool (*func)(void*);
    int unk;
};

ScriptInitializer g_ScriptInitializers[39];

struct ScriptBase {
    char name[16];
    short flags;
};

// FUNCTION: REDLINE 0x0052e258
void* ScriptManager::Lookup(short kind, const char* name, short* out_idx) {
    char* script = (char*)this->sections[kind].scripts;
    for (short i = 0; i < this->sections[kind].entry_count; ++i) {
        if (!strcmpi(script, name)) {
            if (out_idx != NULL)
                *out_idx = i;
            if (((*(short*)(script + 0x10)) & 1) == 0 ) {
                if (!this->InitScriptFields((unsigned short)kind, (char*)script)) {
                    char buffer[200];
                    sprintf(buffer, "*Error: init script: %s", name);
                    g_Log.Debug(buffer);
                    if (out_idx)
                        *out_idx = -1;
                    return NULL;
                }
                if (g_ScriptInitializers[kind].func != NULL && !(g_ScriptInitializers[kind].func(script))) {
                    return NULL;
                }
            }
            return script;
        }
        script += this->sections[kind].entry_len;
    }
    if (out_idx)
        *out_idx = -1;
    return NULL;
}

// GLOBAL: REDLINE 0x005cc9a0
ScriptManager g_Scripts;

// The diff is *massive* here, but should be functionally equivalent.
// Something about the memcpy call and its preamble cause issues with matching
// FUNCTION: REDLINE 0x0052effb
bool LoadScripts(const char* path) {
    g_Scripts.Unknown(); // Clear pointers?
    char* buf = NULL;
    FileContainer* container = new FileContainer();
    int res;
    if (g_unk == 2) {
        res = container->ReadFile(path);
    } else {
        res = container->ReadAsset(path);
    }
    
    if (res != 0) {
        buf = new char[container->size + 1]();
        memcpy(buf, container->data, container->size);
        res = g_Scripts.Read(buf);
    }

    delete container;
    return res != 0;
}
