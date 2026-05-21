#pragma once

enum Key {
    KEY_ESC = 1,
    KEY_1 = 2,
    KEY_2 = 3,
    KEY_3 = 4,
    KEY_4 = 5,
    KEY_5 = 6,
    KEY_6 = 7,
    KEY_7 = 8,
    KEY_8 = 9,
    KEY_9 = 10,
    KEY_0 = 11,
    KEY_MINUS = 12,
    KEY_EQUALS = 13,
    KEY_BACKSPACE = 14,
    KEY_TAB = 15,
    KEY_Q = 16,
    KEY_W = 17,
    KEY_E = 18,
    KEY_R = 19,
    KEY_T = 20,
    KEY_Y = 21,
    KEY_U = 22,
    KEY_I = 23,
    KEY_O = 24,
    KEY_P = 25,
    KEY_LBRACKET = 26,
    KEY_RBRACKET = 27,
    KEY_RETURN = 28,
    KEY_LCTRL = 29,
    KEY_A = 30,
    KEY_S = 31,
    KEY_D = 32,
    KEY_F = 33,
    KEY_G = 34,
    KEY_H = 35,
    KEY_J = 36,
    KEY_K = 37,
    KEY_L = 38,
    KEY_SEMICOLON = 39,
    KEY_APOSTROPHE = 40,
    KEY_BACKTICK = 41,
    KEY_LSHIFT = 42,
    KEY_BACKSLASH = 43,
    KEY_Z = 44,
    KEY_X = 45,
    KEY_C = 46,
    KEY_V = 47,
    KEY_B = 48,
    KEY_N = 49,
    KEY_M = 50,
    KEY_COMMA = 51,
    KEY_PERIOD = 52,
    KEY_SLASH = 53,
    KEY_RSHIFT = 54,
    KEY_MULTIPLY = 55,
    KEY_LALT = 56,
    KEY_SPACE = 57,
    KEY_CAPSLOCK = 58,
    KEY_F1 = 59,
    KEY_F2 = 60,
    KEY_F3 = 61,
    KEY_F4 = 62,
    KEY_F5 = 63,
    KEY_F6 = 64,
    KEY_F7 = 65,
    KEY_F8 = 66,
    KEY_F9 = 67,
    KEY_F10 = 68,
    KEY_NUMLOCK = 69,
    KEY_SCROLLLOCK = 70,
    KEY_PAD7 = 71,
    KEY_PAD8 = 72,
    KEY_PAD9 = 73,
    KEY_SUBTRACT = 74,
    KEY_PAD4 = 75,
    KEY_PAD5 = 76,
    KEY_PAD6 = 77,
    KEY_ADD = 78,
    KEY_PAD1 = 79,
    KEY_PAD2 = 80,
    KEY_PAD3 = 81,
    KEY_PAD0 = 82,
    KEY_DECIMAL = 83,
    KEY_F11 = 84,
    KEY_F12 = 85,
    KEY_ENTER = 86,
    KEY_RCTRL = 87,
    KEY_DIVIDE = 88,
    KEY_SYSRQ = 89,
    KEY_RALT = 90,
    KEY_HOME = 91,
    KEY_UP = 92,
    KEY_PAGEUP = 93,
    KEY_LEFT = 94,
    KEY_RIGHT = 95,
    KEY_END = 96,
    KEY_DOWN = 97,
    KEY_PAGEDOWN = 98,
    KEY_INSERT = 99,
    KEY_DELETE = 100,
    KEY_LWIN = 101,
    KEY_RWIN = 102,
    KEY_APPS = 103,
};

enum MouseButton {
    MOUSE_LEFT = 0,
    MOUSE_RIGHT = 1,
    MOUSE_MIDDLE = 2,
};

enum JoystickButton {
    JOY_BUTTON_1 = 0,
    JOY_BUTTON_2 = 1,
    JOY_BUTTON_3 = 2,
    JOY_BUTTON_4 = 3,
    JOY_BUTTON_5 = 4,
    JOY_BUTTON_6 = 5,
    JOY_BUTTON_7 = 6,
    JOY_BUTTON_8 = 7,
    JOY_BUTTON_9 = 8,
    JOY_BUTTON_10 = 9,
    JOY_BUTTON_11 = 10,
    JOY_BUTTON_12 = 11,
    JOY_BUTTON_13 = 12,
};

enum JoystickHat {
    JOY_HAT_DOWN = 0,
    JOY_HAT_NORTH = 1,
    JOY_HAT_NORTHEAST = 2,
    JOY_HAT_EAST = 3,
    JOY_HAT_SOUTHEAST = 4,
    JOY_HAT_SOUTH = 5,
    JOY_HAT_SOUTHWEST = 6,
    JOY_HAT_WEST = 7,
    JOY_HAT_NORTHWEST = 8,
};

enum Action {
    ACTION_FIRE = 0,
    ACTION_CHANGE_WEAPON = 1,
    ACTION_AIM = 2,
    ACTION_AIM_UP = 3,
    ACTION_AIM_DOWN = 4,
    ACTION_DIVE_LEFT = 5,
    ACTION_DIVE_RIGHT = 6,
    ACTION_DUCK = 7,
    ACTION_JUMP = 8,
    ACTION_PARKING_BRAKE = 9,
    ACTION_STRAFE = 10,
    ACTION_STRAFE_LEFT = 11,
    ACTION_STRAFE_RIGHT = 12,
    ACTION_AIM_CLEAR = 13,
    ACTION_SUICIDE = 14,
    ACTION_RUN = 15,
    ACTION_FORWARD = 16,
    ACTION_BACKWARD = 17,
    ACTION_RIGHT = 18,
    ACTION_LEFT = 19,
    ACTION_LOOK = 20,
    ACTION_WEAPON_1 = 21,
    ACTION_WEAPON_2 = 22,
    ACTION_WEAPON_3 = 23,
    ACTION_WEAPON_4 = 24,
    ACTION_WEAPON_5 = 25,
    ACTION_WEAPON_6 = 26,
    ACTION_WEAPON_7 = 27,
    ACTION_WEAPON_8 = 28,
    ACTION_WEAPON_9 = 29,
    ACTION_WEAPON_10 = 30,
    ACTION_ZOOM_IN = 31,
    ACTION_ZOOM_OUT = 32,
    ACTION_EXIT = 33,
    ACTION_FIRE_LEFT = 34,
    ACTION_FIRE_RIGHT = 35,
    ACTION_FIRE_REAR = 36,
    ACTION_CAMERA_IN = 37,
    ACTION_CAMERA_BEHIND = 38,
    ACTION_CAMERA_FRONT = 39,
    ACTION_CAMERA_SAT = 40,
    ACTION_CHANGE_CAR = 41,
    ACTION_LOOK_LEFT = 42,
    ACTION_LOOK_RIGHT = 43,
    ACTION_LOOK_REAR = 44,
    ACTION_NEXT_TARGET = 45,
    ACTION_PREV_TARGET = 46,
    ACTION_RESERVED_47 = 47,
    ACTION_RESERVED_48 = 48,
    ACTION_RESERVED_49 = 49,
    ACTION_RESERVED_50 = 50,
    ACTION_RESERVED_51 = 51,
    ACTION_RESERVED_52 = 52,
    ACTION_RESERVED_53 = 53,
    ACTION_RESERVED_54 = 54,
    ACTION_RESERVED_55 = 55,
    ACTION_RESERVED_56 = 56,
    ACTION_RESERVED_57 = 57,
    ACTION_RESERVED_58 = 58,
    ACTION_RESERVED_59 = 59,
    ACTION_RESERVED_60 = 60,
    ACTION_RESERVED_61 = 61,
    ACTION_RESERVED_62 = 62,
    ACTION_RESERVED_63 = 63,
};

bool InitKeybinds();
int LookupKeyname(char *name);
int LookupMbuttonName(char *name);
int LookupJoybuttonName(char *name);
int LookupJoyhatName(char *name);
int LookupActionName(char *name);

char *GetKeyName(int idx);
char *GetActionName(int idx);
char *GetMbuttonName(int idx);
char *GetJoybuttonName(int idx);
char *GetJoyhatName(int idx);

// Structure size isn't correct *yet*
class Keybinds {
    int unk[281];
    int unka[256];

    char pad[788];

    int unk1[256];
    int unk2[256];

    char *key_names[255];
    char *mbutton_names[3];
    char *joybutton_names[13];
    char *joyhat_names[9];
    char *action_names[64];

  public:
    Keybinds();
    int Init();
    int LookupKeyName(char *str);
    int LookupMbuttonName(char *str);
    int LookupJoybuttonName(char *str);
    int LookupJoyhatName(char *str);
    int LookupActionName(char *str);

    char *KeyName(int idx);
    char *ActionName(int idx);
    char *MbuttonName(int idx);
    char *JoybuttonName(int idx);
    char *JoyhatName(int idx);

    int InvalidKeyIndex(int idx);
    int InvalidActionIndex(int idx);
    int InvalidMbuttonIndex(int idx);
    int InvalidJoybuttonIndex(int idx);
    int InvalidJoyhatIndex(int idx);
};
