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

// This functions, but is *far* from matching
// FUNCTION: REDLINE 0x0052dba9
bool ScriptManager::Read(char* buffer) {
    this->Clear();
    this->raw = buffer;
    short idx;
    for (idx = 0; idx < 0x27; ++idx) {
        this->sections[idx].descriptor_count = 0;
        this->sections[idx].entry_count = 0;
        this->sections[idx].scripts = NULL;
        this->sections[idx].descriptors = NULL;
    }
    char* raw = this->raw + 4;
    int version = *(int*)raw;
    if (version < 3) {
        g_Log.Debug("**ERROR - script file version not supported - get new file from server!");
        return false;
    }
    raw += 4;
    short section_count = *(short*)raw;
    char* data = raw + 4;
    for (idx = 0; idx < section_count; ++idx) {
        short section_id = *(short*)(data);
        data += 2;
        section_id = *(short*)(data);
        data += 2;
        short stride = *(short*)data;
        data += 2;
        this->sections[section_id].descriptor_count = *(short*)(data);
        data += 2;
        this->sections[section_id].descriptors = (ScriptDescriptor*)data;
        data += this->sections[section_id].descriptor_count * stride;
        this->sections[section_id].entry_len = *(short*)data;
        data += 2;
        this->sections[section_id].entry_count = *(short*)data;
        data += 2;
        this->sections[section_id].scripts = data;

        short i;
        for (i = 0; i < 39; ++i) {
            if (g_ScriptSizes[i][0] == section_id) {
                if (g_ScriptSizes[i][1] != this->sections[section_id].entry_len) {
                    g_Log.Debug("**ERROR - script file indicates a different size for a script category!");
                    return false;
                }
                break;
            }
        }
        if (i >= 39) {
            g_Log.Debug("**ERROR - no size entry in script structure size table that match");
            return false;
        }
        char* scripts = (char*)this->sections[section_id].scripts;
        data += this->sections[section_id].entry_len * this->sections[section_id].entry_count;
        for (short s = 0; s < this->sections[section_id].entry_count; ++s) {
            *(short*)(scripts + 16) = 0;
            for (short d = 0; d < this->sections[section_id].descriptor_count; ++d) {
                ScriptArray* arr;
                ScriptArray2* arr2;
                switch (this->sections[section_id].descriptors[d].kind) {
                    case 0xC:
                        arr = (ScriptArray*)(scripts + this->sections[section_id].descriptors[d].offset);
                        if (arr->count <= 0) {
                            arr->arr = NULL;
                        } else {
                            arr->arr = data;
                            data += 20 * arr->count;
                        }
                        break;
                    case 0xD: // Float array
                        arr = (ScriptArray*)(scripts + this->sections[section_id].descriptors[d].offset);
                        if (arr->count <= 0) {
                            arr->arr = NULL;
                        } else {
                            arr->arr = data;
                            data += 4 * arr->count;
                            if (((float*)arr->arr)[arr->count - 1] == 999.9) {
                                arr->count -= 1;
                            }
                        }
                        break;
                    case 0xE:
                        arr = (ScriptArray*)(scripts + this->sections[section_id].descriptors[d].offset);
                        if (arr->count <= 0) {
                            arr->arr = NULL;
                        } else {
                            arr->arr = data;
                            data += 12 * arr->count;
                        }
                        break;
                    case 0xF:
                        arr2 = (ScriptArray2*)(scripts + this->sections[section_id].descriptors[d].offset);
                        if (arr2->count <= 0) {
                            arr2->arr = NULL;
                        } else {
                            arr2->arr = data;
                            data += 32 * arr2->count;
                        }
                        break;
                    case 0x10:
                        arr2 = (ScriptArray2*)(scripts + this->sections[section_id].descriptors[d].offset);
                        if (arr2->count <= 0) {
                            arr2->arr = NULL;
                        } else {
                            arr2->arr = data;
                            data += 16 * arr2->count;
                        }
                        break;
                    case 0x17:
                        arr = (ScriptArray*)(scripts + this->sections[section_id].descriptors[d].offset);
                        if (arr->count <= 0) {
                            arr->arr = NULL;
                        } else {
                            arr->arr = data;
                            data += 40 * arr->count;
                        }
                        break;
                    case 0x19:
                        arr = (ScriptArray*)(scripts + this->sections[section_id].descriptors[d].offset);
                        if (arr->count <= 0) {
                            arr->arr = NULL;
                        } else {
                            arr->arr = data;
                            data += 152 * arr->count;
                        }
                        break;
                    default:
                        continue;
                }
            }
            scripts += this->sections[section_id].entry_len;
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
