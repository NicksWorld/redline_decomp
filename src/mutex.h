#pragma once

#include <windows.h>

class Mutex {
    HANDLE lock;
    char* name;
    int state;

public:
    Mutex(char* name);
    ~Mutex();
    bool Acquire(DWORD timeout);
    bool Release();
};
