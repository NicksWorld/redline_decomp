#pragma once

#include <windows.h>
#include <time.h>

extern class Log g_Log;
extern class ScriptManager g_Scripts;

// GameData script
extern void* g_GameData;

extern HINSTANCE g_hInstance;
extern int g_nCmdShow;
extern char g_registryKey[128];

extern time_t g_time;

// Load source (loose files, packs, etc.)
extern int g_unk;
extern class AssetManager g_Assets;

extern int g_protection;
