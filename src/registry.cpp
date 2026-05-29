#include "registry.h"
#include <windows.h>

#include "globals.h"

// FUNCTION: REDLINE 0x0052AE13
bool WriteRegistry(const char *key, const char *out) {
    char key_class[12];
    ULONG disposition;

    HKEY reg = NULL;
    LONG status =
        RegCreateKeyExA(HKEY_LOCAL_MACHINE, g_registryKey, 0, key_class, 0,
                        0xF003F, NULL, &reg, &disposition);
    if (!status) {
        status = RegSetValueExA(reg, key, 0, 1, (unsigned char *)out, strlen(out) + 2);
        if (!status) {
            RegCloseKey(reg);
            return true;
        }
    }
    return false;
}

// FUNCTION: REDLINE 0x0052AD10
bool ReadRegistry(const char *keyname, const char *default_val, char *out, const char *path) {
    HKEY key = NULL;
    unsigned char buf[128];
    unsigned long buf_len = 128;
    buf[0] = 0;
    const char *subkey = g_registryKey;
    HKEY base = HKEY_LOCAL_MACHINE;
    if (path) {
        subkey = path;
        base = HKEY_CURRENT_USER;
    }
    LONG res = RegOpenKeyExA(base, subkey, 0, 0xF003F, &key);
    if (!res) {
        res = RegQueryValueExA(key, keyname, NULL, NULL, buf, &buf_len);
    }
    if (res == 0) {
        if (key)
            RegCloseKey(key);
        strcpy(out, (char *)buf);
        return true;
    }
    strcpy(out, default_val);
    if (path)
        return false;
    else {
        return WriteRegistry(keyname, out);
    }
}
