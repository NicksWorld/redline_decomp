#include "keybinds.h"

#include <windows.h>

struct NamedKey {
    int value;
    char name[16];
};

// GLOBAL: REDLINE 0x5906E0
NamedKey g_KeyNames[103] = {
    {1, "ESC"},    {2, "1"},       {3, "2"},      {4, "3"},      {5, "4"},
    {6, "5"},      {7, "6"},       {8, "7"},      {9, "8"},      {10, "9"},
    {11, "0"},     {12, "-"},      {13, "="},     {14, "BCKSP"}, {15, "TAB"},
    {16, "Q"},     {17, "W"},      {18, "E"},     {19, "R"},     {20, "T"},
    {21, "Y"},     {22, "U"},      {23, "I"},     {24, "O"},     {25, "P"},
    {26, "["},     {27, "]"},      {28, "RET"},   {29, "LCTRL"}, {30, "A"},
    {31, "S"},     {32, "D"},      {33, "F"},     {34, "G"},     {35, "H"},
    {36, "J"},     {37, "K"},      {38, "L"},     {39, ";"},     {40, "'"},
    {41, "`"},     {42, "LSHIFT"}, {43, "\\"},    {44, "Z"},     {45, "X"},
    {46, "C"},     {47, "V"},      {48, "B"},     {49, "N"},     {50, "M"},
    {51, ","},     {52, "."},      {53, "/"},     {54, "RSHFT"}, {55, "MULT"},
    {56, "LALT"},  {57, "SP"},     {58, "CAPS"},  {59, "F1"},    {60, "F2"},
    {61, "F3"},    {62, "F4"},     {63, "F5"},    {64, "F6"},    {65, "F7"},
    {66, "F8"},    {67, "F9"},     {68, "F10"},   {69, "NUM"},   {70, "SCRL"},
    {71, "PAD7"},  {72, "PAD8"},   {73, "PAD9"},  {74, "SUB"},   {75, "PAD4"},
    {76, "PAD5"},  {77, "PAD6"},   {78, "ADD"},   {79, "PAD1"},  {80, "PAD2"},
    {81, "PAD3"},  {82, "PAD0"},   {83, "DEC"},   {84, "F11"},   {85, "F12"},
    {86, "ENT"},   {87, "RCTRL"},  {88, "DIV"},   {89, "SYS"},   {90, "RALT"},
    {91, "HOME"},  {92, "UP"},     {93, "PGUP"},  {94, "LEFT"},  {95, "RIGHT"},
    {96, "END"},   {97, "DOWN"},   {98, "PGDN"},  {99, "INS"},   {100, "DEL"},
    {101, "LWIN"}, {102, "RWIN"},  {103, "APPS"},
};

// GLOBAL: REDLINE 0x00590ef0
NamedKey g_MbuttonNames[3] = {
    {0, "MOUSE_LFT"},
    {1, "MOUSE_RGHT"},
    {2, "MOUSE_MID"},
};

// GLOBAL: REDLINE 0x00590f30
NamedKey g_JoyNames[13] = {
    {0, "JB1"},   {1, "JB2"},   {2, "JB3"},   {3, "JB4"}, {4, "JB5"},
    {5, "JB6"},   {6, "JB7"},   {7, "JB8"},   {8, "JB9"}, {9, "JB10"},
    {10, "JB11"}, {11, "JB12"}, {12, "JB13"},
};

// GLOBAL: REDLINE 0x00591038
NamedKey g_JhatNames[9] = {
    {0, "JHATD"}, {1, "JHATN"},  {2, "JHATNE"}, {3, "JHATE"},  {4, "JHATSE"},
    {5, "JHATS"}, {6, "JHATSW"}, {7, "JHATW"},  {8, "JHATNW"},
};

// GLOBAL: REDLINE 0x005910F0
NamedKey g_ActionNames[64] = {
    {0, "FIRE"},          {1, "CHANGE_WEAPON"}, {2, "AIM"},
    {3, "AIM_UP"},        {4, "AIM_DOWN"},      {5, "DIVE_LEFT"},
    {6, "DIVE_RIGHT"},    {7, "DUCK"},          {8, "JUMP"},
    {9, "PARKING_BRAKE"}, {10, "STRAFE"},       {11, "STRAFE_LEFT"},
    {12, "STRAFE_RIGHT"}, {13, "AIM_CLEAR"},    {14, "SUICIDE"},
    {15, "RUN"},          {16, "FORWARD"},      {17, "BACKWARD"},
    {18, "RIGHT"},        {19, "LEFT"},         {20, "LOOK"},
    {21, "WEAPON_1"},     {22, "WEAPON_2"},     {23, "WEAPON_3"},
    {24, "WEAPON_4"},     {25, "WEAPON_5"},     {26, "WEAPON_6"},
    {27, "WEAPON_7"},     {28, "WEAPON_8"},     {29, "WEAPON_9"},
    {30, "WEAPON_10"},    {31, "ZOOM_IN"},      {32, "ZOOM_OUT"},
    {33, "EXIT"},         {34, "FIRE_LEFT"},    {35, "FIRE_RIGHT"},
    {36, "FIRE_REAR"},    {37, "CAMERA_IN"},    {38, "CAMERA_BEHIND"},
    {39, "CAMERA_FRONT"}, {40, "CAMERA_SAT"},   {41, "CHANGE_CAR"},
    {42, "LOOK_LEFT"},    {43, "LOOK_RIGHT"},   {44, "LOOK_REAR"},
    {45, "NEXT_TARGET"},  {46, "PREV_TARGET"},  {47, "47"},
    {48, "48"},           {49, "49"},           {50, "50"},
    {51, "51"},           {52, "52"},           {53, "53"},
    {54, "54"},           {55, "55"},           {56, "56"},
    {57, "57"},           {58, "58"},           {59, "59"},
    {60, "60"},           {61, "61"},           {62, "62"},
    {63, "63"},
};

// GLOBAL: REDLINE 0x005C3F4C
Keybinds *g_Keybinds = NULL;

// FUNCTION: REDLINE 0x0043C680
bool InitKeybinds() {
    if (g_Keybinds != NULL)
        return 1;
    g_Keybinds = new Keybinds();
    if (g_Keybinds != NULL)
        return true;
    return false;
}

// FUNCTION: REDLINE 0x00496ab0
Keybinds::Keybinds() {
    memset(this->unk, -1, sizeof(this->unk));
    memset(this->unka, 0, sizeof(this->unka));

    // TODO: Set values in unknown segments

    this->Init();
}

// FUNCTION: REDLINE 0x00498742
int Keybinds::Init() {
    int i;
    for (i = 0; i < 0xff; ++i)
        this->key_names[i] = NULL;
    for (i = 0; i < 103; ++i)
        this->key_names[g_KeyNames[i].value] = g_KeyNames[i].name;

    for (i = 0; i < 3; ++i)
        this->mbutton_names[i] = NULL;
    for (i = 0; i < 3; ++i)
        this->mbutton_names[g_MbuttonNames[i].value] = g_MbuttonNames[i].name;

    for (i = 0; i < 13; ++i)
        this->joybutton_names[i] = NULL;
    for (i = 0; i < 13; ++i)
        this->joybutton_names[g_JoyNames[i].value] = g_JoyNames[i].name;

    for (i = 0; i < 9; ++i)
        this->joyhat_names[i] = NULL;
    for (i = 0; i < 9; ++i) {
        // BUG: Uses indices from JoyNames instead of JhatNames
        this->joyhat_names[g_JoyNames[i].value] = g_JhatNames[i].name;
    }

    for (i = 0; i < 64; ++i)
        this->action_names[i] = NULL;
    for (i = 0; i < 64; ++i)
        this->action_names[g_ActionNames[i].value] = g_ActionNames[i].name;

    memset(this->unk1, 0, sizeof(this->unk1));
    memset(this->unk2, 0, sizeof(this->unk2));

    this->unk1[1] = 1;
    this->unk1[2] = 2;
    this->unk1[3] = 3;
    this->unk1[4] = 4;
    this->unk1[5] = 5;
    this->unk1[6] = 6;
    this->unk1[7] = 7;
    this->unk1[8] = 8;
    this->unk1[9] = 9;
    this->unk1[10] = 10;
    this->unk1[11] = 11;
    this->unk1[12] = 12;
    this->unk1[13] = 13;
    this->unk1[14] = 14;
    this->unk1[15] = 15;
    this->unk1[16] = 16;
    this->unk1[17] = 17;
    this->unk1[18] = 18;
    this->unk1[19] = 19;
    this->unk1[20] = 20;
    this->unk1[21] = 21;
    this->unk1[22] = 22;
    this->unk1[23] = 23;
    this->unk1[24] = 24;
    this->unk1[25] = 25;
    this->unk1[26] = 26;
    this->unk1[27] = 27;
    this->unk1[28] = 28;
    this->unk1[29] = 29;
    this->unk1[30] = 30;
    this->unk1[31] = 31;
    this->unk1[32] = 32;
    this->unk1[33] = 33;
    this->unk1[34] = 34;
    this->unk1[35] = 35;
    this->unk1[36] = 36;
    this->unk1[37] = 37;
    this->unk1[38] = 38;
    this->unk1[39] = 39;
    this->unk1[40] = 40;
    this->unk1[41] = 41;
    this->unk1[42] = 42;
    this->unk1[43] = 43;
    this->unk1[44] = 44;
    this->unk1[45] = 45;
    this->unk1[46] = 46;
    this->unk1[47] = 47;
    this->unk1[48] = 48;
    this->unk1[49] = 49;
    this->unk1[50] = 50;
    this->unk1[51] = 51;
    this->unk1[52] = 52;
    this->unk1[53] = 53;
    this->unk1[54] = 54;
    this->unk1[55] = 55;
    this->unk1[56] = 56;
    this->unk1[57] = 57;
    this->unk1[58] = 58;
    this->unk1[59] = 59;
    this->unk1[60] = 60;
    this->unk1[61] = 61;
    this->unk1[62] = 62;
    this->unk1[63] = 63;
    this->unk1[64] = 64;
    this->unk1[65] = 65;
    this->unk1[66] = 66;
    this->unk1[67] = 67;
    this->unk1[68] = 68;
    this->unk1[69] = 69;
    this->unk1[70] = 70;
    this->unk1[71] = 71;
    this->unk1[72] = 72;
    this->unk1[73] = 73;
    this->unk1[74] = 74;
    this->unk1[75] = 75;
    this->unk1[76] = 76;
    this->unk1[77] = 77;
    this->unk1[78] = 78;
    this->unk1[79] = 79;
    this->unk1[80] = 80;
    this->unk1[81] = 81;
    this->unk1[82] = 82;
    this->unk1[83] = 83;
    this->unk1[84] = 87;
    this->unk1[85] = 88;
    this->unk1[86] = 156;
    this->unk1[87] = 157;
    this->unk1[88] = 181;
    this->unk1[89] = 183;
    this->unk1[90] = 184;
    this->unk1[91] = 199;
    this->unk1[92] = 200;
    this->unk1[93] = 201;
    this->unk1[94] = 203;
    this->unk1[95] = 205;
    this->unk1[96] = 207;
    this->unk1[97] = 208;
    this->unk1[98] = 209;
    this->unk1[99] = 210;
    this->unk1[100] = 211;
    this->unk1[101] = 219;
    this->unk1[102] = 220;
    this->unk1[103] = 221;

    for (i = 0; i < 104; ++i)
        this->unk2[this->unk1[i]] = i;

    return 0;
}

// FUNCTION: REDLINE 0x0043cace
int LookupKeyname(char *name) { return g_Keybinds->LookupKeyName(name); }

// FUNCTION: REDLINE 0x00498ef7
int Keybinds::LookupKeyName(char *name) {
    if (name == NULL)
        return -1;
    strupr(name);
    for (int i = 0; i < 255; ++i) {
        if (this->key_names[i] != NULL && *name == *this->key_names[i]) {
            if (strlen(name) == 1) {
                if (this->key_names[i][1] == NULL)
                    return i;
            } else if (strcmp(this->key_names[i], name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

// FUNCTION: REDLINE 0x0043caf6
int LookupMbuttonName(char *name) {
    return g_Keybinds->LookupMbuttonName(name);
}

// FUNCTION: REDLINE 0x00498ff2
int Keybinds::LookupMbuttonName(char *name) {
    if (name == NULL)
        return -1;
    strupr(name);
    for (int i = 0; i < 3; ++i) {
        if (this->mbutton_names[i] != NULL &&
            *name == *this->mbutton_names[i]) {
            if (strlen(name) == 1) {
                if (this->mbutton_names[i][1] == NULL)
                    return i;
            } else if (strcmp(this->mbutton_names[i], name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

// FUNCTION: REDLINE 0x0043cb1e
int LookupJoybuttonName(char *name) {
    return g_Keybinds->LookupJoybuttonName(name);
}

// FUNCTION: REDLINE 0x004990ea
int Keybinds::LookupJoybuttonName(char *name) {
    if (name == NULL)
        return -1;
    strupr(name);
    for (int i = 0; i < 13; ++i) {
        if (this->joybutton_names[i] != NULL &&
            *name == *this->joybutton_names[i]) {
            if (strlen(name) == 1) {
                if (this->joybutton_names[i][1] == NULL)
                    return i;
            } else if (strcmp(this->joybutton_names[i], name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

// FUNCTION: REDLINE 0x0043cb46
int LookupJoyhatName(char *name) { return g_Keybinds->LookupJoyhatName(name); }

// FUNCTION: REDLINE 0x004991e2
int Keybinds::LookupJoyhatName(char *name) {
    if (name == NULL)
        return -1;
    strupr(name);
    for (int i = 0; i < 9; ++i) {
        if (this->joyhat_names[i] != NULL && *name == *this->joyhat_names[i]) {
            if (strlen(name) == 1) {
                if (this->joyhat_names[i][1] == NULL)
                    return i;
            } else if (strcmp(this->joyhat_names[i], name) == 0) {
                return i;
            }
        }
    }
    return -1;
}

// FUNCTION: REDLINE 0x0043cb6e
int LookupActionName(char *name) { return g_Keybinds->LookupActionName(name); }

// FUNCTION: REDLINE 0x004992da
int Keybinds::LookupActionName(char *str) {
    if (str == NULL)
        return -1;
    strupr(str);
    for (int i = 0; i < 64; ++i) {
        if (this->action_names[i] != NULL && *str == *this->action_names[i] &&
            strcmp(this->action_names[i], str) == 0) {
            return i;
        }
    }
    return -1;
}

// FUNCTION: REDLINE 0x0043CAE2
char* GetKeyName(int idx) {
    return g_Keybinds->KeyName(idx);
}

// FUNCTION: REDLINE 0x00498FC4
char* Keybinds::KeyName(int idx) {
    if (this->InvalidKeyIndex(idx))
        return NULL;
    return this->key_names[idx];
}

// FUNCTION: REDLINE 0x0049863C
int Keybinds::InvalidKeyIndex(int idx) {
    if (idx < 0 || idx > 0x100) return true;
    return false;
}

// FUNCTION: REDLINE 0x0043CB82
char* GetActionName(int idx) {
    return g_Keybinds->ActionName(idx);
}

// FUNCTION: REDLINE 0x00499371
char* Keybinds::ActionName(int idx) {
    if (this->InvalidActionIndex(idx))
        return NULL;
    return this->action_names[idx];
}

// FUNCTION: REDLINE 0x00498661
int Keybinds::InvalidActionIndex(int idx) {
    if (idx < 0 || idx >= 0x40) return true;
    return false;
}

// FUNCTION: REDLINE 0x0043CB0A
char* GetMbuttonName(int idx) {
    return g_Keybinds->MbuttonName(idx);
}

// FUNCTION: REDLINE 0x004990BC
char* Keybinds::MbuttonName(int idx) {
    if (this->InvalidMbuttonIndex(idx))
        return NULL;
    return this->mbutton_names[idx];
}

// FUNCTION: REDLINE 0x00498683
int Keybinds::InvalidMbuttonIndex(int idx) {
    if (idx < 0 || idx >= 3) return true;
    return false;
}

// FUNCTION: REDLINE 0x0043CB32
char* GetJoybuttonName(int idx) {
    return g_Keybinds->JoybuttonName(idx);
}

// FUNCTION: REDLINE 0x004991B4
char* Keybinds::JoybuttonName(int idx) {
    if (this->InvalidJoybuttonIndex(idx))
        return NULL;
    return this->joybutton_names[idx];
}

// FUNCTION: REDLINE 0x004986A5
int Keybinds::InvalidJoybuttonIndex(int idx) {
    if (idx < 0 || idx >= 13) return true;
    return false;
}

// FUNCTION: REDLINE 0x0043CB5A
char* GetJoyhatName(int idx) {
    return g_Keybinds->JoyhatName(idx);
}

// FUNCTION: REDLINE 0x004992AC
char* Keybinds::JoyhatName(int idx) {
    if (this->InvalidJoyhatIndex(idx))
        return NULL;
    return this->joyhat_names[idx];
}

// FUNCTION: REDLINE 0x004986C7
int Keybinds::InvalidJoyhatIndex(int idx) {
    if (idx < 0 || idx >= 9) return true;
    return false;
}
