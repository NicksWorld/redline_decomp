#pragma once

#include "../interface.h"

namespace StateImpl {
    namespace StartMenu {
        bool Init(int state);
        bool Shutdown(int state);
        bool EventTick();
        bool Tick();
    };
};

// VTABLE: REDLINE 0x00586810
class IScreen {
    public:
        bool a;
        bool b;
        int unk;

        // TODO vmethods
        IScreen();

        virtual ~IScreen();
        virtual bool UnknownInit();
        virtual void Deinit();
        virtual bool Activate();
        virtual bool Deactivate();

        virtual bool VMethod5();
        virtual void VMethod6();
        virtual short VMethod7();
        virtual void VMethod8(int unk);
};

// VTABLE: REDLINE 0x00586858
class ConfirmationDialog : public IScreen {
    public:
        Widget** unk_arr;
        int unk_count;
        char pad[4];
        GraphicWidget* img_prompt;
        ButtonWidget* btn_yes;
        ButtonWidget* btn_no;
    
        ConfirmationDialog();

        virtual ~ConfirmationDialog();
        virtual bool UnknownInit();
        virtual void Deinit();
        virtual bool Activate();
        virtual bool Deactivate();

        virtual short VMethod7();

        bool AddWidgets();
        void SetPromptImage(const char* img, short width, short height, short x, short y);
};

// VTABLE: REDLINE 0x005868C4
class MainMenu : public IScreen {
    public:
        int unk0;
        int unk1;
        short unk2;
        IScreen* screen1;
        IScreen* screen2;
        IScreen* screen3;
        IScreen* screen4;
        IScreen* screen5;
        IScreen* screen6;
        ConfirmationDialog* screen7;
        ConfirmationDialog* screen8;
        ConfirmationDialog* screen9;
        IScreen* screen10;
        char pad2[12];
        ButtonWidget* btn_credits;
        ButtonWidget* btn_newgame[2];
        // ButtonWidget* btn_restart;
        ButtonWidget* btn_load[2];
        // ButtonWidget* btn_load_pause;
        ButtonWidget* btn_save[2];
        // ButtonWidget* btn_save;
        ButtonWidget* btn_multiplayer[2];
        // ButtonWidget* btn_mainmenu;

        void* unk4;
        void* unk5;
        ButtonWidget* btn_settings[2];
        // ButtonWidget* btn_settings_pause;
        ButtonWidget* btn_quit[2];
        // ButtonWidget* btn_exit;
        ButtonWidget* btn_resume[2];
        // ButtonWidget* btn_training;
        LabelWidget* lbl_version[2];
        // void* lbl_version2;

        GraphicWidget* startup_banner;

        short pause_menu_btn_x;
        short btn_multiplayer_y[2];
        short btn_settings_y[2];
        short btn_quit_y[2];
        short btn_resume_y[2];
        short btn_resume_x[2];
        short lbl_version_pos[2][2];

        MainMenu();

        bool AddWidgets();


        virtual ~MainMenu();
        virtual bool UnknownInit();
        virtual void Deinit();
        virtual bool Activate();
        virtual bool Deactivate();
        virtual short VMethod7();
};

class CStartMenu {
    public:
    bool unk0;
    MainMenu* menu;
    bool unk2;
    int unk3;
    int unk4;
    int unk5;
    bool unk6;

    CStartMenu();
    void Init();
    void InitMenu();
};
