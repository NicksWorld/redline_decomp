#include "keybinds.h"

#include <windows.h>
#include <dinput.h>

// REMOVE imports
#include "globals.h"
#include "log.h"
#include <stdio.h>

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
    {41, "`"},     {42, "LSHFT"}, {43, "\\"},    {44, "Z"},     {45, "X"},
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
    memset(this->key_map, -1, sizeof(this->key_map));
    memset(this->action_map, 0, sizeof(this->action_map));

    // TODO: Set values in unknown segments
    this->input_devices = NULL;
    this->input_device_count = 0;
    this->input_devices_max = 32;
    this->unk = 0;
    this->dinput = NULL;

    this->Init();
}

// FUNCTION: REDLINE 0x004986E9
int Keybinds::KeyToInput(int key) {
    return this->key_to_scancode[key];
}

// FUNCTION: REDLINE 0x0043C9B2
int BindKey(int key, int action) {
    return g_Keybinds->BindKey(key, action);
}

// FUNCTION: REDLINE 0x00498176
int Keybinds::BindKey(int key, int action) {
    if (this->InvalidKeyIndex(key)) return 1;
    int scancode = this->KeyToInput(key);
    return this->Bind(scancode, action);
}

// FUNCTION: REDLINE 0x00498703
int Keybinds::MouseToInput(int mbutton) {
    return mbutton + 256;
}

// FUNCTION: REDLINE 0x0043C9DE
int BindMouse(int mbutton, int action) {
    return g_Keybinds->BindMouse(mbutton, action);
}

// FUNCTION: REDLINE 0x004981BB
int Keybinds::BindMouse(int mbutton, int action) {
    if (this->InvalidMbuttonIndex(mbutton))
        return 1;
    int input = this->MouseToInput(mbutton);
    return this->Bind(input, action);
}

// FUNCTION: REDLINE 0x00498718
int Keybinds::JoybuttonToInput(int button) {
    return button + 259;
}

// FUNCTION: REDLINE 0x0043CA0A
int BindJoybutton(int button, int action) {
    return g_Keybinds->BindJoybutton(button, action);
}

// FUNCTION: REDLINE 0x00498200
int Keybinds::BindJoybutton(int button, int action) {
    if (this->InvalidJoybuttonIndex(button)) return 1;
    int input = this->JoybuttonToInput(button);
    return this->Bind(input, action);
}

// FUNCTION: REDLINE 0x0049872D
int Keybinds::JoyhatToInput(int button) {
    return button + 272;
}

// FUNCTION: REDLINE 0x0043CA36
int BindJoyhat(int button, int action) {
    return g_Keybinds->BindJoyhat(button, action);
}

// FUNCTION: REDLINE 0x00498245
int Keybinds::BindJoyhat(int button, int action) {
    if (this->InvalidJoyhatIndex(button)) return 1;
    int input = this->JoyhatToInput(button);
    return this->Bind(input, action);
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

    memset(this->key_to_scancode, 0, sizeof(this->key_to_scancode));
    memset(this->scancode_to_key, 0, sizeof(this->scancode_to_key));

    this->key_to_scancode[1] = 1;
    this->key_to_scancode[2] = 2;
    this->key_to_scancode[3] = 3;
    this->key_to_scancode[4] = 4;
    this->key_to_scancode[5] = 5;
    this->key_to_scancode[6] = 6;
    this->key_to_scancode[7] = 7;
    this->key_to_scancode[8] = 8;
    this->key_to_scancode[9] = 9;
    this->key_to_scancode[10] = 10;
    this->key_to_scancode[11] = 11;
    this->key_to_scancode[12] = 12;
    this->key_to_scancode[13] = 13;
    this->key_to_scancode[14] = 14;
    this->key_to_scancode[15] = 15;
    this->key_to_scancode[16] = 16;
    this->key_to_scancode[17] = 17;
    this->key_to_scancode[18] = 18;
    this->key_to_scancode[19] = 19;
    this->key_to_scancode[20] = 20;
    this->key_to_scancode[21] = 21;
    this->key_to_scancode[22] = 22;
    this->key_to_scancode[23] = 23;
    this->key_to_scancode[24] = 24;
    this->key_to_scancode[25] = 25;
    this->key_to_scancode[26] = 26;
    this->key_to_scancode[27] = 27;
    this->key_to_scancode[28] = 28;
    this->key_to_scancode[29] = 29;
    this->key_to_scancode[30] = 30;
    this->key_to_scancode[31] = 31;
    this->key_to_scancode[32] = 32;
    this->key_to_scancode[33] = 33;
    this->key_to_scancode[34] = 34;
    this->key_to_scancode[35] = 35;
    this->key_to_scancode[36] = 36;
    this->key_to_scancode[37] = 37;
    this->key_to_scancode[38] = 38;
    this->key_to_scancode[39] = 39;
    this->key_to_scancode[40] = 40;
    this->key_to_scancode[41] = 41;
    this->key_to_scancode[42] = 42;
    this->key_to_scancode[43] = 43;
    this->key_to_scancode[44] = 44;
    this->key_to_scancode[45] = 45;
    this->key_to_scancode[46] = 46;
    this->key_to_scancode[47] = 47;
    this->key_to_scancode[48] = 48;
    this->key_to_scancode[49] = 49;
    this->key_to_scancode[50] = 50;
    this->key_to_scancode[51] = 51;
    this->key_to_scancode[52] = 52;
    this->key_to_scancode[53] = 53;
    this->key_to_scancode[54] = 54;
    this->key_to_scancode[55] = 55;
    this->key_to_scancode[56] = 56;
    this->key_to_scancode[57] = 57;
    this->key_to_scancode[58] = 58;
    this->key_to_scancode[59] = 59;
    this->key_to_scancode[60] = 60;
    this->key_to_scancode[61] = 61;
    this->key_to_scancode[62] = 62;
    this->key_to_scancode[63] = 63;
    this->key_to_scancode[64] = 64;
    this->key_to_scancode[65] = 65;
    this->key_to_scancode[66] = 66;
    this->key_to_scancode[67] = 67;
    this->key_to_scancode[68] = 68;
    this->key_to_scancode[69] = 69;
    this->key_to_scancode[70] = 70;
    this->key_to_scancode[71] = 71;
    this->key_to_scancode[72] = 72;
    this->key_to_scancode[73] = 73;
    this->key_to_scancode[74] = 74;
    this->key_to_scancode[75] = 75;
    this->key_to_scancode[76] = 76;
    this->key_to_scancode[77] = 77;
    this->key_to_scancode[78] = 78;
    this->key_to_scancode[79] = 79;
    this->key_to_scancode[80] = 80;
    this->key_to_scancode[81] = 81;
    this->key_to_scancode[82] = 82;
    this->key_to_scancode[83] = 83;
    this->key_to_scancode[84] = 87;
    this->key_to_scancode[85] = 88;
    this->key_to_scancode[86] = 156;
    this->key_to_scancode[87] = 157;
    this->key_to_scancode[88] = 181;
    this->key_to_scancode[89] = 183;
    this->key_to_scancode[90] = 184;
    this->key_to_scancode[91] = 199;
    this->key_to_scancode[92] = 200;
    this->key_to_scancode[93] = 201;
    this->key_to_scancode[94] = 203;
    this->key_to_scancode[95] = 205;
    this->key_to_scancode[96] = 207;
    this->key_to_scancode[97] = 208;
    this->key_to_scancode[98] = 209;
    this->key_to_scancode[99] = 210;
    this->key_to_scancode[100] = 211;
    this->key_to_scancode[101] = 219;
    this->key_to_scancode[102] = 220;
    this->key_to_scancode[103] = 221;

    for (i = 0; i < 104; ++i)
        this->scancode_to_key[this->key_to_scancode[i]] = i;

    return 0;
}

// FUNCTION: REDLINE 0x0043C98E
void UnbindAllKeybinds() {
    g_Keybinds->UnbindAll();
}

// FUNCTION: REDLINE 0x004995BC
void Keybinds::UnbindAll() {
    for (int i = 0; i < 281; ++i)
        this->Unbind(i);
}

// FUNCTION: REDLINE 0x00498529
int Keybinds::Unbind(int key) {
    int act = this->key_map[key];
    if (act == -1) return 0;

    for (int i = 0; i < 4; ++i) {
        if (this->action_map[act][i] == key) {
            this->action_map[act][i] = 0;
            break;
        }
    }
    this->key_map[key] = -1;
    return 0;
}

// FUNCTION: REDLINE 0x0043C99E
void UnbindAction(int action) {
    g_Keybinds->UnbindAction(action);
}

// FUNCTION: REDLINE 0x004985AE
void Keybinds::UnbindAction(int action) {
    if (this->InvalidActionIndex(action)) return;
    for (int i = 0; i < 4; ++i) {
        if (this->action_map[action][i] != 0) {
            this->key_map[this->action_map[action][i]] = -1;
            this->action_map[action][i] = 0;
        }
    }
}

// FUNCTION: REDLINE 0x0049828A
int Keybinds::Bind(int key, int action) {
    if (this->InvalidActionIndex(action)) return 1;
    for (int i = 0; i < 4; ++i) {
        if (this->action_map[action][i] == key)
            return 0;
    }

    int j = 0;
    while (j < 4 && this->action_map[action][j] != 0) {
        j++;
    }
    int k;
    if (j == 4) {
        for (k = 0; k < 3; ++k)
            this->action_map[action][k + 1] = this->action_map[action][k];
        this->key_map[this->action_map[action][0]] = -1;
        this->action_map[action][0] = 0;
        j = 0;
    }
    if (this->key_map[key] != -1) {
        for (k = 0; k < 4; ++k) {
            if (this->action_map[this->key_map[key]][k] == key) {
                this->action_map[this->key_map[key]][k] = 0;
                break;
            }
        }
        this->key_map[key] = -1;
    }
    this->key_map[key] = action;
    this->action_map[action][j] = key;
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

// FUNCTION: REDLINE 0x00495200
InputDevice::InputDevice() {
    // TODO
}

// SYNTHETIC: REDLINE 0x00496A30
// InputDevice::ScalarDeletingDestructor

// FUNCTION: REDLINE 0x0049522E
InputDevice::~InputDevice() {
    if (this->acquired)
        this->Unacquire();
    if (this->dev) {
        this->dev->Release();
        this->dev = NULL;
    }
}

// FUNCTION: REDLINE 0x00495354
int InputDevice::Init(const char* name, HWND window) {
    if (name) {
        if (strlen(name) >= 128) {
            strncpy(this->name, name, 126);
            this->name[127] = 0;
        } else {
            strcpy(this->name, name);
        }
    }
    return 0;
}

// FUNCTION: REDLINE 0x0049573F
int MouseDevice::Init(const char* name, HWND window) {
    InputDevice::Init(name, window);
    int res;
    if (g_DebugMouse) {
        res = this->dev->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    } else {
        res = this->dev->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    }
    if (res)
        return 1;

    res = this->dev->SetDataFormat(&c_dfDIMouse); // TODO: Verify format

    if (res)
        return 1;
    return 0;
}

// FUNCTION: REDLINE 0x00495B87
int JoystickDevice::Init(const char* name, HWND window) {
    InputDevice::Init(name, window);

    int res = this->dev->SetCooperativeLevel(window, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
    if (res)
        return 1;
    res = this->dev->SetDataFormat(&c_dfDIJoystick); // TODO: Very uncertain of format
    if (res)
        return 1;

    DIPROPRANGE range;
    range.diph.dwSize = sizeof(range);
    range.diph.dwHeaderSize = sizeof(range.diph);
    range.diph.dwObj = 0;
    range.diph.dwHow = 1;
    range.lMin = -127;
    range.lMax = 127;
    res = this->dev->SetProperty(DIPROP_RANGE, &range.diph);
    if (res)
        return 1;

    range.diph.dwSize = sizeof(range);
    range.diph.dwHeaderSize = sizeof(range.diph);
    range.diph.dwObj = 4;
    range.diph.dwHow = 1;
    range.lMin = -127;
    range.lMax = 127;
    res = this->dev->SetProperty(DIPROP_RANGE, &range.diph);
    if (res)
        return 1;

    range.diph.dwSize = sizeof(range);
    range.diph.dwHeaderSize = sizeof(range.diph);
    range.diph.dwObj = 8;
    range.diph.dwHow = 1;
    range.lMin = -127;
    range.lMax = 127;
    res = this->dev->SetProperty(DIPROP_RANGE, &range.diph);
    if (res) {
        this->joy_configured = false;
    } else {
        this->joy_configured = true;
    }

    DIPROPDWORD deadzone;
    deadzone.diph.dwSize = sizeof(deadzone);
    deadzone.diph.dwHeaderSize = sizeof(deadzone.diph);
    deadzone.diph.dwHow = 1;
    deadzone.dwData = 1000;
    deadzone.diph.dwObj = 0;
    res = this->dev->SetProperty(DIPROP_DEADZONE, &deadzone.diph);
    if (res)
        return 1;
    deadzone.diph.dwObj = 4;
    res = this->dev->SetProperty(DIPROP_DEADZONE, &deadzone.diph);
    if (res)
        return 1;
    deadzone.diph.dwObj = 8;
    res = this->dev->SetProperty(DIPROP_DEADZONE, &deadzone.diph);
    if (res) {
        this->joy_configured = false;
    }
    this->multi_pov = 0;
    DIDEVCAPS caps;
    res = this->dev->GetCapabilities(&caps);
    if (!res) {
        if (caps.dwPOVs > 0) {
            this->multi_pov = true;
        }
    }

    return 0;
}

// FUNCTION: REDLINE 0x00495E26
int GamepadDevice::Init(const char* name, HWND window) {
    int res = JoystickDevice::Init(name, window);
    if (res)
        return res;
    this->unk0 = 0;
    this->unk1 = 0;
    this->unk2 = 6;
    this->unk3 = 24;
    this->unk4 = 0;
    return 0;
}

// FUNCTION: REDLINE 0x00495488
int KeyboardDevice::Init(const char* name, HWND window) {
    InputDevice::Init(name, window);
    int res = this->dev->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (res)
        return 1;
    res = this->dev->SetDataFormat(&c_dfDIKeyboard);
    if (res)
        return 1;
    return 0;
}

// FUNCTION: REDLINE 0x004999C7
BOOL CALLBACK EnumDinputDevicesCb(LPCDIDEVICEINSTANCE devinst, void* userdata) {
    Keybinds* keybinds = (Keybinds*)userdata;
    InputDevice* input = NULL;
    int unk = 0;

    int dev_type = LOBYTE(devinst->dwDevType);
    switch (dev_type) {
        case DIDEVTYPE_MOUSE:
            input = new MouseDevice();
            break;

        case DIDEVTYPE_JOYSTICK:
            LPDIRECTINPUTA dinput;
            int res = DirectInputCreateA(g_hInstance, DIRECTINPUT_VERSION, &dinput, 0);
            if (res) {
                input = new JoystickDevice();
                break;
            }
            LPDIRECTINPUTDEVICEA dev = NULL;
            res = dinput->CreateDevice(devinst->guidInstance, &dev, NULL);
            if (res) {
                input = new JoystickDevice();
                break;
            }
            DIDEVCAPS caps;
            res = dev->GetCapabilities(&caps);
            if (res)
                caps.dwFlags = 0;
            if (HIBYTE(LOWORD(devinst->dwDevType)) == DIDEVTYPEJOYSTICK_GAMEPAD) {
                input = new GamepadDevice();
            } else {
                input = new JoystickDevice();
            }
            dev->Release();
            dinput->Release();
            break;
    }

    if (input) {
        keybinds->AddInputDevice(input, devinst->guidInstance, devinst->tszProductName, unk);
    }

    return 1;
}

// FUNCTION: REDLINE 0x00496F74
void Keybinds::AddInputDevice(InputDevice* dev, GUID guid, const char* dev_name, int unk) {
    if (this->input_device_count >= this->input_devices_max) {
        if (dev)
            delete dev;
        return;
    }
    LPDIRECTINPUTDEVICEA idev;
    int res = this->dinput->CreateDevice(guid, &idev, NULL);
    if (res) {
        if (dev)
            delete dev;
        return;
    }
    res = idev->QueryInterface(IID_IDirectInputDevice2A, (LPVOID*) &dev->dev);
    idev->Release();

    if (res) {
        if (dev)
            delete dev;
        return;
    }

    if (dev->Init(dev_name, g_Window)) {
        if (dev)
            delete dev;
        return;
    }
    
    this->input_devices[this->input_device_count++] = dev;
    if (unk) {
        this->unk = unk;
    }
}

// FUNCTION: REDLINE 0x00496E82
int Keybinds::DeinitDirectInput() {
    if (this->input_devices) {
        for (int i = 0; i < this->input_device_count; ++i) {
            if (this->input_devices[i]) {
                delete this->input_devices[i];
                this->input_devices[i] = NULL;
            }
        }
        delete this->input_devices;
        this->input_devices = NULL;
    }
    if (this->dinput) {
        this->dinput->Release();
        this->dinput = NULL;
    }
    return 0;
}

// FUNCTION: REDLINE 0x00496B74
int Keybinds::InitDirectInput() {
    if (DirectInputCreateA(g_hInstance, DIRECTINPUT_VERSION, &this->dinput, NULL))
        return 1;
    this->input_devices = new InputDevice*[this->input_devices_max];
    if(this->dinput->EnumDevices(0, EnumDinputDevicesCb, this, DIEDFL_ATTACHEDONLY)) {
        this->DeinitDirectInput();
        return 1;
    }

    InputDevice* keyboard = new KeyboardDevice();
    this->AddInputDevice(keyboard, GUID_SysKeyboard, "Sys Keyboard", 0);

    this->mouse_sens_foot = g_Mouse_Foot;
    this->joy_sens_horiz = g_Joystick_LeftRight;
    this->joy_sens_vert = g_Joystick_UpDown;

    if (g_Joystick_DeadZoneX < 0)
        g_Joystick_DeadZoneX = -g_Joystick_DeadZoneX;
    if (g_Joystick_DeadZoneX > 127)
        g_Joystick_DeadZoneX = 127;
    if (g_Joystick_DeadZoneY < 0)
        g_Joystick_DeadZoneY = -g_Joystick_DeadZoneY;
    if (g_Joystick_DeadZoneY > 127)
        g_Joystick_DeadZoneY = 127;

    // TODO: Bunch of memsetting into unk segments.
    // Good for reference on member size
    return 0;
}

// FUNCTION: REDLINE 0x0043C70C
bool InitDirectInput() {
    if (!g_Keybinds)
        return 0;
    if (g_Keybinds->InitDirectInput())
        return 0;
    return 1;
}

// FUNCTION: REDLINE 0x00495319
bool InputDevice::Unacquire() {
    int res = this->dev->Unacquire();
    this->acquired = false;
    return this->acquired;
}

// FUNCTION: REDLINE 0x00497102
void Keybinds::UnacquireInput() {
    for (int i = 0; i < this->input_device_count; ++i) {
        this->input_devices[i]->Unacquire();
    }
}

// FUNCTION: REDLINE 0x0043C7B2
void UnacquireInput() {
    g_Keybinds->UnacquireInput();
}
