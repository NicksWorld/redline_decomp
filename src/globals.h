#pragma once

#include <time.h>
#include <windows.h>

extern class Log g_Log;
extern class ScriptManager g_Scripts;
extern class Config *g_Config;
extern class EngineState *g_EngineState;
extern class StateTree *g_StateTree;
extern class Keybinds *g_Keybinds;

// GameData script
extern void *g_GameData;

extern HINSTANCE g_hInstance;
extern int g_nCmdShow;
extern char g_registryKey[128];

extern time_t g_time;

// Load source (loose files, packs, etc.)
extern int g_unk;
extern class AssetManager g_Assets;

extern int g_protection;

#define GLOBAL_STRING_SIZE 128
// Configurable Values
extern int g_D3DSound;
extern float g_Mouse_Foot;
extern float g_Mouse_Car;
extern int g_FreeLook;
extern int g_CarFreeLook;
extern int g_ReverseYAxis;
extern int g_ReverseYAxisCar;
extern int g_ScreenWidth;
extern int g_ScreenHeight;
extern int g_ScreenBPP;
extern float g_ScreenGamma;
extern int g_MipMapping;
extern int g_DrawShadows;
extern int g_DisplayParticles;
extern int g_DisplayTireTreads;
extern int g_DisplayScreenFlash;
extern int g_DifficultyLevel;
extern int g_EnableFog;
extern int g_EnviroMapping;
extern int g_GroundLighting;
extern int g_PalettedTextures;
extern int g_TextureDetail;
extern int g_TripleBuffer;
extern int g_LimitParticleSize;
extern int g_DXtextureManager;
extern int g_DitherEnable;
extern int g_WeaponDisplayMin;
extern int g_PersonGunDraw;
extern int g_carDashboardDraw;
extern int g_turretDashboardDraw;
extern int g_carCamMode;
extern int g_turretCamMode;
extern int g_turretFollowCamMode;
extern int g_carFollowCamMode;
extern int g_mouseWheel;
extern int g_carMouseWheel;
extern int g_carSteerInc;
extern int g_carSteerMax;
extern int g_footSteerInc;
extern int g_footSteerMax;
extern int g_soundChannels;
extern int g_cruiseControl;
extern char g_LastMap[GLOBAL_STRING_SIZE];
extern char g_DisplayDevice[GLOBAL_STRING_SIZE];
extern char g_DeviceDriver[GLOBAL_STRING_SIZE];
extern char g_Net_GameName[GLOBAL_STRING_SIZE];
extern bool g_Net_Mode_ScoreLimitOn;
extern int g_Net_Mode_ScoreLimit;
extern bool g_Net_Mode_TimeLimitOn;
extern int g_Net_Mode_TimeLimit;
extern bool g_Net_SmartCrosshair;
extern bool g_Net_Teams;
extern bool g_Net_FriendlyFire;
extern bool g_Net_TeamPlace;
extern bool g_Net_Mode_CTF;
extern bool g_Net_Mode_CTF_FlagDrop;
extern bool g_Net_Mode_CTF_Adv;
extern char g_Net_LastIP[GLOBAL_STRING_SIZE];
extern int g_Net_PlayerTeam;
extern int g_Net_PlayerSkel;
extern int g_Net_MaxPlayers;
extern bool g_Net_ConsoleTCP;
extern float g_Joystick_UpDown;
extern float g_Joystick_LeftRight;
extern int g_Joystick_FreeLook;
extern float g_Master_Volume;
extern float g_CarJoystick_UpDown;
extern float g_CarJoystick_LeftRight;
extern int g_Joystick_DeadZoneX;
extern int g_Joystick_DeadZoneY;
extern bool g_CDAudio_Active;
extern bool g_IntroVideo;

// Debug
extern int g_DebugMouse;
extern int g_DebugFrames;
extern int g_NumFrames;
extern int g_PlayDemo;
extern int g_RecordDemo;
extern int g_Windowed;
extern int g_AIActive;
extern int g_QuickRun;
extern int g_CreateUseFile;
extern int g_replayRecord;
extern int g_replayPlay;
extern int g_Net_Perf_CliSendFrames;
extern int g_Net_Perf_ServSendFrames;
extern bool g_Net_Perf_CliInterp;
extern bool g_Net_Perf_CliPredict;
extern bool g_Net_Perf_ExtraLatencyOn;
extern int g_Net_Perf_ExtraLatency;
extern bool g_Net_FastStart;
extern bool g_Net_FastHost;
