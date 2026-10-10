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
        char pad2_1[72];
        GraphicWidget* startup_banner;
        char pad3[32];

        MainMenu();

        bool AddWidgets();


        virtual ~MainMenu();
        virtual bool UnknownInit();
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
