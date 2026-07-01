#pragma once

class Log {
    char filename[0x100];
    int unk;
    public:
    void Open(const char* filename, int unk);
    void Print(const char* msg);
    void Debug(const char* msg);
    void DxErr(const char* msg, int err);
    void D3dErr(const char* msg, int err);

    private:
    void DxErrToString(int err, char* buf);
    void D3dErrToString(int err, char* buf);
};

void SomeDebug(const char* msg);
void Warn(const char *fmt, ...);
