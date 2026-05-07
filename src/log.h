#pragma once

class Log {
    char filename[0x100];
    int unk;
    public:
    void Open(char* filename, int unk);
    void Debug(char* msg);
};
