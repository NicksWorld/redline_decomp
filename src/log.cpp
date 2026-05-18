#include "log.h"

#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

// GLOBAL: REDLINE 0x00587578
int g_protection = 0x1a4;

// Stack is a bit scrambled, and inlining isn't applying on ofstream methods
// FUNCTION: REDLINE 0x004a8e90
void Log::Open(const char *filename, int truncate) {
    int flags;
    ofstream *ostream;
    if (filename != NULL && strlen(filename) != 0 && strlen(filename) < 0x100) {
        strcpy(this->filename, filename);

        ostream = NULL;
        flags = ios::out | ios::binary;
        if (truncate != 0) {
            flags |= ios::trunc;
        } else {
            flags |= ios::app;
        }

        ostream = new ofstream(filename, flags, g_protection);
        if (ostream == NULL)
            goto cleanup;
        if (ostream->fail() == 2) {
            goto cleanup;
        }

        this->unk = 0;
        time_t time_base;
        time(&time_base);
        struct tm *time;
        time = localtime(&time_base);

        char msg[128];
        strcpy(msg, asctime(time));

        if (strlen(msg) > 2) {
            msg[strlen(msg) - 1] = '\0';
        }

        *ostream << "// Program started on " << msg << "\r\n";
    cleanup:
        if (ostream != NULL) {
            delete ostream;
            ostream = NULL;
        }
    }
}

// FUNCTION: REDLINE 0x004a94a1
void Log::Debug(const char *msg) {
    // Really, this method is completely empty
    MessageBoxA(NULL, msg, NULL, MB_ICONWARNING);
}
