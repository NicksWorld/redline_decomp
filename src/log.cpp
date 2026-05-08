#include "log.h"

#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

// GLOBAL: REDLINE 0x00587578
int flags = 0x1a4;

// Stack is a bit scrambled, and inlining isn't applying on ofstream methods
// FUNCTION: REDLINE 0x004a8e90
void Log::Open(const char *filename, int truncate) {
    int open_flags;
    struct tm *tm;
    ofstream *stream;
    time_t t;
    if (filename != NULL && strlen(filename) != 0 && strlen(filename) < 0x100) {
        ;
        strcpy(this->filename, filename);

        stream = NULL;
        open_flags = ios::out | ios::binary;
        if (truncate != 0) {
            open_flags |= ios::trunc;
        } else {
            open_flags |= ios::app;
        }

        stream = new ofstream(filename, open_flags, flags);
        if (stream == NULL)
            return;
        if ((stream->rdstate() & 6) == 2)
            return;
        this->unk = 0;
        time(&t);
        tm = localtime(&t);

        char msg[128];
        strcpy(msg, asctime(tm));

        if (strlen(msg) > 2) {
            msg[strlen(msg) - 1] = '\0';
        }

        *stream << "// Program started on " << msg << "\r\n";

        if (stream) {
            delete stream;
        }
    }
}

// FUNCTION: REDLINE 0x004a94a1
void Log::Debug(const char *msg) {}
