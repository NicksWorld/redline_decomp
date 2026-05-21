#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "keybinds.h"
#include "log.h"

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
    this->unused_size6 = 256;
    this->keybinds_car = new int[this->keybind_size];
    this->unused_size7 = 3;
    this->mousebinds_car = new int[this->mousebind_size];
    this->unused_size8 = 13;
    this->joybinds_car = new int[this->joybind_size];
    this->unused_size9 = 9;
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
    bool car = false;
    int action = -1;
    int joyhat = action;
    int joybutton = joyhat;
    int mbutton = joybutton;
    int key = mbutton;

    // Skip whitespace
    while (isspace(*mapping) && *mapping != NULL)
        ++mapping;
    if (*mapping == NULL)
        return -1;

    if (mapping[0] == 'c' && mapping[1] == 'a' && mapping[3] == 'r') {
        mapping += 3;
        car = true;
        while (isspace(*mapping) && *mapping != NULL)
            ++mapping;
    }

    char buf[128];
    int i;

    // Copy remaining line
    for (i = 0; i < 127; ++i) {
        if (isspace(mapping[i]))
            break;
        buf[i] = mapping[i];
    }
    if (i == 127)
        return -1;
    buf[i] = 0;

    key = LookupKeyname(buf);
    if (key < 0) {
        strlwr(buf);
        mbutton = LookupMbuttonName(buf);
        if (mbutton < 0) {
            strlwr(buf);
            joybutton = LookupJoybuttonName(buf);
            if (joybutton < 0) {
                strlwr(buf);
                joyhat = LookupJoyhatName(buf);
            }
        }
    }
    if (key < 0 && mbutton < 0 && joybutton < 0 && joyhat < 0)
        return -1;

    while (isspace(mapping[i]) && i < 127)
        ++i;
    if (i == 127)
        return -1;

    // Copy action name into buf
    int buf_idx;
    for (buf_idx = 0; i < 127; ++i, ++buf_idx) {
        if (isspace(mapping[i]) || mapping[i] == NULL)
            break;
        buf[buf_idx] = mapping[i];
    }
    buf[buf_idx] = 0;

    action = LookupActionName(buf);
    if (action < 0)
        return -1;

    if (key >= 0) {
        if (car)
            this->keybinds_car[key] = action;
        else
            this->keybinds_foot[key] = action;
    } else if (mbutton >= 0) {
        if (car)
            this->mousebinds_car[mbutton] = action;
        else
            this->mousebinds_foot[mbutton] = action;
    } else if (joybutton >= 0) {
        if (car)
            this->joybinds_car[joybutton] = action;
        else
            this->joybinds_foot[joybutton] = action;
    } else {
        if (car)
            this->jhatbinds_car[joyhat] = action;
        else
            this->jhatbinds_foot[joyhat] = action;
    }

    return 0;
}

// FUNCTION: REDLINE 0x0043d3a0
Value::Value() {
    this->line[0] = 0;
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

// FUNCTION: REDLINE 0x00440c42
int Config::StoreValue(Value *v) {
    if (v == NULL)
        return -1;
    int i = v->line[0];
    Value *prev_head = this->conf_values[i];
    if (prev_head != NULL)
        v->next = prev_head;
    else
        v->next = NULL;
    this->conf_values[i] = v;
    return 0;
}

// FUNCTION: REDLINE 0x004408BD
int Config::ParseOther(char *line) {
    // TODO: free is unlikely to be the real call on line_copy
    int integer;
    float decimal;

    const char seperators[6] = " \b\r\t\n";
    const char line_endings[5] = "\b\r\t\n"; // Note exclusion of space

    int kind = -1;
    while (isspace(*line) && *line != NULL)
        ++line;
    if (*line == NULL)
        return -1;

    char *line_copy = strdup(line); // Needs freed
    char *sep = strtok(line_copy, seperators);
    if (sep == NULL) {
        free(line_copy);
        return -1;
    }
    while (isspace(*line) && *line != NULL)
        ++line;

    if (*line == NULL) {
        free(line_copy);
        return -1;
    }
    char *line_end = strtok(NULL, line_endings);
    if (line_end == NULL) {
        free(line_copy);
        return -1;
    }
    void *unk2 = NULL;
    if (*line_end == '\'') {
        kind = 2;
        ++line_end;
    } else if (strstr(line_end, ".")) {
        char *after_dot = strstr(line_end, ".");
        after_dot++;
        unk2 = strstr(line_end, ".");
        if (unk2 != NULL) {
            decimal = (float)atof(line_end);
            if (decimal != 0.0) {
                kind = 1;
            } else if (strstr(line_end, "0.0")) {
                kind = 1;
            }
        }
    }

    if (kind < 0 && unk2 == NULL) {
        integer = atol(line_end);
        if (integer != 0) {
            kind = 0;
        } else if (*line_end == '0') {
            kind = 0;
        }
    }
    if (kind == -1)
        kind = 2;

    Value *v = new Value();

    // int v19 = -1;
    if (strlen(sep) >= 32) {
        strncpy(v->line, sep, 30);
        v->line[31] = 0;
    } else {
        strcpy(v->line, sep);
    }
    // TODO: switch
    int len;
    switch (kind) {
    case 1:
        v->value.decimal = decimal;
        break;
    case 0:
    case 3:
        v->value.integer = integer;
        break;
    case 2:
        v->value.string = strdup(line_end);
        len = strlen(v->value.string);
        if (len > 0 && v->value.string[len - 1] == '\'')
            v->value.string[len - 1] = 0;
        break;
    case -1:
        free(line_copy);
        return -1;
    }
    v->kind = kind;
    free(line_copy);
    return this->StoreValue(v);
    free(line_copy);
    return -1;
}

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

struct DefaultConfValue {
    char name[128];
    void *value;
    int kind;
};

// TODO: Second value needs to be a pointer to the respective global.
// This may take a long while
// GLOBAL: REDLINE 0x005985e8
DefaultConfValue g_DefaultConf[] = {
    {"D3DSound", 0, VALUE_UNSIGNED},
    {"Mouse_Foot", 0, VALUE_FLOAT},
    {"Mouse_Car", 0, VALUE_FLOAT},
    {"FreeLook", 0, VALUE_UNSIGNED},
    {"CarFreeLook", 0, VALUE_UNSIGNED},
    {"ReverseYAxis", 0, VALUE_UNSIGNED},
    {"ReverseYAxisCar", 0, VALUE_UNSIGNED},
    {"ScreenWidth", 0, VALUE_SIGNED},
    {"ScreenHeight", 0, VALUE_SIGNED},
    {"ScreenBPP", 0, VALUE_SIGNED},
    {"ScreenGamma", 0, VALUE_SIGNED},
    {"MipMapping", 0, VALUE_SIGNED},
    {"DrawShadows", 0, VALUE_UNSIGNED},
    {"DisplayParticles", 0, VALUE_SIGNED},
    {"DisplayTireTreads", 0, VALUE_UNSIGNED},
    {"DisplayScreenFlash", 0, VALUE_UNSIGNED},
    {"DifficultyLevel", 0, VALUE_SIGNED},
    {"EnableFog", 0, VALUE_UNSIGNED},
    {"EnviroMapping", 0, VALUE_UNSIGNED},
    {"GroundLighting", 0, VALUE_UNSIGNED},
    {"PalettedTextures", 0, VALUE_UNSIGNED},
    {"TextureDetail", 0, VALUE_SIGNED},
    {"TripleBuffer", 0, VALUE_UNSIGNED},
    {"LimitParticleSize", 0, VALUE_UNSIGNED},
    {"DXtextureManager", 0, VALUE_UNSIGNED},
    {"DitherEnable", 0, VALUE_UNSIGNED},
    {"WeaponDisplayMin", 0, VALUE_UNSIGNED},
    {"PersonGunDraw", 0, VALUE_UNSIGNED},
    {"carDashboardDraw", 0, VALUE_UNSIGNED},
    {"turretDashboardDraw", 0, VALUE_SIGNED},
    {"carCamMode", 0, VALUE_SIGNED},
    {"turretCamMode", 0, VALUE_SIGNED},
    {"turretFollowCamMode", 0, VALUE_SIGNED},
    {"carFollowCamMode", 0, VALUE_SIGNED},
    {"mouseWheel", 0, VALUE_SIGNED},
    {"carMouseWheel", 0, VALUE_SIGNED},
    {"carSteerInc", 0, VALUE_SIGNED},
    {"carSteerMax", 0, VALUE_SIGNED},
    {"footSteerInc", 0, VALUE_SIGNED},
    {"footSteerMax", 0, VALUE_SIGNED},
    {"soundChannels", 0, VALUE_SIGNED},
    {"cruiseControl", 0, VALUE_SIGNED},
    {"LastMap", 0, VALUE_STRING},
    {"DisplayDevice", 0, VALUE_STRING},
    {"DeviceDriver", 0, VALUE_STRING},
    {"Net_GameName", 0, VALUE_STRING},
    {"Net_Mode_ScoreLimitOn", 0, VALUE_BOOL},
    {"Net_Mode_ScoreLimit", 0, VALUE_SIGNED},
    {"Net_Mode_TimeLimitOn", 0, VALUE_BOOL},
    {"Net_Mode_TimeLimit", 0, VALUE_SIGNED},
    {"Net_SmartCrosshair", 0, VALUE_BOOL},
    {"Net_Teams", 0, VALUE_BOOL},
    {"Net_FriendlyFire", 0, VALUE_BOOL},
    {"Net_TeamPlace", 0, VALUE_BOOL},
    {"Net_Mode_CTF", 0, VALUE_BOOL},
    {"Net_Mode_CTF_FlagDrop", 0, VALUE_BOOL},
    {"Net_Mode_CTF_Adv", 0, VALUE_BOOL},
    {"Net_LastIP", 0, VALUE_STRING},
    {"Net_PlayerTeam", 0, VALUE_SIGNED},
    {"Net_PlayerSkel", 0, VALUE_SIGNED},
    {"Net_MaxPlayers", 0, VALUE_SIGNED},
    {"Net_ConsoleTCP", 0, VALUE_BOOL},
    {"Joystick_UpDown", 0, VALUE_FLOAT},
    {"Joystick_LeftRight", 0, VALUE_FLOAT},
    {"Joystick_FreeLook", 0, VALUE_UNSIGNED},
    {"Master_Volume", 0, VALUE_FLOAT},
    {"CarJoystick_UpDown", 0, VALUE_FLOAT},
    {"CarJoystick_LeftRight", 0, VALUE_FLOAT},
    {"Joystick_DeadZoneX", 0, VALUE_SIGNED},
    {"Joystick_DeadZoneY", 0, VALUE_SIGNED},
    {"CDAudio_Active", 0, VALUE_BOOL},
    {"IntroVideo", 0, VALUE_BOOL},
    // Debug/Inaccessible?
    {"", 0, VALUE_SIGNED},
    {"DebugMouse", 0, VALUE_UNSIGNED},
    {"DebugFrames", 0, VALUE_UNSIGNED},
    {"NumFrames", 0, VALUE_SIGNED},
    {"PlayDemo", 0, VALUE_UNSIGNED},
    {"RecordDemo", 0, VALUE_UNSIGNED},
    {"Windowed", 0, VALUE_UNSIGNED},
    {"AIActive", 0, VALUE_UNSIGNED},
    {"QuickRun", 0, VALUE_UNSIGNED},
    {"CreateUseFile", 0, VALUE_UNSIGNED},
    {"replayRecord", 0, VALUE_SIGNED},
    {"replayPlay", 0, VALUE_SIGNED},
    {"Net_Perf_CliSendFrames", 0, VALUE_SIGNED},
    {"Net_Perf_ServSendFrames", 0, VALUE_SIGNED},
    {"Net_Perf_CliInterp", 0, VALUE_BOOL},
    {"Net_Perf_CliPredict", 0, VALUE_BOOL},
    {"Net_Perf_ExtraLatencyOn", 0, VALUE_BOOL},
    {"Net_Perf_ExtraLatency", 0, VALUE_SIGNED},
    {"Net_FastStart", 0, VALUE_BOOL},
    {"Net_FastHost", 0, VALUE_BOOL},
};

// FUNCTION: REDLINE 0x0043df94
void Config::DefaultConf() {
    int i = 0;
    this->ClearConfValues();
    while (g_DefaultConf[i].name[0]) {
        Value *v = new Value();
        v->kind = g_DefaultConf[i].kind;
        strcpy(v->line, g_DefaultConf[i].name);
        strlwr(v->line);

        switch (v->kind) {
        case 0:
        case 3:
            v->value.integer = *(int *)g_DefaultConf[i].value;
            break;
        case 4:
            v->value.integer = *(bool *)g_DefaultConf[i].value;
            break;
        case 1:
            v->value.decimal = *(float *)g_DefaultConf[i].value;
            break;
        case 2:
            v->value.string = strdup((char *)g_DefaultConf[i].value);
            break;
        default:
            break;
        }

        this->StoreValue(v);

        ++i;
    }
}

// FUNCTION: REDLINE 0x0043DF77
void Config::Default() {
    this->LoadPreset(0);
    this->DefaultConf();
};

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
                         key, act);
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
                         key, act);
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
                         key, act);
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
                         key, act);
                    arr1[k] = -1;
                    --count;
                }
            }
        }
    }

    fclose(file);
    return 0;
}
