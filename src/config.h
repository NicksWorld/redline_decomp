#pragma once

class Config {
    int* arr1;
    int size1;
    int *arr2;
    int size2;
    int *arr3;
    int size3;
    int *arr4;
    int size4;
    int *arr5;
    int size5;

    // Further ints seem unused for sizing
    int* arr6;
    int unused_size6;
    int* arr7;
    int unused_size7;
    int* arr8;
    int unused_size8;
    int* arr9;
    int unused_size9;
    public:
    Config();
    int Load();
    int ParseLine(char* line);
    int ParseMapping(char* mapping);
    int ParseOther(char* line);
};
