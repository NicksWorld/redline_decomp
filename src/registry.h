#pragma once

bool WriteRegistry(const char *key, const char *out);
bool ReadRegistry(const char *keyname, const char *default_val, char *out, const char *path);
