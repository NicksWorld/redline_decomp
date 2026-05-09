#include "pack.h"

#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>

#include "globals.h"
#include "log.h"


class Unk {
    public:
    int ReadPack(const char* name);
};
// is a thiscall
// STUB: REDLINE 0x004aaae5
int Unk::ReadPack(const char* name) {
    return 0;
}

// GLOBAL: REDLINE 0x005c4078
Unk glob;

// FUNCTION: REDLINE 0x005523e1
bool LoadPack(const char* name, int unk1, int unk2) {
    if (glob.ReadPack(name) == false) {
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
