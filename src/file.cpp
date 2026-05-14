#include "file.h"
#include "globals.h"
#include "pack.h"
#include "log.h"
#include <fstream.h>

// FUNCTION: REDLINE 0x004169d0
FileContainer::FileContainer() {
    this->stream = NULL;
    this->out = NULL;
    this->data = NULL;
    this->unk2 = 0;
    this->size = 0;
    this->unk3 = 0;
}

// FUNCTION: REDLINE 0x00416a1c
FileContainer::~FileContainer() {
    if (this->stream != NULL)
        delete this->stream;
    if (this->out != NULL)
        delete this->out;
    this->Clear();
}

// FUNCTION: REDLINE 0x00417036
void FileContainer::Clear() {
    if (this->unk3 == 0) {
        if (this->data != NULL) {
            delete this->data;
            this->data = NULL;
        }
    } else {
        this->data = NULL;
    }
}

// FUNCTION: REDLINE 0x00416b7f
int FileContainer::ReadOpen(const char* name) {
    if (this->out != NULL) {
        delete this->out;
        this->out = NULL;
    }
    if (this->stream != NULL) {
        delete this->stream;
        this->stream = NULL;
    }
    this->stream = new ifstream(name, ios::binary | ios::nocreate, g_protection);
    if (this->stream == NULL) return false;
    if (this->stream->fail() == ios::failbit) return false;
    return true;
}

// FUNCTION: REDLINE 0x00417844
int FileContainer::ReadFile(const char* name) {
    this->Clear();
    this->unk2 = 0;
    if (this->ReadOpen(name) == 0) return false;

    this->stream->seekg(0, ios::end);
    int len = this->stream->tellg();
    if (len < 1) return false;

    this->size = len;
    this->data = new char[this->size + 1]();
    this->unk3 = 0;
    if (this->data == NULL) return false;

    this->stream->seekg(0, ios::beg);
    this->stream->read(this->data, this->size);
    this->stream->close();
    this->unk1 = 0;
    return true;
}

// FUNCTION: REDLINE 0x004175ea
int FileContainer::ReadAsset(const char* name){
    this->Clear();
    this->unk2 = 0;
    if (g_unk == 0) {
        if(this->ReadOpen(name) == 0) {
            return false;
        }

        this->stream->seekg(0, ios::end);
        int len = this->stream->tellg();
        if (len < 1) {
            return false;
        }

        this->size = len;
        this->data = new char[this->size + 1]();
        this->unk3 = 0;
        if (this->data == NULL){
            return false;
        }

        this->stream->seekg(0, ios::beg);
        this->stream->read(this->data, this->size);
        this->stream->close();
    } else {
        this->size = g_Assets.Get(name, this, 0);
        if (this->size == 0) {
            g_Log.Debug("No asset found");
            return 0;
        }
        this->unk3 = 0;
    }

    this->unk1 = 0;
    return true;
}
