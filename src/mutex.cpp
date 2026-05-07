#include "mutex.h"

// FUNCTION: REDLINE 0x004d2970
Mutex::Mutex(char *name) {
    this->state = 0;
    if (name != NULL) {
        this->name = new char[strlen(name) + 1];
        strcpy(this->name, name);
    } else {
        this->name = NULL;
    }
    this->lock = NULL;
    this->lock = CreateMutexA(0, false, this->name);
}

// FUNCTION: REDLINE 0x004d29f4
Mutex::~Mutex() {
    if (this->state != 0) {
        this->Release();
    }
    if (this->name != NULL) {
        delete this->name;
    }
    CloseHandle(this->lock);
}

// FUNCTION: REDLINE 0x004d2a3c
BOOL Mutex::Acquire(DWORD timeout) {
    DWORD res = WaitForSingleObject(this->lock, timeout);
    if (res == 0) {
        this->state = 1;
        return false;
    } else {
        return true;
    }
}

// FUNCTION: REDLINE 0x004d2a77
BOOL Mutex::Release() {
    this->state = 0;
    BOOL res = ReleaseMutex(this->lock);
    if (res == 1)
        return false;
    return true;
}
