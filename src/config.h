#pragma once


union ValueInner {
    int integer;
    float decimal;
    char* string;
    bool boolean;
};

enum ValueTag {
    VALUE_SIGNED = 0,
    VALUE_FLOAT = 1,
    VALUE_STRING = 2,
    VALUE_INTBOOL = 3,
    VALUE_CHARBOOL = 4,
};

// Linked list node!
class Value {
    public:
    char name[32];
    ValueInner value;
    int kind; // union tag
    Value* next;
    public:
    Value();
    ~Value();
};

struct DefaultConfValue {
    char name[128];
    void *value;
    int kind;
};

extern DefaultConfValue g_ConfDefault[73];
extern DefaultConfValue g_ConfDefaultDebug[20];

class Config {
    Value** conf_values;
    int conf_size;
    int *keybinds_foot;
    int keybind_size;
    int *mousebinds_foot;
    int mousebind_size;
    int *joybinds_foot;
    int joybind_size;
    int *jhatbinds_foot;
    int jhatbind_size;

    // Further ints seem unused for sizing
    int* keybinds_car;
    int unused_size6;
    int* mousebinds_car;
    int unused_size7;
    int* joybinds_car;
    int unused_size8;
    int* jhatbinds_car;
    int unused_size9;
    public:
    Config();
    void ResetBinds();
    void ClearConfValues();

    void LoadPreset(int mode);

    void Default();
    void DefaultConf();

    int Load();

    int ApplyKeybinds(bool car);

    int ParseLine(char* line);
    int ParseMapping(char* mapping);
    int ParseOther(char* line);

    Value* GetValue(const char* name);
    int StoreValue(Value* v);

    int GetIntValue(const char* name, int* out);
    int GetFloatValue(const char* name, float* out);
    int GetStringValue(const char* name, char* out);
    int GetBoolValue(const char* name, bool* out);
};
