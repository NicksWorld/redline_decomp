#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "keybinds.h"
#include "log.h"

// GLOBAL: REDLINE 0x005a55dc
Config *g_Config;

// FUNCTION: REDLINE 0x0043d46b
Config::Config() {
    this->conf_size = 256;
    this->conf_values = new Value *[this->conf_size];
    this->keybind_size = 256;
    this->keybinds_foot = new int[this->keybind_size];
    this->mousebind_size = 3;
    this->mousebinds_foot = new int[this->mousebind_size];
    this->joybind_size = 13;
    this->joybinds_foot = new int[this->joybind_size];
    this->jhatbind_size = 9;
    this->jhatbinds_foot = new int[this->jhatbind_size];

    // Size variables appear to have been copied and pasted, but unused?
    // May not be an issue as it could be correct if dynamically resized?
    this->car_keybind_size = 256;
    this->keybinds_car = new int[this->keybind_size];
    this->car_mousebind_size = 3;
    this->mousebinds_car = new int[this->mousebind_size];
    this->car_joybind_size = 13;
    this->joybinds_car = new int[this->joybind_size];
    this->car_jhatbind_size = 9;
    this->jhatbinds_car = new int[this->jhatbind_size];

    int i;
    for (i = 0; i < this->conf_size; ++i) {
        this->conf_values[i] = 0;
    }
    for (i = 0; i < this->keybind_size; ++i) {
        this->keybinds_foot[i] = -1;
        this->keybinds_car[i] = -1;
    }
    for (i = 0; i < this->mousebind_size; ++i) {
        this->mousebinds_foot[i] = -1;
        this->mousebinds_car[i] = -1;
    }
    for (i = 0; i < this->joybind_size; ++i) {
        this->joybinds_foot[i] = -1;
        this->joybinds_car[i] = -1;
    }
    for (i = 0; i < this->jhatbind_size; ++i) {
        this->jhatbinds_foot[i] = -1;
        this->jhatbinds_car[i] = -1;
    }
}

// FUNCTION: REDLINE 0x00440517
int Config::ParseMapping(char *mapping) {
    struct Locals {
        int key;
        int action;
        char buf[128];
        int buf_idx;
        int i;
        int joybutton;
        int joyhat;
        int mbutton;
        bool car;
    } l;
    l.car = false;
    l.action = -1;
    l.joyhat = l.action;
    l.joybutton = l.joyhat;
    l.mbutton = l.joybutton;
    l.key = l.mbutton;

    // Skip whitespace
    while (isspace(*mapping) && *mapping != NULL)
        ++mapping;
    if (*mapping == NULL)
        return -1;

    if (mapping[0] == 'c' && mapping[1] == 'a' && mapping[2] == 'r') {
        mapping += 3;
        l.car = true;
        while (isspace(*mapping) && *mapping != NULL)
            ++mapping;
    }


    // Copy remaining line
    for (l.i = 0; l.i < 127; ++l.i) {
        if (isspace(mapping[l.i]))
            break;
        l.buf[l.i] = mapping[l.i];
    }
    if (l.i == 127)
        return -1;
    l.buf[l.i] = 0;
    l.key = LookupKeyname(l.buf);
    if (l.key < 0) {
        strlwr(l.buf);
        l.mbutton = LookupMbuttonName(l.buf);
        if (l.mbutton < 0) {
            strlwr(l.buf);
            l.joybutton = LookupJoybuttonName(l.buf);
            if (l.joybutton < 0) {
                strlwr(l.buf);
                l.joyhat = LookupJoyhatName(l.buf);
            }
        }
    }
    if (l.key < 0 && l.mbutton < 0 && l.joybutton < 0 && l.joyhat < 0)
        return -1;
    while (isspace(mapping[l.i]) && l.i < 127)
        ++l.i;
    if (l.i == 127)
        return -1;

    // Copy action name into buf
    for (l.buf_idx = 0; l.i < 127; ++l.i, ++l.buf_idx) {
        if (isspace(mapping[l.i]) || mapping[l.i] == NULL)
            break;
        l.buf[l.buf_idx] = mapping[l.i];
    }
    l.buf[l.buf_idx] = 0;
    l.action = LookupActionName(l.buf);
    if (l.action < 0)
        return -1;
    if (l.key >= 0) {
        if (l.car)
            this->keybinds_car[l.key] = l.action;
        else
            this->keybinds_foot[l.key] = l.action;
    } else if (l.mbutton >= 0) {
        if (l.car)
            this->mousebinds_car[l.mbutton] = l.action;
        else
            this->mousebinds_foot[l.mbutton] = l.action;
    } else if (l.joybutton >= 0) {
        if (l.car)
            this->joybinds_car[l.joybutton] = l.action;
        else
            this->joybinds_foot[l.joybutton] = l.action;
    } else {
        if (l.car)
            this->jhatbinds_car[l.joyhat] = l.action;
        else
            this->jhatbinds_foot[l.joyhat] = l.action;
    }

    return 0;
}

// FUNCTION: REDLINE 0x0043d3a0
Value::Value() {
    this->name[0] = 0;
    this->next = NULL;
    this->value.string = NULL;
    this->kind = -1;
}

// FUNCTION: REDLINE 0x0043D3D2
Value::~Value() {
    if (this->next) {
        delete this->next;
        this->next = NULL;
    }
    if (this->kind == 2 && this->value.string != NULL) {
        free(this->value.string);
        this->value.string = NULL;
        this->kind = -1;
    }
}

// FUNCTION: REDLINE 0x00440C9E
Value *Config::GetValue(const char *name) {
    struct Locals {
        int key;
        Value *v;
        char lower[64];
    } l;

    if (name == NULL)
        return NULL;
    strcpy(l.lower, name);
    strlwr(l.lower);
    l.key = *l.lower;
    l.v = this->conf_values[l.key];
    while (l.v != NULL) {
        if (!strcmp(l.lower, l.v->name))
            break;
        l.v = l.v->next;
    }
    return l.v;
}

// FUNCTION: REDLINE 0x00440c42
int Config::StoreValue(Value *v) {
    if (v == NULL)
        return -1;
    int i = v->name[0];
    Value *prev_head = this->conf_values[i];
    if (prev_head != NULL)
        v->next = prev_head;
    else
        v->next = NULL;
    this->conf_values[i] = v;
    return 0;
}

// FUNCTION: REDLINE 0x0043F86D
int Config::SetBoolValue(const char* key, bool value) {
    bool is_new = false;
    if (!key)
        return -1;
    Value *val = this->GetValue(key);
    if (!val) {
        val = new Value();
        strcpy(val->name, key);
        strlwr(val->name);
        val->kind = VALUE_INTBOOL;
        is_new = true;
    } else {
        if (val->kind && val->kind != VALUE_INTBOOL && val->kind != VALUE_CHARBOOL)
            return -1;
    }
    val->value.integer = value != false;
    int res = 0;
    if (is_new) {
        res = this->StoreValue(val);
        if (res != 0) {
            delete val;
        }
    }
    return res;
}

// FUNCTION: REDLINE 0x0043F716
int Config::SetIntValue(const char* key, int value) {
    bool is_new = false;
    if (!key)
        return -1;
    Value *val = this->GetValue(key);
    if (!val) {
        val = new Value();
        strcpy(val->name, key);
        strlwr(val->name);
        val->kind = VALUE_SIGNED;
        is_new = true;
    } else {
        // FIXME: Shouldn't this verify it is VALUE_SIGNED, not a bool?
        if (val->kind && val->kind != VALUE_INTBOOL && val->kind != VALUE_CHARBOOL)
            return -1;
    }
    val->value.integer = value;
    int res = 0;
    if (is_new) {
        res = this->StoreValue(val);
        if (res != 0) {
            delete val;
        }
    }
    return res;
}

// FUNCTION: REDLINE 0x0043F460
int Config::SetStringValue(const char *key, const char *value) {
    bool is_new = false;
    if (!value || !key)
        return -1;
    Value *val = this->GetValue(key);
    if (!val) {
        val = new Value();
        strcpy(val->name, key);
        strlwr(val->name);
        val->kind = VALUE_STRING;
        is_new = true;
    }
    if (val->kind == 2 && val->value.string)
        free(val->value.string);
    val->kind = 2;
    val->value.string = strdup(value);
    int res = 0;
    if (is_new) {
        res = this->StoreValue(val);
        if (res != 0) {
            delete val;
        }
    }
    return res;
}

// FUNCTION: REDLINE 0x0043F3B0
int Config::GetIntValue(const char *name, int *out) {
    Value *v = this->GetValue(name);
    if (!v)
        return -1;
    if (v->kind != VALUE_SIGNED && v->kind != VALUE_INTBOOL &&
        v->kind != VALUE_CHARBOOL)
        return -1;
    *out = v->value.integer;
    return 0;
}

// FUNCTION: REDLINE 0x0043F36C
int Config::GetFloatValue(const char *name, float *out) {
    Value *v = this->GetValue(name);
    if (!v)
        return -1;
    if (v->kind != VALUE_FLOAT)
        return -1;
    *out = v->value.decimal;
    return 0;
}

// FUNCTION: REDLINE 0x0043F315
int Config::GetStringValue(const char *name, char *out) {
    if (out == NULL)
        return -1;
    Value *v = this->GetValue(name);
    if (!v)
        return -1;
    if (v->kind != VALUE_STRING)
        return -1;
    strcpy(out, v->value.string);
    return 0;
}

// FUNCTION: REDLINE 0x0043F406
int Config::GetBoolValue(const char *name, bool *out) {
    Value *v = this->GetValue(name);
    if (!v)
        return -1;
    if (v->kind != VALUE_SIGNED && v->kind != VALUE_INTBOOL &&
        v->kind != VALUE_CHARBOOL)
        return -1;
    *out = v->value.integer != 0; // TODO: This doesn't perfectly match
    return 0;
}

// Intrinsic memcpy is to replicate initialization of seperators and line_endings
// as if they weren't inside of a struct
#pragma intrinsic(memcpy)
// FUNCTION: REDLINE 0x004408BD
int Config::ParseOther(char *line) {
    struct Locals {
        int len;
        char *after_dot;
        int integer;
        char *sep;
        char *line_copy;
        float decimal;
        void *unk2;
        char line_endings[5];
        Value *v;
        int kind;
        char seperators[6];
        char *line_end;
    } l;

    memcpy(l.seperators, " \b\r\t\n", 6);
    memcpy(l.line_endings, "\b\r\t\n", 5); // Note exclusion of space

    l.kind = -1;
    while (isspace(*line) && *line != NULL)
        ++line;
    if (*line == NULL)
        return -1;
    l.line_copy = strdup(line); // Needs freed
    l.sep = strtok(l.line_copy, l.seperators);
    if (l.sep == NULL) {
        free(l.line_copy);
        return -1;
    }
    while (isspace(*line) && *line != NULL)
        ++line;

    if (*line == NULL) {
        free(l.line_copy);
        return -1;
    }
    l.line_end = strtok(NULL, l.line_endings);
    if (l.line_end == NULL) {
        free(l.line_copy);
        return -1;
    }
    l.unk2 = NULL;
    if (*l.line_end == '\'') {
        l.kind = 2;
        ++l.line_end;
    } else if (strstr(l.line_end, ".")) {
        l.after_dot = strstr(l.line_end, ".");
        l.after_dot++;
        l.unk2 = strstr(l.line_end, ".");
        if (l.unk2 == NULL) {
            l.decimal = (float)atof(l.line_end);
            if (l.decimal != 0.0) {
                l.kind = 1;
            } else if (strstr(l.line_end, "0.0")) {
                l.kind = 1;
            }
        }
    }
    if (l.kind < 0 && l.unk2 == NULL) {
        l.integer = atol(l.line_end);
        if (l.integer != 0) {
            l.kind = 0;
        } else if (*l.line_end == '0') {
            l.kind = 0;
        }
    }
    if (l.kind == -1)
        l.kind = 2;
    l.v = new Value();

    // int v19 = -1;
    if (strlen(l.sep) >= 32) {
        strncpy(l.v->name, l.sep, 30);
        l.v->name[31] = 0;
    } else {
        strcpy(l.v->name, l.sep);
    }
    // TODO: switch
    switch (l.kind) {
        case 1:
            l.v->value.decimal = l.decimal;
            break;
        case 0:
        case 3:
            l.v->value.integer = l.integer;
            break;
        case 2:
            l.v->value.string = strdup(l.line_end);
            l.len = strlen(l.v->value.string);
            if (l.len > 0 && l.v->value.string[l.len - 1] == '\'')
                l.v->value.string[l.len - 1] = 0;
            break;
        case -1:
            free(l.line_copy);
            return -1;
    }
    l.v->kind = l.kind;
    free(l.line_copy);
    return this->StoreValue(l.v);
    free(l.line_copy);
    return -1;
}
#pragma function(memcpy)

// FUNCTION: REDLINE 0x004403fe
int Config::ParseLine(char *line) {
    if (line == NULL)
        return -1;

    int len = strlen(line);

    // Detect comment lines (including preceding whitespace)
    for (int i = 0; i < len; ++i) {
        if (!isspace(line[i]) && line[i] != '#') {
            break;
        }
        if (line[i] == '#')
            return 0;
    }

    // Convert line to lowercase
    strlwr(line);

    // Skip preceding whitespace
    while (isspace(*line) && *line != NULL)
        ++line;

    // Blank line
    if (*line == NULL)
        return 0;

    if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p' &&
        isspace(line[3])) {
        return ParseMapping(&line[4]);
    }

    return ParseOther(line);
}

// FUNCTION: REDLINE 0x0043CBE5
void Warn(const char *fmt, ...) {} // Left empty in build

// FUNCTION: REDLINE 0x0043D90B
void Config::ResetBinds() {
    int i;
    if (this->keybinds_foot) {
        for (i = 0; i < this->conf_size; ++i) {
            this->keybinds_foot[i] = -1;
            this->keybinds_car[i] = -1;
        }
    }
    if (this->mousebinds_foot) {
        for (i = 0; i < this->mousebind_size; ++i) {
            this->mousebinds_foot[i] = -1;
            this->mousebinds_car[i] = -1;
        }
    }
    if (this->joybinds_foot) {
        for (i = 0; i < this->joybind_size; ++i) {
            this->joybinds_foot[i] = -1;
            this->joybinds_car[i] = -1;
        }
    }
    if (this->jhatbinds_foot) {
        for (i = 0; i < this->jhatbind_size; ++i) {
            this->jhatbinds_foot[i] = -1;
            this->jhatbinds_car[i] = -1;
        }
    }
}

// FUNCTION: REDLINE 0x0043E117
void Config::LoadPreset(int mode) {
    this->ResetBinds();
    switch (mode) {
    default:
        this->mousebinds_foot[MOUSE_LEFT] = ACTION_FIRE;
        this->mousebinds_foot[MOUSE_RIGHT] = ACTION_FORWARD;
        this->keybinds_foot[KEY_UP] = ACTION_FORWARD;
        this->keybinds_foot[KEY_DOWN] = ACTION_BACKWARD;
        this->keybinds_foot[KEY_LEFT] = ACTION_LEFT;
        this->keybinds_foot[KEY_RIGHT] = ACTION_RIGHT;
        this->keybinds_foot[KEY_PAD8] = ACTION_FORWARD;
        this->keybinds_foot[KEY_PAD2] = ACTION_BACKWARD;
        this->keybinds_foot[KEY_PAD4] = ACTION_LEFT;
        this->keybinds_foot[KEY_PAD6] = ACTION_RIGHT;
        this->keybinds_foot[KEY_PAD5] = ACTION_JUMP;
        this->keybinds_foot[KEY_PAD9] = ACTION_AIM_UP;
        this->keybinds_foot[KEY_PAD3] = ACTION_AIM_DOWN;
        this->keybinds_foot[KEY_LCTRL] = ACTION_FIRE;
        this->keybinds_foot[KEY_RCTRL] = ACTION_FIRE;
        this->keybinds_foot[KEY_X] = ACTION_DUCK;
        this->keybinds_foot[KEY_RALT] = ACTION_AIM;
        this->keybinds_foot[KEY_LALT] = ACTION_AIM;
        this->keybinds_foot[KEY_LSHIFT] = ACTION_STRAFE;
        this->keybinds_foot[KEY_RSHIFT] = ACTION_STRAFE;
        this->keybinds_foot[KEY_COMMA] = ACTION_STRAFE_LEFT;
        this->keybinds_foot[KEY_PERIOD] = ACTION_STRAFE_RIGHT;
        this->keybinds_foot[KEY_SPACE] = ACTION_JUMP;
        this->keybinds_foot[KEY_A] = ACTION_AIM_UP;
        this->keybinds_foot[KEY_Z] = ACTION_AIM_DOWN;
        this->keybinds_foot[KEY_Q] = ACTION_AIM_CLEAR;
        this->keybinds_foot[KEY_DELETE] = ACTION_DIVE_LEFT;
        this->keybinds_foot[KEY_PAGEDOWN] = ACTION_DIVE_RIGHT;
        this->keybinds_foot[KEY_TAB] = ACTION_CHANGE_WEAPON;
        this->keybinds_foot[KEY_F1] = ACTION_CAMERA_IN;
        this->keybinds_foot[KEY_F2] = ACTION_CAMERA_BEHIND;
        this->keybinds_foot[KEY_F3] = ACTION_CAMERA_FRONT;
        this->keybinds_foot[KEY_INSERT] = ACTION_LOOK_LEFT;
        this->keybinds_foot[KEY_HOME] = ACTION_LOOK_REAR;
        this->keybinds_foot[KEY_PAGEUP] = ACTION_LOOK_RIGHT;
        this->keybinds_foot[KEY_1] = ACTION_WEAPON_1;
        this->keybinds_foot[KEY_2] = ACTION_WEAPON_2;
        this->keybinds_foot[KEY_3] = ACTION_WEAPON_3;
        this->keybinds_foot[KEY_4] = ACTION_WEAPON_4;
        this->keybinds_foot[KEY_5] = ACTION_WEAPON_5;
        this->keybinds_foot[KEY_6] = ACTION_WEAPON_6;
        this->keybinds_foot[KEY_7] = ACTION_WEAPON_7;
        this->keybinds_foot[KEY_8] = ACTION_WEAPON_8;
        this->keybinds_foot[KEY_9] = ACTION_WEAPON_9;
        this->keybinds_foot[KEY_0] = ACTION_WEAPON_10;
        this->keybinds_foot[KEY_LBRACKET] = ACTION_PREV_TARGET;
        this->keybinds_foot[KEY_RBRACKET] = ACTION_NEXT_TARGET;
        this->keybinds_foot[KEY_SUBTRACT] = ACTION_ZOOM_OUT;
        this->keybinds_foot[KEY_ADD] = ACTION_ZOOM_IN;
        this->keybinds_foot[KEY_F12] = ACTION_SUICIDE;
        this->keybinds_foot[KEY_ESC] = ACTION_EXIT;
        this->keybinds_car[KEY_SPACE] = ACTION_PARKING_BRAKE;
        this->keybinds_car[KEY_PAD5] = ACTION_PARKING_BRAKE;
        this->keybinds_car[KEY_COMMA] = ACTION_FIRE_LEFT;
        this->keybinds_car[KEY_PERIOD] = ACTION_FIRE_REAR;
        this->keybinds_car[KEY_SLASH] = ACTION_FIRE_RIGHT;
        break;
    case 1:
        this->mousebinds_foot[MOUSE_LEFT] = ACTION_FIRE;
        this->mousebinds_foot[MOUSE_RIGHT] = ACTION_JUMP;
        this->keybinds_foot[KEY_W] = ACTION_FORWARD;
        this->keybinds_foot[KEY_S] = ACTION_BACKWARD;
        this->keybinds_foot[KEY_A] = ACTION_STRAFE_LEFT;
        this->keybinds_foot[KEY_D] = ACTION_STRAFE_RIGHT;
        this->keybinds_foot[KEY_LSHIFT] = ACTION_CHANGE_WEAPON;
        this->keybinds_foot[KEY_LCTRL] = ACTION_FIRE;
        this->keybinds_foot[KEY_Z] = ACTION_DIVE_LEFT;
        this->keybinds_foot[KEY_C] = ACTION_DIVE_RIGHT;
        this->keybinds_foot[KEY_X] = ACTION_DUCK;
        this->keybinds_foot[KEY_SPACE] = ACTION_JUMP;
        this->keybinds_foot[KEY_SUBTRACT] = ACTION_ZOOM_OUT;
        this->keybinds_foot[KEY_ADD] = ACTION_ZOOM_IN;
        this->keybinds_car[KEY_W] = ACTION_FORWARD;
        this->keybinds_car[KEY_S] = ACTION_BACKWARD;
        this->keybinds_car[KEY_A] = ACTION_LEFT;
        this->keybinds_car[KEY_D] = ACTION_RIGHT;
        this->keybinds_car[KEY_LCTRL] = ACTION_FIRE;
        this->keybinds_car[KEY_PAD8] = ACTION_FIRE;
        this->keybinds_car[KEY_Q] = ACTION_AIM_UP;
        this->keybinds_car[KEY_E] = ACTION_AIM_DOWN;
        this->keybinds_car[KEY_SPACE] = ACTION_PARKING_BRAKE;
        this->keybinds_car[KEY_PAD8] = ACTION_FIRE;
        this->keybinds_car[KEY_PAD4] = ACTION_FIRE_LEFT;
        this->keybinds_car[KEY_PAD5] = ACTION_FIRE_REAR;
        this->keybinds_car[KEY_PAD6] = ACTION_FIRE_RIGHT;
        this->keybinds_car[KEY_UP] = ACTION_FIRE;
        this->keybinds_car[KEY_LEFT] = ACTION_FIRE_LEFT;
        this->keybinds_car[KEY_DOWN] = ACTION_FIRE_REAR;
        this->keybinds_car[KEY_RIGHT] = ACTION_FIRE_RIGHT;
        this->keybinds_foot[KEY_F1] = ACTION_CAMERA_IN;
        this->keybinds_foot[KEY_F2] = ACTION_CAMERA_BEHIND;
        this->keybinds_foot[KEY_F3] = ACTION_CAMERA_FRONT;
        this->keybinds_foot[KEY_INSERT] = ACTION_LOOK_LEFT;
        this->keybinds_foot[KEY_HOME] = ACTION_LOOK_REAR;
        this->keybinds_foot[KEY_PAGEUP] = ACTION_LOOK_RIGHT;
        this->keybinds_foot[KEY_1] = ACTION_WEAPON_1;
        this->keybinds_foot[KEY_2] = ACTION_WEAPON_2;
        this->keybinds_foot[KEY_3] = ACTION_WEAPON_3;
        this->keybinds_foot[KEY_4] = ACTION_WEAPON_4;
        this->keybinds_foot[KEY_5] = ACTION_WEAPON_5;
        this->keybinds_foot[KEY_6] = ACTION_WEAPON_6;
        this->keybinds_foot[KEY_7] = ACTION_WEAPON_7;
        this->keybinds_foot[KEY_8] = ACTION_WEAPON_8;
        this->keybinds_foot[KEY_9] = ACTION_WEAPON_9;
        this->keybinds_foot[KEY_0] = ACTION_WEAPON_10;
        this->keybinds_foot[KEY_LBRACKET] = ACTION_PREV_TARGET;
        this->keybinds_foot[KEY_RBRACKET] = ACTION_NEXT_TARGET;
        this->keybinds_foot[KEY_F12] = ACTION_SUICIDE;
        this->keybinds_foot[KEY_ESC] = ACTION_EXIT;
        break;
    case 2:
        this->mousebinds_foot[MOUSE_LEFT] = ACTION_FIRE;
        this->mousebinds_foot[MOUSE_RIGHT] = ACTION_JUMP;
        this->keybinds_foot[KEY_PAD8] = ACTION_FORWARD;
        this->keybinds_foot[KEY_PAD5] = ACTION_BACKWARD;
        this->keybinds_foot[KEY_PAD7] = ACTION_STRAFE_LEFT;
        this->keybinds_foot[KEY_PAD9] = ACTION_STRAFE_RIGHT;
        this->keybinds_foot[KEY_PAD4] = ACTION_LEFT;
        this->keybinds_foot[KEY_PAD6] = ACTION_RIGHT;
        this->keybinds_foot[KEY_PAD1] = ACTION_DIVE_LEFT;
        this->keybinds_foot[KEY_PAD3] = ACTION_DIVE_RIGHT;
        this->keybinds_foot[KEY_DIVIDE] = ACTION_CHANGE_WEAPON;
        this->keybinds_foot[KEY_X] = ACTION_DUCK;
        this->keybinds_foot[KEY_Q] = ACTION_AIM;
        this->keybinds_foot[KEY_SPACE] = ACTION_JUMP;
        this->keybinds_foot[KEY_SUBTRACT] = ACTION_ZOOM_OUT;
        this->keybinds_foot[KEY_ADD] = ACTION_ZOOM_IN;
        this->keybinds_foot[KEY_RSHIFT] = ACTION_STRAFE;
        this->keybinds_car[KEY_PAD8] = ACTION_FORWARD;
        this->keybinds_car[KEY_PAD5] = ACTION_BACKWARD;
        this->keybinds_car[KEY_PAD7] = ACTION_DIVE_LEFT;
        this->keybinds_car[KEY_PAD9] = ACTION_DIVE_RIGHT;
        this->keybinds_car[KEY_PAD4] = ACTION_LEFT;
        this->keybinds_car[KEY_PAD6] = ACTION_RIGHT;
        this->keybinds_car[KEY_PAD1] = ACTION_FIRE_LEFT;
        this->keybinds_car[KEY_PAD3] = ACTION_FIRE_RIGHT;
        this->keybinds_car[KEY_PAD2] = ACTION_FIRE_REAR;
        this->keybinds_car[KEY_ENTER] = ACTION_FIRE;
        this->mousebinds_car[MOUSE_LEFT] = ACTION_FIRE;
        this->mousebinds_car[MOUSE_RIGHT] = ACTION_PARKING_BRAKE;
        this->keybinds_foot[KEY_F1] = ACTION_CAMERA_IN;
        this->keybinds_foot[KEY_F2] = ACTION_CAMERA_BEHIND;
        this->keybinds_foot[KEY_F3] = ACTION_CAMERA_FRONT;
        this->keybinds_foot[KEY_INSERT] = ACTION_LOOK_LEFT;
        this->keybinds_foot[KEY_HOME] = ACTION_LOOK_REAR;
        this->keybinds_foot[KEY_PAGEUP] = ACTION_LOOK_RIGHT;
        this->keybinds_foot[KEY_1] = ACTION_WEAPON_1;
        this->keybinds_foot[KEY_2] = ACTION_WEAPON_2;
        this->keybinds_foot[KEY_3] = ACTION_WEAPON_3;
        this->keybinds_foot[KEY_4] = ACTION_WEAPON_4;
        this->keybinds_foot[KEY_5] = ACTION_WEAPON_5;
        this->keybinds_foot[KEY_6] = ACTION_WEAPON_6;
        this->keybinds_foot[KEY_7] = ACTION_WEAPON_7;
        this->keybinds_foot[KEY_8] = ACTION_WEAPON_8;
        this->keybinds_foot[KEY_9] = ACTION_WEAPON_9;
        this->keybinds_foot[KEY_0] = ACTION_WEAPON_10;
        this->keybinds_foot[KEY_LBRACKET] = ACTION_PREV_TARGET;
        this->keybinds_foot[KEY_RBRACKET] = ACTION_NEXT_TARGET;
        this->keybinds_foot[KEY_F12] = ACTION_SUICIDE;
        this->keybinds_foot[KEY_ESC] = ACTION_EXIT;
        break;
    }
}

// FUNCTION: REDLINE 0x0043D86C
void Config::ClearConfValues() {
    if (!this->conf_values)
        return;
    for (int i = 0; i < this->conf_size; ++i) {
        if (this->conf_values[i]) {
            delete this->conf_values[i];
            this->conf_values[i] = NULL;
        }
    }
}

#define GLOBAL_STRING_SIZE 128

// GLOBAL: REDLINE 0x005CE638
int g_D3DSound;
// GLOBAL: REDLINE 0x005CE654
float g_Mouse_Foot;
// GLOBAL: REDLINE 0x005CE658
float g_Mouse_Car;
// GLOBAL: REDLINE 0x005CE608
int g_FreeLook;
// GLOBAL: REDLINE 0x005CE60C
int g_CarFreeLook;
// GLOBAL: REDLINE 0x005CE610
int g_ReverseYAxis;
// GLOBAL: REDLINE 0x005CE614
int g_ReverseYAxisCar;
// GLOBAL: REDLINE 0x005CE88C
int g_ScreenWidth;
// GLOBAL: REDLINE 0x005CE890
int g_ScreenHeight;
// GLOBAL: REDLINE 0x005CE894
int g_ScreenBPP;
// GLOBAL: REDLINE 0x005CE898
float g_ScreenGamma;
// GLOBAL: REDLINE 0x005CE8F0
int g_MipMapping;
// GLOBAL: REDLINE 0x005CE640
int g_DrawShadows;
// GLOBAL: REDLINE 0x005CE89C
int g_DisplayParticles;
// GLOBAL: REDLINE 0x005CE8A0
int g_DisplayTireTreads;
// GLOBAL: REDLINE 0x005CE8A4
int g_DisplayScreenFlash;
// GLOBAL: REDLINE 0x005CE8A8
int g_DifficultyLevel;
// GLOBAL: REDLINE 0x005CE8AC
int g_EnableFog;
// GLOBAL: REDLINE 0x005CE8B0
int g_EnviroMapping;
// GLOBAL: REDLINE 0x005CE8B4
int g_GroundLighting;
// GLOBAL: REDLINE 0x005CE8B8
int g_PalettedTextures;
// GLOBAL: REDLINE 0x005CE8C4
int g_TextureDetail;
// GLOBAL: REDLINE 0x005CE8BC
int g_TripleBuffer;
// GLOBAL: REDLINE 0x005CE8C0
int g_LimitParticleSize;
// GLOBAL: REDLINE 0x005CE8C8
int g_DXtextureManager;
// GLOBAL: REDLINE 0x005CE8CC
int g_DitherEnable;
// GLOBAL: REDLINE 0x005CE8D0
int g_WeaponDisplayMin;
// GLOBAL: REDLINE 0x005CE8D4
int g_PersonGunDraw;
// GLOBAL: REDLINE 0x005CE8D8
int g_carDashboardDraw;
// GLOBAL: REDLINE 0x005CE8DC
int g_turretDashboardDraw;
// GLOBAL: REDLINE 0x005CE8E0
int g_carCamMode;
// GLOBAL: REDLINE 0x005CE8E4
int g_turretCamMode;
// GLOBAL: REDLINE 0x005CE8EC
int g_turretFollowCamMode;
// GLOBAL: REDLINE 0x005CE8E8
int g_carFollowCamMode;
// GLOBAL: REDLINE 0x005CE64C
int g_mouseWheel;
// GLOBAL: REDLINE 0x005CE648
int g_carMouseWheel;
// GLOBAL: REDLINE 0x005CE664
int g_carSteerInc;
// GLOBAL: REDLINE 0x005CE668
int g_carSteerMax;
// GLOBAL: REDLINE 0x005CE65C
int g_footSteerInc;
// GLOBAL: REDLINE 0x005CE660
int g_footSteerMax;
// GLOBAL: REDLINE 0x005CE8F4
int g_soundChannels;
// GLOBAL: REDLINE 0x005CE650
int g_cruiseControl;
// GLOBAL: REDLINE 0x005CE674
char g_LastMap[GLOBAL_STRING_SIZE];
// GLOBAL: REDLINE 0x005CE78C
char g_DisplayDevice[GLOBAL_STRING_SIZE];
// GLOBAL: REDLINE 0x005CE80C
char g_DeviceDriver[GLOBAL_STRING_SIZE];
// GLOBAL: REDLINE 0x005CE9C6
char g_Net_GameName[GLOBAL_STRING_SIZE];
// GLOBAL: REDLINE 0x005CE91A
bool g_Net_Mode_ScoreLimitOn;
// GLOBAL: REDLINE 0x005CE920
int g_Net_Mode_ScoreLimit;
// GLOBAL: REDLINE 0x005CE919
bool g_Net_Mode_TimeLimitOn;
// GLOBAL: REDLINE 0x005CE91C
int g_Net_Mode_TimeLimit;
// GLOBAL: REDLINE 0x005CE918
bool g_Net_SmartCrosshair;
// GLOBAL: REDLINE 0x005CE90B
bool g_Net_Teams;
// GLOBAL: REDLINE 0x005CE90E
bool g_Net_FriendlyFire;
// GLOBAL: REDLINE 0x005CE90C
bool g_Net_TeamPlace;
// GLOBAL: REDLINE 0x005CE908
bool g_Net_Mode_CTF;
// GLOBAL: REDLINE 0x005CE909
bool g_Net_Mode_CTF_FlagDrop;
// GLOBAL: REDLINE 0x005CE90A
bool g_Net_Mode_CTF_Adv;
// GLOBAL: REDLINE 0x005CE946
char g_Net_LastIP[GLOBAL_STRING_SIZE];
// GLOBAL: REDLINE 0x005CE910
int g_Net_PlayerTeam;
// GLOBAL: REDLINE 0x005CE914
int g_Net_PlayerSkel;
// GLOBAL: REDLINE 0x005CE928
int g_Net_MaxPlayers;
// GLOBAL: REDLINE 0x005CE8F9
bool g_Net_ConsoleTCP;
// GLOBAL: REDLINE 0x005CEAC8
float g_Joystick_UpDown;
// GLOBAL: REDLINE 0x005CEACC
float g_Joystick_LeftRight;
// GLOBAL: REDLINE 0x005CEAD8
int g_Joystick_FreeLook;
// GLOBAL: REDLINE 0x005CEAE4
float g_Master_Volume;
// GLOBAL: REDLINE 0x005CEAD0
float g_CarJoystick_UpDown;
// GLOBAL: REDLINE 0x005CEAD4
float g_CarJoystick_LeftRight;
// GLOBAL: REDLINE 0x005CEADC
int g_Joystick_DeadZoneX;
// GLOBAL: REDLINE 0x005CEAE0
int g_Joystick_DeadZoneY;
// GLOBAL: REDLINE 0x005CEAE8
bool g_CDAudio_Active;
// GLOBAL: REDLINE 0x005CEAE9
bool g_IntroVideo;

// GLOBAL: REDLINE 0x005985e8
DefaultConfValue g_ConfDefault[73] = {
    {"D3DSound", &g_D3DSound, VALUE_INTBOOL},
    {"Mouse_Foot", &g_Mouse_Foot, VALUE_FLOAT},
    {"Mouse_Car", &g_Mouse_Car, VALUE_FLOAT},
    {"FreeLook", &g_FreeLook, VALUE_INTBOOL},
    {"CarFreeLook", &g_CarFreeLook, VALUE_INTBOOL},
    {"ReverseYAxis", &g_ReverseYAxis, VALUE_INTBOOL},
    {"ReverseYAxisCar", &g_ReverseYAxisCar, VALUE_INTBOOL},
    {"ScreenWidth", &g_ScreenWidth, VALUE_SIGNED},
    {"ScreenHeight", &g_ScreenHeight, VALUE_SIGNED},
    {"ScreenBPP", &g_ScreenBPP, VALUE_SIGNED},
    {"ScreenGamma", &g_ScreenGamma, VALUE_FLOAT},
    {"MipMapping", &g_MipMapping, VALUE_SIGNED},
    {"DrawShadows", &g_DrawShadows, VALUE_INTBOOL},
    {"DisplayParticles", &g_DisplayParticles, VALUE_SIGNED},
    {"DisplayTireTreads", &g_DisplayTireTreads, VALUE_INTBOOL},
    {"DisplayScreenFlash", &g_DisplayScreenFlash, VALUE_INTBOOL},
    {"DifficultyLevel", &g_DifficultyLevel, VALUE_SIGNED},
    {"EnableFog", &g_EnableFog, VALUE_INTBOOL},
    {"EnviroMapping", &g_EnviroMapping, VALUE_INTBOOL},
    {"GroundLighting", &g_GroundLighting, VALUE_INTBOOL},
    {"PalettedTextures", &g_PalettedTextures, VALUE_INTBOOL},
    {"TextureDetail", &g_TextureDetail, VALUE_SIGNED},
    {"TripleBuffer", &g_TripleBuffer, VALUE_INTBOOL},
    {"LimitParticleSize", &g_LimitParticleSize, VALUE_INTBOOL},
    {"DXtextureManager", &g_DXtextureManager, VALUE_INTBOOL},
    {"DitherEnable", &g_DitherEnable, VALUE_INTBOOL},
    {"WeaponDisplayMin", &g_WeaponDisplayMin, VALUE_INTBOOL},
    {"PersonGunDraw", &g_PersonGunDraw, VALUE_INTBOOL},
    {"carDashboardDraw", &g_carDashboardDraw, VALUE_INTBOOL},
    {"turretDashboardDraw", &g_turretDashboardDraw, VALUE_SIGNED},
    {"carCamMode", &g_carCamMode, VALUE_SIGNED},
    {"turretCamMode", &g_turretCamMode, VALUE_SIGNED},
    {"turretFollowCamMode", &g_turretFollowCamMode, VALUE_SIGNED},
    {"carFollowCamMode", &g_carFollowCamMode, VALUE_SIGNED},
    {"mouseWheel", &g_mouseWheel, VALUE_SIGNED},
    {"carMouseWheel", &g_carMouseWheel, VALUE_SIGNED},
    {"carSteerInc", &g_carSteerInc, VALUE_SIGNED},
    {"carSteerMax", &g_carSteerMax, VALUE_SIGNED},
    {"footSteerInc", &g_footSteerInc, VALUE_SIGNED},
    {"footSteerMax", &g_footSteerMax, VALUE_SIGNED},
    {"soundChannels", &g_soundChannels, VALUE_SIGNED},
    {"cruiseControl", &g_cruiseControl, VALUE_SIGNED},
    {"LastMap", g_LastMap, VALUE_STRING},
    {"DisplayDevice", g_DisplayDevice, VALUE_STRING},
    {"DeviceDriver", g_DeviceDriver, VALUE_STRING},
    {"Net_GameName", g_Net_GameName, VALUE_STRING},
    {"Net_Mode_ScoreLimitOn", &g_Net_Mode_ScoreLimitOn, VALUE_CHARBOOL},
    {"Net_Mode_ScoreLimit", &g_Net_Mode_ScoreLimit, VALUE_SIGNED},
    {"Net_Mode_TimeLimitOn", &g_Net_Mode_TimeLimitOn, VALUE_CHARBOOL},
    {"Net_Mode_TimeLimit", &g_Net_Mode_TimeLimit, VALUE_SIGNED},
    {"Net_SmartCrosshair", &g_Net_SmartCrosshair, VALUE_CHARBOOL},
    {"Net_Teams", &g_Net_Teams, VALUE_CHARBOOL},
    {"Net_FriendlyFire", &g_Net_FriendlyFire, VALUE_CHARBOOL},
    {"Net_TeamPlace", &g_Net_TeamPlace, VALUE_CHARBOOL},
    {"Net_Mode_CTF", &g_Net_Mode_CTF, VALUE_CHARBOOL},
    {"Net_Mode_CTF_FlagDrop", &g_Net_Mode_CTF_FlagDrop, VALUE_CHARBOOL},
    {"Net_Mode_CTF_Adv", &g_Net_Mode_CTF_Adv, VALUE_CHARBOOL},
    {"Net_LastIP", g_Net_LastIP, VALUE_STRING},
    {"Net_PlayerTeam", &g_Net_PlayerTeam, VALUE_SIGNED},
    {"Net_PlayerSkel", &g_Net_PlayerSkel, VALUE_SIGNED},
    {"Net_MaxPlayers", &g_Net_MaxPlayers, VALUE_SIGNED},
    {"Net_ConsoleTCP", &g_Net_ConsoleTCP, VALUE_CHARBOOL},
    {"Joystick_UpDown", &g_Joystick_UpDown, VALUE_FLOAT},
    {"Joystick_LeftRight", &g_Joystick_LeftRight, VALUE_FLOAT},
    {"Joystick_FreeLook", &g_Joystick_FreeLook, VALUE_INTBOOL},
    {"Master_Volume", &g_Master_Volume, VALUE_FLOAT},
    {"CarJoystick_UpDown", &g_CarJoystick_UpDown, VALUE_FLOAT},
    {"CarJoystick_LeftRight", &g_CarJoystick_LeftRight, VALUE_FLOAT},
    {"Joystick_DeadZoneX", &g_Joystick_DeadZoneX, VALUE_SIGNED},
    {"Joystick_DeadZoneY", &g_Joystick_DeadZoneY, VALUE_SIGNED},
    {"CDAudio_Active", &g_CDAudio_Active, VALUE_CHARBOOL},
    {"IntroVideo", &g_IntroVideo, VALUE_CHARBOOL},
    {"", 0, 0},
};

// GLOBAL: REDLINE 0x005CE604
int g_DebugMouse;
// GLOBAL: REDLINE 0x005CE618
int g_DebugFrames;
// GLOBAL: REDLINE 0x005CE66C
int g_NumFrames;
// GLOBAL: REDLINE 0x005CE61C
int g_PlayDemo;
// GLOBAL: REDLINE 0x005CE624
int g_RecordDemo;
// GLOBAL: REDLINE 0x005CE630
int g_Windowed;
// GLOBAL: REDLINE 0x005CE634
int g_AIActive;
// GLOBAL: REDLINE 0x005CE63C
int g_QuickRun;
// GLOBAL: REDLINE 0x005CE644
int g_CreateUseFile;
// GLOBAL: REDLINE 0x005CE628
int g_replayRecord;
// GLOBAL: REDLINE 0x005CE62C
int g_replayPlay;
// GLOBAL: REDLINE 0x005CE930
int g_Net_Perf_CliSendFrames;
// GLOBAL: REDLINE 0x005CE934
int g_Net_Perf_ServSendFrames;
// GLOBAL: REDLINE 0x005CE938
bool g_Net_Perf_CliInterp;
// GLOBAL: REDLINE 0x005CE939
bool g_Net_Perf_CliPredict;
// GLOBAL: REDLINE 0x005CE93D
bool g_Net_Perf_ExtraLatencyOn;
// GLOBAL: REDLINE 0x005CE940
int g_Net_Perf_ExtraLatency;
// GLOBAL: REDLINE 0x005CE8FA
bool g_Net_FastStart;
// GLOBAL: REDLINE 0x005CE8FB
bool g_Net_FastHost;

// Debug/Inaccessible?
DefaultConfValue g_ConfDefaultDebug[20] = {
    {"DebugMouse", &g_DebugMouse, VALUE_INTBOOL},
    {"DebugFrames", &g_DebugFrames, VALUE_INTBOOL},
    {"NumFrames", &g_NumFrames, VALUE_SIGNED},
    {"PlayDemo", &g_PlayDemo, VALUE_INTBOOL},
    {"RecordDemo", &g_RecordDemo, VALUE_INTBOOL},
    {"Windowed", &g_Windowed, VALUE_INTBOOL},
    {"AIActive", &g_AIActive, VALUE_INTBOOL},
    {"QuickRun", &g_QuickRun, VALUE_INTBOOL},
    {"CreateUseFile", &g_CreateUseFile, VALUE_INTBOOL},
    {"replayRecord", &g_replayRecord, VALUE_SIGNED},
    {"replayPlay", &g_replayPlay, VALUE_SIGNED},
    {"Net_Perf_CliSendFrames", &g_Net_Perf_CliSendFrames, VALUE_SIGNED},
    {"Net_Perf_ServSendFrames", &g_Net_Perf_ServSendFrames, VALUE_SIGNED},
    {"Net_Perf_CliInterp", &g_Net_Perf_CliInterp, VALUE_CHARBOOL},
    {"Net_Perf_CliPredict", &g_Net_Perf_CliPredict, VALUE_CHARBOOL},
    {"Net_Perf_ExtraLatencyOn", &g_Net_Perf_ExtraLatencyOn, VALUE_CHARBOOL},
    {"Net_Perf_ExtraLatency", &g_Net_Perf_ExtraLatency, VALUE_SIGNED},
    {"Net_FastStart", &g_Net_FastStart, VALUE_CHARBOOL},
    {"Net_FastHost", &g_Net_FastHost, VALUE_CHARBOOL},
    {"", 0, 0},
};

// FUNCTION: REDLINE 0x0043df94
void Config::DefaultConf() {
    int i = 0;
    this->ClearConfValues();
    while (g_ConfDefault[i].name[0]) {
        Value *v = new Value();
        v->kind = g_ConfDefault[i].kind;
        strcpy(v->name, g_ConfDefault[i].name);
        strlwr(v->name);

        switch (v->kind) {
        case 0:
        case 3:
            v->value.integer = *(int *)g_ConfDefault[i].value;
            break;
        case 4:
            v->value.integer = *(bool *)g_ConfDefault[i].value;
            break;
        case 1:
            v->value.decimal = *(float *)g_ConfDefault[i].value;
            break;
        case 2:
            v->value.string = strdup((char *)g_ConfDefault[i].value);
            break;
        default:
            break;
        }

        this->StoreValue(v);

        ++i;
    }
}

// FUNCTION: REDLINE 0x00440E05
void Config::PopulateDefaults() {
    int i = 0;
    while (g_ConfDefault[i].name[0]) {
        bool created = false;
        Value *v = this->GetValue(g_ConfDefault[i].name);
        if (!v) {
            v = new Value();
            created = true;
        }
        v->kind = g_ConfDefault[i].kind;
        strcpy(v->name, g_ConfDefault[i].name);
        strlwr(v->name);
        switch (v->kind) {
        case 0:
        case 3:
            v->value.integer = *(int *)g_ConfDefault[i].value;
            break;
        case 4:
            v->value.integer = *(unsigned char *)g_ConfDefault[i].value != 0;
            break;
        case 1:
            v->value.decimal = *(float *)g_ConfDefault[i].value;
            break;
        case 2:
            if (v->value.string) {
                free(v->value.string);
                v->value.string = NULL;
            }
            v->value.string = strdup((char *)g_ConfDefault[i].value);
            break;
        default:
            break;
        }
        if (created)
            this->StoreValue(v);

        ++i;
    }
}

// FUNCTION: REDLINE 0x0043DF77
void Config::Default() {
    this->LoadPreset(0);
    this->DefaultConf();
};

// FUNCTION: REDLINE 0x0043DF05
int Config::Write() {
    FILE* file = fopen("redline.cfg", "w");
    if (file) {
        this->PopulateDefaults();
        this->WriteHeader(file);
        this->WriteMappings(file);
        this->WriteValues(file);
        fflush(file);
        fclose(file);
    }
    return 0;
}

// FUNCTION: REDLINE 0x0043EA54
void Config::WriteHeader(FILE* file) {
    if (!file)
        return;
    fprintf(file, "%s\n", "##########################################################");
    fprintf(file, "%s\n", "# Redline Configuration File");
    fprintf(file, "%s\n", "##########################################################");
}

// FUNCTION: REDLINE 0x0043EAAB
void Config::WriteMappings(FILE* file) {
    if (!file)
        return;

    fprintf(file, "\n%s\n", "################");
    fprintf(file, "%s\n", "# Input Mappings");
    fprintf(file, "%s\n\n", "################");

    short i;
    char* action_name;
    char* button_name;

    fprintf(file, "%s\n", "# Keyboard Mappings");
    for (i = 0; i < this->keybind_size; ++i) {
        if (this->keybinds_foot[i] > -1) {
            action_name = GetActionName(this->keybinds_foot[i]);
            button_name = GetKeyName(i);
            if (button_name && action_name) {
                fprintf(file, "map %s %s\n", button_name, action_name);
            } else if (button_name) {
                fprintf(file, "# Unable to locate impulse %d\n", this->keybinds_foot[i]);
            } else {
                fprintf(file, "# Unable to locate input %d\n", i);
            }
        }
    }

    fprintf(file, "%s\n", "# Mouse Mappings");
    for (i = 0; i < this->mousebind_size; ++i) {
        if (this->mousebinds_foot[i] > -1) {
            action_name = GetActionName(this->mousebinds_foot[i]);
            button_name = GetMbuttonName(i);
            if (button_name && action_name) {
                fprintf(file, "map %s %s\n", button_name, action_name);
            } else if (button_name) {
                fprintf(file, "# Unable to locate impulse %d\n", this->mousebinds_foot[i]);
            } else {
                fprintf(file, "# Unable to locate input %d\n", i);
            }
        }
    }

    fprintf(file, "%s\n", "# Joystick Mappings");
    for (i = 0; i < this->joybind_size; ++i) {
        if (this->joybinds_foot[i] > -1) {
            action_name = GetActionName(this->joybinds_foot[i]);
            button_name = GetJoybuttonName(i);
            if (button_name && action_name) {
                fprintf(file, "map %s %s\n", button_name, action_name);
            } else if (button_name) {
                fprintf(file, "# Unable to locate impulse %d\n", this->joybinds_foot[i]);
            } else {
                fprintf(file, "# Unable to locate input %d\n", i);
            }
        }
    }

    fprintf(file, "%s\n", "# Joystick Hat Mappings");
    for (i = 0; i < this->jhatbind_size; ++i) {
        if (this->jhatbinds_foot[i] > -1) {
            action_name = GetActionName(this->jhatbinds_foot[i]);
            button_name = GetJoyhatName(i);
            if (button_name && action_name) {
                fprintf(file, "map %s %s\n", button_name, action_name);
            } else if (button_name) {
                fprintf(file, "# Unable to locate impulse %d\n", this->jhatbinds_foot[i]);
            } else {
                fprintf(file, "# Unable to locate input %d\n", i);
            }
        }
    }

    fprintf(file, "%s\n", "# Car Keyboard Mappings");
    for (i = 0; i < this->car_keybind_size; ++i) {
        if (this->keybinds_car[i] > -1) {
            action_name = GetActionName(this->keybinds_car[i]);
            button_name = GetKeyName(i);
            if (button_name && action_name) {
                fprintf(file, "map car %s %s\n", button_name, action_name);
            } else if (button_name) {
                fprintf(file, "# Unable to locate impulse %d\n", this->keybinds_car[i]);
            } else {
                fprintf(file, "# Unable to locate input %d\n", i);
            }
        }
    }

    fprintf(file, "%s\n", "# Car Mouse Mappings");
    for (i = 0; i < this->car_mousebind_size; ++i) {
        if (this->mousebinds_car[i] > -1) {
            action_name = GetActionName(this->mousebinds_car[i]);
            button_name = GetMbuttonName(i);
            if (button_name && action_name) {
                fprintf(file, "map car %s %s\n", button_name, action_name);
            } else if (button_name) {
                fprintf(file, "# Unable to locate impulse %d\n", this->mousebinds_car[i]);
            } else {
                fprintf(file, "# Unable to locate input %d\n", i);
            }
        }
    }

    fprintf(file, "%s\n", "# Car Joystick Mappings");
    for (i = 0; i < this->car_joybind_size; ++i) {
        if (this->joybinds_car[i] > -1) {
            action_name = GetActionName(this->joybinds_car[i]);
            button_name = GetJoybuttonName(i);
            if (button_name && action_name) {
                fprintf(file, "map car %s %s\n", button_name, action_name);
            } else if (button_name) {
                fprintf(file, "# Unable to locate impulse %d\n", this->joybinds_car[i]);
            } else {
                fprintf(file, "# Unable to locate input %d\n", i);
            }
        }
    }

    fprintf(file, "%s\n", "# Car Joystick Hat Mappings");
    for (i = 0; i < this->car_jhatbind_size; ++i) {
        if (this->jhatbinds_car[i] > -1) {
            action_name = GetActionName(this->jhatbinds_car[i]);
            button_name = GetJoyhatName(i);
            if (button_name && action_name) {
                fprintf(file, "map car %s %s\n", button_name, action_name);
            } else if (button_name) {
                fprintf(file, "# Unable to locate impulse %d\n", this->jhatbinds_car[i]);
            } else {
                fprintf(file, "# Unable to locate input %d\n", i);
            }
        }
    }
}

// FUNCTION: REDLINE 0x0043F1C7
void Config::WriteValues(FILE* file) {
    if (!file)
        return;
    fprintf(file, "\n%s\n", "#############");
    fprintf(file, "%s\n", "# Definitions");
    fprintf(file, "%s\n\n", "#############");
    for (int i = 0; i < this->conf_size; ++i) {
        for (Value* v = this->conf_values[i]; v; v = v->next) {
            fprintf(file, "%s ", v->name);
            switch (v->kind) {
                case VALUE_SIGNED:
                case VALUE_INTBOOL:
                case VALUE_CHARBOOL:
                    fprintf(file, "%d\n", v->value.integer);
                    break;
                case VALUE_FLOAT:
                    fprintf(file, "%f\n", v->value.decimal);
                    break;
                case VALUE_STRING:
                    fprintf(file, "'%s'\n", v->value.string);
                    break;
                default:
                    fprintf(file, "Unknown data type\n");
                    break;
            }
        }
    }
}

// FUNCTION: REDLINE 0x0043da38
int Config::Load() {
    // TODO: Init some variables
    int read = -1;
    FILE *file = fopen("redline.cfg", "r");
    if (file == NULL) {
        this->Default();
        return -1;
    }

    char buf[128];
    read = fread(buf, 1, 128, file);

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

            if (this->ParseLine(&buf[line_start]) < 0) {
                g_Log.Debug("Unable to parse config line:");
                g_Log.Debug(&buf[line_start]);
            }
            cursor += 1;
            remaining -= cursor - line_start;
            line_start = cursor;
        }

        read = remaining;
        read += fread(&buf[cursor], 1, 128 - remaining, file);
        remaining = read;
    }
    for (int i = 0; i < 2; ++i) {
        int *arr1;
        int *arr2;
        int *arr3;
        int *arr4;
        if (i != 0) {
            arr1 = this->keybinds_car;
            arr2 = this->mousebinds_car;
            arr3 = this->joybinds_car;
            arr4 = this->jhatbinds_car;
        } else {
            arr1 = this->keybinds_foot;
            arr2 = this->mousebinds_foot;
            arr3 = this->joybinds_foot;
            arr4 = this->jhatbinds_foot;
        }

        for (int j = 0; j < 64; ++j) {
            int count = 0;
            int k;
            for (k = 0; k < 256; ++k) {
                if (arr1[k] == j)
                    count++;
                if (count > 4) {
                    char *key = GetKeyName(k);
                    char *act = GetActionName(j);
                    Warn("*Max Bind Exceeded (%d) for %s: Removing bind for %s",
                         key,
                         act);
                    arr1[k] = -1;
                    --count;
                }
            }
            for (k = 0; k < 3; ++k) {
                if (arr1[k] == j)
                    count++;
                if (count > 4) {
                    char *key = GetMbuttonName(k);
                    char *act = GetActionName(j);
                    Warn("*Max Bind Exceeded (%d) for %s: Removing bind for %s",
                         key,
                         act);
                    arr1[k] = -1;
                    --count;
                }
            }
            for (k = 0; k < 13; ++k) {
                if (arr1[k] == j)
                    count++;
                if (count > 4) {
                    char *key = GetJoybuttonName(k);
                    char *act = GetActionName(j);
                    Warn("*Max Bind Exceeded (%d) for %s: Removing bind for %s",
                         key,
                         act);
                    arr1[k] = -1;
                    --count;
                }
            }
            for (k = 0; k < 9; ++k) {
                if (arr1[k] == j)
                    count++;
                if (count > 4) {
                    char *key = GetJoyhatName(k);
                    char *act = GetActionName(j);
                    Warn("*Max Bind Exceeded (%d) for %s: Removing bind for %s",
                         key,
                         act);
                    arr1[k] = -1;
                    --count;
                }
            }
        }
    }

    fclose(file);
    return 0;
}

// GLOBAL: REDLINE 0x0058CB38
const char *g_CmdlineOptions[] = {"connect",
                                  "name",
                                  "team",
                                  "skin",
                                  "game",
                                  "ip",
                                  "host",
                                  "maxplayers",
                                  "hostname",
                                  "console",
                                  "lobby"};

enum CmdlineOptions {
    CMD_CONNECT = 0,
    CMD_NAME = 1,
    CMD_TEAM = 2,
    CMD_SKIN = 3,
    CMD_GAME = 4,
    CMD_IP = 5,
    CMD_HOST = 6,
    CMD_MAXPLAYERS = 7,
    CMD_HOSTNAME = 8,
    CMD_CONSOLE = 9,
    CMD_LOBBY = 10,
    CMD_UNK = 11,
};

// FUNCTION: REDLINE 0x00441149
char *Config::TokenizeCmdline(char *cmdline, int *out_token, char *out_val) {
    struct Locals {
        int i;
        bool quote;
        char *cursor;
        char buf[32];
    } l;
    if (!cmdline || !*cmdline)
        return NULL;
    l.cursor = cmdline;
    while (*l.cursor != '+' && *l.cursor)
        ++l.cursor;
    if (*l.cursor == NULL) {
        *out_token = CMD_UNK;
        return NULL;
    }
    l.cursor++;
    if (*l.cursor == NULL) {
        *out_token = CMD_UNK;
        return NULL;
    }
    l.i = 0;
    while (!isspace(*l.cursor) && *l.cursor)
        l.buf[l.i++] = *l.cursor++;
    l.buf[l.i] = 0;

    for (*out_token = 0; *out_token < CMD_UNK; ++*out_token) {
        if (!strcmp(l.buf, g_CmdlineOptions[*out_token]))
            break;
    }

    if (*out_token == CMD_UNK)
        return l.cursor;
    if (*out_token == CMD_CONSOLE) {
        *out_val = NULL;
        return l.cursor;
    }
    if (*out_token == CMD_LOBBY) {
        *out_val = NULL;
        return l.cursor;
    }
    while (isspace(*l.cursor) && *l.cursor)
        ++l.cursor;
    if (*l.cursor == NULL) {
        *out_token = CMD_UNK;
        return NULL;
    }
    l.i = 0;
    l.quote = false;
    while ((!isspace(*l.cursor) || l.quote) && *l.cursor) {
        if (*l.cursor == '"') {
            l.quote = !l.quote;
            ++l.cursor;
            continue;
        }
        out_val[l.i++] = *l.cursor;
        ++l.cursor;
    }
    out_val[l.i] = 0;
    return l.cursor;
}

// GLOBAL: REDLINE 0x005ce944
short g_ConnectPort = 0;
// GLOBAL: REDLINE 0x005cea46
char g_ConnectIP[128];

// GLOBAL: REDLINE 0x005ce8ff
bool g_ConnectRelated1;
// GLOBAL: REDLINE 0x005ce900
bool g_ConnectRelated2;

// FUNCTION: REDLINE 0x00441365
void Config::ProcessConnect(char *val) {
    struct Locals {
        char *c;
        int i;
    } l;
    g_ConnectRelated1 = true;
    g_ConnectRelated2 = false;
    l.i = 0;
    for (l.c = val; *l.c; ++l.c) {
        if (*l.c == ':') {
            g_ConnectPort = (short)atol(l.c + 1);
            break;
        }
        ++l.i;
    }
    strncpy(g_ConnectIP, val, l.i);
    g_ConnectIP[l.i] = 0;
}

// FUNCTION: REDLINE 0x00441439
void Config::ProcessHost(char *val) {
    g_ConnectRelated1 = true;
    g_ConnectRelated2 = true;
}

// FUNCTION: REDLINE 0x004413F0
void Config::ProcessPlayerName(char *val) {
    this->SetStringValue("Player", val);
}

// FUNCTION: REDLINE 0x0044140e
void Config::ProcessTeam(char *val) {}

// FUNCTION: REDLINE 0x0044141B
void Config::ProcessHostname(char *val) { strcpy(g_Net_GameName, val); }

// FUNCTION: REDLINE 0x00440FE1
bool Config::ProcessCmdline() {
    char *cmdline = GetCommandLineA();
    if (!cmdline || !*cmdline)
        return 0;
    Warn("Processing CMD Line: %s", cmdline);
    char *cursor = cmdline;
    while (cursor != NULL) {
        int opt;
        char val[128];
        cursor = this->TokenizeCmdline(cursor, &opt, val);
        int x = opt;
        switch (x) {
        case CMD_CONNECT:
            this->ProcessConnect(val);
            break;
        case CMD_NAME:
            this->ProcessPlayerName(val);
            break;
        case CMD_TEAM:
            this->ProcessTeam(val);
            break;
        case CMD_HOST:
            this->ProcessHost(val);
            break;
        case CMD_HOSTNAME:
            this->ProcessHostname(val);
            break;
        case CMD_CONSOLE:
            g_ConsoleEnabled = 1;
            return true;
        case CMD_LOBBY:
            g_LobbyEnabled = 1;
            return true;
        default:
            break;
        }
    }
    if (g_ConnectRelated1 || g_LobbyEnabled) {
        return true;
    }
    return false;
}

// FUNCTION: REDLINE 0x0043F9D3
int Config::ApplyKeybinds(bool car) {
    int i;
    UnbindAllKeybinds();
    // Apply foot keybindings
    for (i = 0; i < this->keybind_size; ++i)
        if (this->keybinds_foot[i] > -1)
            BindKey(i, this->keybinds_foot[i]);
    for (i = 0; i < this->mousebind_size; ++i)
        if (this->mousebinds_foot[i] > -1)
            BindMouse(i, this->mousebinds_foot[i]);
    for (i = 0; i < this->joybind_size; ++i)
        if (this->joybinds_foot[i] > -1)
            BindJoybutton(i, this->joybinds_foot[i]);
    for (i = 0; i < this->jhatbind_size; ++i)
        if (this->jhatbinds_foot[i] > -1)
            BindJoyhat(i, this->jhatbinds_foot[i]);

    if (car) {
        // Remove conflicting foot keybinds for car-bound actions
        for (i = 0; i < this->car_keybind_size; ++i)
            if (this->keybinds_car[i] > -1)
                UnbindAction(this->keybinds_car[i]);
        for (i = 0; i < this->car_mousebind_size; ++i)
            if (this->mousebinds_car[i] > -1)
                UnbindAction(this->mousebinds_car[i]);
        for (i = 0; i < this->car_joybind_size; ++i)
            if (this->joybinds_car[i] > -1)
                UnbindAction(this->joybinds_car[i]);
        for (i = 0; i < this->car_jhatbind_size; ++i)
            if (this->jhatbinds_car[i] > -1)
                UnbindAction(this->jhatbinds_car[i]);

        // Apply car keybindings
        for (i = 0; i < this->car_keybind_size; ++i)
            if (this->keybinds_car[i] > -1)
                BindKey(i, this->keybinds_car[i]);
        for (i = 0; i < this->car_mousebind_size; ++i)
            if (this->mousebinds_car[i] > -1)
                BindMouse(i, this->mousebinds_car[i]);
        for (i = 0; i < this->car_joybind_size; ++i)
            if (this->joybinds_car[i] > -1)
                BindJoybutton(i, this->joybinds_car[i]);
        for (i = 0; i < this->car_jhatbind_size; ++i)
            if (this->jhatbinds_car[i] > -1)
                BindJoyhat(i, this->jhatbinds_car[i]);
    }

    // Load globals
    Value *conf_val = this->GetValue("Mouse_Foot");
    if (conf_val != NULL)
        g_Mouse_Foot = conf_val->value.decimal;
    conf_val = this->GetValue("Mouse_Car");
    if (conf_val != NULL)
        g_Mouse_Car = conf_val->value.decimal;

    return 0;
}
