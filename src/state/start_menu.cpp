#include "start_menu.h"

#include "../keybinds.h"
#include "../interface.h"
#include "../globals.h"
#include "../enginestate.h"
#include "../scripts.h"
#include "../render.h"
#include <winspool.h>

// GLOBAL: REDLINE 0x005A55E4
CStartMenu* g_StartMenu = NULL;

// GLOBAL: REDLINE 0x005A55E0
bool g_MainMenuInited = false;

// FUNCTION: REDLINE 0x00441460
CStartMenu::CStartMenu() {
    this->unk2 = 0;
    this->unk3 = 0;
    this->unk4 = -1;
    this->unk5 = 0;
    this->menu = 0;
    this->unk0 = 0;
}

// FUNCTION: REDLINE 0x004414E6
void CStartMenu::Init() {
    if (!g_MainMenuInited) {
        if (!g_Interface) {
            g_Interface = new CInterface();
            g_Interface->InitGraphics();
        }
    }
    this->menu = new MainMenu();
    this->menu->UnknownInit();
}

// FUNCTION: REDLINE 0x004AAFA0
IScreen::IScreen() {
    this->b = false;
    this->unk = 0;
}

// SYNTHETIC: REDLINE 0x004ABC70
// IScreen::`scalar deleting destructor'
// FUNCTION: REDLINE 0x004AAFC8
IScreen::~IScreen() {}

// FUNCTION: REDLINE 0x004AAFE7
bool IScreen::UnknownInit() {return false;}

// FUNCTION: REDLINE 0x004aafdc
void IScreen::Deinit() {}
// FUNCTION: REDLINE 0x004aaff4
bool IScreen::Activate() {return false;}
// FUNCTION: REDLINE 0x004ab001
bool IScreen::Deactivate() {return false;}

// FUNCTION: REDLINE 0x004AB00E
bool IScreen::VMethod5() {
    if (this->b) {
        // (this->unk + 20) (this->unk);
        this->b = 0;
    }
    this->Deactivate();
    return true;
}

// FUNCTION: REDLINE 0x004AB059
void IScreen::VMethod6() {
    this->a = 1;
}

// FUNCTION: REDLINE 0x004AB04A
short IScreen::VMethod7() {
    return -1;
}

// FUNCTION: REDLINE 0x004AB06B
void IScreen::VMethod8(int unk) {
    this->b = 1;
    this->unk = unk;
    this->Deactivate();
    // return (this->unk + 12)(unk)
}

// FUNCTION: REDLINE 0x004B0860
MainMenu::MainMenu() {
    this->unk2 = 0;
    this->screen2 = 0;
    this->screen3 = 0;
    this->screen4 = 0;
    this->screen5 = 0;
    this->screen6 = 0;
    this->screen10 = 0;
    this->screen1 = 0;
    this->unk0 = 0;
    this->unk1 = 0;
}

// SYNTHETIC: REDLINE 0x004B3650
// MainMenu::`scalar deleting destructor'
// FUNCTION: REDLINE 0x004B08E2
MainMenu::~MainMenu() {
    // TODO
}

// FUNCTION: REDLINE 0x004B0C41
bool MainMenu::UnknownInit() {
    // TODO: Init a ton of subscreens
    this->screen7 = new ConfirmationDialog();
    this->screen8 = new ConfirmationDialog();
    this->screen9 = new ConfirmationDialog();

    if (!this->AddWidgets()) return false;
    if(!this->screen7->UnknownInit()) return false;
    if(!this->screen8->UnknownInit()) return false;
    if(!this->screen9->UnknownInit()) return false;

    this->screen9->SetPromptImage("PU1LEAVE_active", 144, 18, 249, 212);
    this->screen7->SetPromptImage("2EXITTOWIN_active", 144, 12, 249, 214);
    this->screen8->SetPromptImage("2RUSURE_active", 144, 24, 254, 202);

    return true;
}

// FUNCTION: REDLINE 0x004B1104
bool MainMenu::AddWidgets() {
    this->unk = 0;
    // TODO: Bunch of initializations of shorts
    this->startup_banner = (GraphicWidget*)CreateWidget(WIDGET_GRAPHIC, g_Interface);
    this->startup_banner->SetUnkFloats(257, 102, 0);
    this->startup_banner->active = 0;
    this->startup_banner->SetImage("2Start_active", 0, 0, 144, 14, 0, 0, 0);
    g_Interface->AddControl(this->startup_banner);

    this->btn_credits = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_credits->SetUnkFloats(180, 394, 0);
    this->btn_credits->active = 0;
    this->btn_credits->unk_120 = 1;
    this->btn_credits->SetImage("2CREDITS_active", 0, 0, 248, 40, 0, 0, 0);
    this->btn_credits->LoadImage("2CREDITS_select", 0, 0, 248, 40, 0, 0, 0);
    this->btn_credits->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0];
    g_Interface->AddControl(this->btn_credits);

    this->btn_newgame[0]= (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_newgame[0]->SetUnkFloats(253, 135, 0);
    this->btn_newgame[0]->active = 0;
    this->btn_newgame[0]->unk_120 = 1;
    this->btn_newgame[0]->SetImage("2NEW_active", 0, 0, 152, 28, 0, 0, 0);
    this->btn_newgame[0]->LoadImage("2NEW_select", 0, 0, 152, 28, 0, 0, 0);
    this->btn_newgame[0]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_newgame[0]);

    this->btn_newgame[1] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_newgame[1]->SetUnkFloats(235, 146, 0);
    this->btn_newgame[1]->active = 0;
    this->btn_newgame[1]->unk_120 = 1;
    this->btn_newgame[1]->SetImage("PU1RESTART_active", 0, 0, 168, 22, 0, 0, 0);
    this->btn_newgame[1]->LoadImage("PU1RESTART_select", 0, 0, 168, 22, 0, 0, 0);
    this->btn_newgame[1]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_newgame[1]);

    this->btn_save[0] = NULL;

    this->btn_save[1] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_save[1]->SetUnkFloats(235, 172, 0);
    this->btn_save[1]->active = 0;
    this->btn_save[1]->unk_120 = 1;
    this->btn_save[1]->SetImage("PU1SAVE_active", 0, 0, 168, 22, 0, 0, 0);
    this->btn_save[1]->LoadImage("PU1SAVE_select", 0, 0, 168, 22, 0, 0, 0);
    this->btn_save[1]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_save[1]);

    this->btn_load[0]= (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_load[0]->SetUnkFloats(253, 171, 0);
    this->btn_load[0]->active = 0;
    this->btn_load[0]->unk_120 = 1;
    this->btn_load[0]->SetImage("2LOAD_active", 0, 0, 152, 28, 0, 0, 0);
    this->btn_load[0]->LoadImage("2LOAD_select", 0, 0, 152, 28, 0, 0, 0);
    this->btn_load[0]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_load[0]);

    this->btn_load[1] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_load[1]->SetUnkFloats(235, 198, 0);
    this->btn_load[1]->active = 0;
    this->btn_load[1]->unk_120 = 1;
    this->btn_load[1]->SetImage("PU1LOAD_active", 0, 0, 168, 22, 0, 0, 0);
    this->btn_load[1]->LoadImage("PU1LOAD_select", 0, 0, 168, 22, 0, 0, 0);
    this->btn_load[1]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_load[1]);

    this->btn_multiplayer[0]= (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_multiplayer[0]->SetUnkFloats(253, 207, 0);
    this->btn_multiplayer[0]->active = 0;
    this->btn_multiplayer[0]->unk_120 = 1;
    this->btn_multiplayer[0]->SetImage("2MULTI_active", 0, 0, 152, 28, 0, 0, 0);
    this->btn_multiplayer[0]->LoadImage("2MULTI_select", 0, 0, 152, 28, 0, 0, 0);
    this->btn_multiplayer[0]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_multiplayer[0]);

    this->btn_multiplayer[1] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_multiplayer[1]->SetUnkFloats(235, 224, 0);
    this->btn_multiplayer[1]->active = 0;
    this->btn_multiplayer[1]->unk_120 = 1;
    this->btn_multiplayer[1]->SetImage("PU1MAIN_active", 0, 0, 168, 22, 0, 0, 0);
    this->btn_multiplayer[1]->LoadImage("PU1MAIN_select", 0, 0, 168, 22, 0, 0, 0);
    this->btn_multiplayer[1]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_multiplayer[1]);

    this->btn_settings[0]= (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_settings[0]->SetUnkFloats(253, 243, 0);
    this->btn_settings[0]->active = 0;
    this->btn_settings[0]->unk_120 = 1;
    this->btn_settings[0]->SetImage("2SET_active", 0, 0, 152, 28, 0, 0, 0);
    this->btn_settings[0]->LoadImage("2SET_select", 0, 0, 152, 28, 0, 0, 0);
    this->btn_settings[0]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_settings[0]);

    this->btn_settings[1] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_settings[1]->SetUnkFloats(235, 251, 0);
    this->btn_settings[1]->active = 0;
    this->btn_settings[1]->unk_120 = 1;
    this->btn_settings[1]->SetImage("PU1SET_active", 0, 0, 168, 22, 0, 0, 0);
    this->btn_settings[1]->LoadImage("PU1SET_select", 0, 0, 168, 22, 0, 0, 0);
    this->btn_settings[1]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_settings[1]);

    this->btn_resume[1] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_resume[1]->SetUnkFloats(253, 279, 0);
    this->btn_resume[1]->active = 0;
    this->btn_resume[1]->unk_120 = 1;
    this->btn_resume[1]->SetImage("2TRAIN_active", 0, 0, 152, 28, 0, 0, 0);
    this->btn_resume[1]->LoadImage("2TRAIN_select", 0, 0, 152, 28, 0, 0, 0);
    this->btn_resume[1]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_resume[1]);

    this->btn_quit[0] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_quit[0]->SetUnkFloats(253, 315, 0);
    this->btn_quit[0]->active = 0;
    this->btn_quit[0]->unk_120 = 1;
    this->btn_quit[0]->SetImage("2Quit_active", 0, 0, 152, 28, 0, 0, 0);
    this->btn_quit[0]->LoadImage("2Quit_select", 0, 0, 152, 28, 0, 0, 0);
    this->btn_quit[0]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_quit[0]);

    this->btn_quit[1] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_quit[1]->SetUnkFloats(235, 277, 0);
    this->btn_quit[1]->active = 0;
    this->btn_quit[1]->unk_120 = 1;
    this->btn_quit[1]->SetImage("PU1EXIT_active", 0, 0, 168, 22, 0, 0, 0);
    this->btn_quit[1]->LoadImage("PU1EXIT_select", 0, 0, 168, 22, 0, 0, 0);
    this->btn_quit[1]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_quit[1]);

    this->btn_resume[0] = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_resume[0]->SetUnkFloats(294, 322, 0);
    this->btn_resume[0]->active = 0;
    this->btn_resume[0]->unk_120 = 1;
    this->btn_resume[0]->SetImage("PU1RESUME_active", 0, 0, 104, 10, 0, 0, 0);
    this->btn_resume[0]->LoadImage("PU1RESUME_select", 0, 0, 104, 10, 0, 0, 0);
    this->btn_resume[0]->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0]
    g_Interface->AddControl(this->btn_resume[0]);

    // TODO: Version labels

    // TODO: call method that sets up keyboard navigation
    return true;
}

// FUNCTION: REDLINE 0x004B0933
void MainMenu::Deinit() {
    // TODO
}

// FUNCTION: REDLINE 0x004B259C
bool MainMenu::Activate() {
    bool gameplay = false;
    if (g_unkBool /* means multiplayer */) {
        // paused = unk != 0;
    }

    if (false/*unk*/) {
        // TODO: Pause menus
    } else {
        this->unk2 = 0;
        // TODO: Call that sets the interface background and stuff
    }
    // TODO

    if (gameplay) {
        this->btn_newgame[this->unk2]->active = 0;
        this->btn_load[this->unk2]->active = 0;
    } else {
        this->btn_newgame[this->unk2]->active = 1;
        this->btn_load[this->unk2]->active = 1;
    }
    if (this->unk2 == 1) {
        // TODO this->unk3[unk2] + 48 = !gameplay
        this->btn_resume[0]->active = 1;
    } else {
        this->btn_credits->active = 1;
        if (!gameplay)
            this->btn_resume[1]->active = 1;
    }
    this->btn_multiplayer[this->unk2]->active = 1;
    this->btn_settings[this->unk2]->active = 1;
    this->btn_quit[this->unk2]->active = 1;
    // TODO: lbl_version

    // TODO: Interface calls
    return true;
}

// FUNCTION: REDLINE 0x004B2B58
bool MainMenu::Deactivate() {
    return true;
    // TODO
}

// FUNCTION: REDLINE 0x004B2CC2
short MainMenu::VMethod7() {
    return -1;
    // TODO
}

// FUNCTION: REDLINE 0x004ABD10
ConfirmationDialog::ConfirmationDialog() {
    this->unk_arr = NULL;
    this->unk_count = 0;
}

// SYNTHETIC: REDLINE 0x004AC290
// ConfirmationDialog::`scalar deleting destructor'
// FUNCTION: REDLINE 0x004ABD43
ConfirmationDialog::~ConfirmationDialog() {
    // TODO
}

// FUNCTION: REDLINE 0x004ABFD3
bool ConfirmationDialog::AddWidgets() {
    this->img_prompt = (GraphicWidget*)CreateWidget(WIDGET_GRAPHIC, g_Interface);
    this->img_prompt->SetUnkFloats(249, 212, 0);
    this->img_prompt->active = 0;
    this->img_prompt->unk_120 = 1;
    g_Interface->AddControl(this->img_prompt);

    this->btn_yes = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_yes->SetUnkFloats(252, 245, 0);
    this->btn_yes->active = 0;
    this->btn_yes->unk_120 = 1;
    this->btn_yes->SetImage("PU1YES_active", 0, 0, 64, 22, 0, 0, 0);
    this->btn_yes->LoadImage("PU1YES_select", 0, 0, 64, 22, 0, 0, 0);
    this->btn_yes->SetButtonUnk(0, 1, 1);
    // TODO: Call to vtable[0];
    g_Interface->AddControl(this->btn_yes);

    this->btn_no = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_no->SetUnkFloats(324, 245, 0);
    this->btn_no->active = 0;
    this->btn_no->unk_120 = 1;
    this->btn_no->SetImage("PU1NO_active", 0, 0, 64, 22, 0, 0, 0);
    this->btn_no->LoadImage("PU1NO_select", 0, 0, 64, 22, 0, 0, 0);
    this->btn_no->SetButtonUnk(0, 1, 1);
    // TODO: Call to vtable[0];
    g_Interface->AddControl(this->btn_no);

    // TODO: 0x004ABF7A Does it set next button selected with arrow keys?
    return true;
}

// FUNCTION: REDLINE 0x004AC210
void ConfirmationDialog::SetPromptImage(const char* img, short width, short height, short x, short y) {
    this->img_prompt->SetImage(img, 0, 0, width, height, 2, 0, 0);
    this->img_prompt->SetUnkFloats(x, y, 0);
}
 
// FUNCTION: REDLINE 0x004ABD94
bool ConfirmationDialog::UnknownInit() {
    return this->AddWidgets();
}

// FUNCTION: REDLINE 0x004ABDB6
void ConfirmationDialog::Deinit() {
    for (int i = 0; i < this->unk_count; ++i) {
        if (this->unk_arr[i])
            g_Interface->RemoveControl(this->unk_arr[i]);
    }
    if (this->unk_arr) {
        delete this->unk_arr;
        this->unk_arr = NULL;
    }
    this->unk_count = 0;
}

// FUNCTION: REDLINE 0x004ABE3B
bool ConfirmationDialog::Activate() {
    this->a = 0;
    this->unk_arr[8] = 0;
    // TODO: g_Interface call
    // TODO: Set next sel?
    this->img_prompt->active = 1;
    this->btn_yes->active = 1;
    this->btn_no->active = 1;
    // TODO: g_Interface call
    return true;
}

// FUNCTION: REDLINE 0x004ABEBC
bool ConfirmationDialog::Deactivate() {
    // TODO: g_Interface call
    this->img_prompt->active = 0;
    this->btn_yes->active = 0;
    this->btn_no->active = 0;
    return true;
}

// FUNCTION: REDLINE 0x004ABF10
short ConfirmationDialog::VMethod7() {
    // TODO: Input handling?
    return 0;
}

// FUNCTION: REDLINE 0x00541020
bool StateImpl::StartMenu::Init(int prev_state) {
    UnacquireInput();
    SetMouseSensitivityFoot(2.0);

    if (!g_StartMenu) {
        g_StartMenu = new CStartMenu();
        g_StartMenu->Init();
    }

    bool unk = 0;
    if (g_ConnectDirect == 1 || g_LobbyEnabled == 1 || g_ConsoleEnabled == 1) {
        unk = 1;
    } else {
        g_unkBool = 0;
    }

    if (g_QuickRun && prev_state != STATE_GAME_PLAY && !unk) {
        // TODO
    } else if (g_ConsoleEnabled) {
        // TODO
        return true;
    } else {
        // TODO: 5CD028 = 0
        if (!g_MainMenuInited) {
            g_MainMenuInited = 1;
            g_StartMenu->InitMenu();
        }
        // TODO: NOP
        g_StartMenu->unk2 = 1;
        // TODO: Spawn audio thread
    }
    
    void* menu_music = NULL;
    menu_music = g_Scripts.Lookup(SK_Sounds, "Menu_Music", 0);

    return true;
}

// FUNCTION: REDLINE 0x004415D5
void CStartMenu::InitMenu() {
    this->menu->Activate();

    this->unk6 = 1;
    this->unk3 = 15;
    this->unk4 = 1;
    SetMouseSensitivityFoot(2.0);
}

// FUNCTION: REDLINE 0x0054139D
bool StateImpl::StartMenu::Shutdown(int next_state) {
    // TODO
    return true;
}

// FUNCTION: REDLINE 0x00541235
bool StateImpl::StartMenu::Tick() {
    // TODO: Keybinds thing
    // TODO: Audio thing
    // TODO: Start menu call
    // TODO
    return true;
}

// FUNCTION: REDLINE 0x0054132F
bool StateImpl::StartMenu::EventTick() {
    LockRender();
    ClearViewport(2);
    // TODO: 0x0043C1A9 Which resets drawn tri count and more
    // TODO: Interface method
    // TODO: 0x0043BD50
    // TODO: 0x0048EED1 TextureMgr call
    if (BeginScene()) {
        g_Interface->Render(1);
        // TODO: 0x0043BDB5 Tri drawing routine?
        // TODO: 0x0043CE39 Renderhelper drawing
    }
    if (EndScene())
        FlipDisplay();
    UnlockRender();
    return true;
}

