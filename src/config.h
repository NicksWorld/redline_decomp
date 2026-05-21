#pragma once

union ValueInner {
    int integer;
    float decimal;
    char* string;
};

enum ValueTag {
    VALUE_SIGNED = 0,
    VALUE_FLOAT = 1,
    VALUE_STRING = 2,
    VALUE_UNSIGNED = 3,
    VALUE_BOOL = 4,
};

// Linked list node!
class Value {
    public:
    char line[32];
    ValueInner value;
    int kind; // union tag
    Value* next;
    public:
    Value();
    ~Value();
};

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
    int ParseLine(char* line);
    int ParseMapping(char* mapping);
    int ParseOther(char* line);
    int StoreValue(Value* v);
};
