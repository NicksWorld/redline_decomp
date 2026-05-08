#pragma once

class Log {
    char filename[0x100];
    int unk;
    public:
    void Open(const char* filename, int unk);
    void Debug(const char* msg);
};
