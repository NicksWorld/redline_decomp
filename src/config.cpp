#include "config.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

// FUNCTION: REDLINE 0x0043d46b
Config::Config() {
    this->size1 = 256;
    this->arr1 = new int[this->size1];
    this->size2 = 256;
    this->arr2 = new int[this->size2];
    this->size3 = 3;
    this->arr3 = new int[this->size3];
    this->size4 = 13;
    this->arr4 = new int[this->size4];
    this->size5 = 9;
    this->arr5 = new int[this->size5];

    // Size variables appear to have been copied and pasted, but unused?
    // May not be an issue as it could be correct if dynamically resized?
    this->unused_size6 = 256;
    this->arr6 = new int[this->size2];
    this->unused_size7 = 3;
    this->arr7 = new int[this->size3];
    this->unused_size8 = 13;
    this->arr8 = new int[this->size4];
    this->unused_size9 = 9;
    this->arr9 = new int[this->size5];

    int i;
    for (i = 0; i < this->size1; ++i) {
        this->arr1[i] = 0;
    }
    for (i = 0; i < this->size2; ++i) {
        this->arr2[i] = -1;
        this->arr6[i] = -1;
    }
    for (i = 0; i < this->size3; ++i) {
        this->arr3[i] = -1;
        this->arr7[i] = -1;
    }
    for (i = 0; i < this->size4; ++i) {
        this->arr4[i] = -1;
        this->arr8[i] = -1;
    }
    for (i = 0; i < this->size5; ++i) {
        this->arr5[i] = -1;
        this->arr9[i] = -1;
    }
}

// STUB: REDLINE 0x00440517
int Config::ParseMapping(char* mapping) {
    return 0;
}
// STUB: REDLINE 0x004408BD
int Config::ParseOther(char* line) {
    return 0;
}

// FUNCTION: REDLINE 0x004403fe
int Config::ParseLine(char* line) {
    if (line == NULL) return -1;

    int len = strlen(line);

    // Detect comment lines (including preceding whitespace)
    for (int i = 0; i < len; ++i) {
        if (!isspace(line[i]) && line[i] != '#') {
            break;
        }
        if (line[i] == '#') return 0;
    }

    // Convert line to lowercase
    strlwr(line);

    // Skip preceding whitespace
    while (isspace(*line) && *line != NULL)
        ++line;

    // Blank line
    if (*line == NULL) return 0;

    if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p' && isspace(line[3])) {
        return ParseMapping(&line[4]);
    }

    return ParseOther(line);
}

// STUB: REDLINE 0x0043da38
int Config::Load() {
    FILE* file = fopen("redline.cfg", "r");
    if (file == NULL) {
        fclose(file);
        return -1;
    }

    char buf[128];
    int read = fread(buf, 1, 128, file);

    int remaining = read;
    int line_start = 0;
    int cursor = 0;
    while (read != -1 && read != 0) {
        while (remaining >= 0) {
            // Locate next newline
            while (cursor != read && buf[cursor] != '\n')
                ++cursor;
            if (cursor == read) {
                strncpy(buf, &buf[line_start], remaining);
                cursor = read - line_start;
                line_start = 0;
                break;
            }

            // Null terminate line, replacing \n
            buf[cursor] = 0; 

            // TODO: Parse config line
            return -1;
        }
    }

    return 1;
}
