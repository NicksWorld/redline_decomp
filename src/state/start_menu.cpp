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
// FUNCTION: REDLINE 0x004B0933
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
    // TODO: Bunch of initializations of shorts
    this->startup_banner = (GraphicWidget*)CreateWidget(WIDGET_GRAPHIC, g_Interface);
    this->startup_banner->SetUnkFloats(257, 102, 0);
    this->startup_banner->unk_92 = 0;
    this->startup_banner->SetImage("2Start_active", 0, 0, 144, 14, 0, 0, 0);
    g_Interface->AddControl(this->startup_banner);

    this->btn_credits = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_credits->SetUnkFloats(180, 394, 0);
    this->btn_credits->unk_92 = 0;
    this->btn_credits->unk_120 = 1;
    this->btn_credits->SetImage("2CREDITS_active", 0, 0, 248, 40, 0, 0, 0);
    this->btn_credits->LoadImage("2CREDITS_select", 0, 0, 248, 40, 0, 0, 0);
    this->btn_credits->SetButtonUnk(0, 1, 1);
    // TODO: vtable[0];
    g_Interface->AddControl(this->btn_credits);

    // TODO
    return true;
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
    this->img_prompt->unk_92 = 0;
    this->img_prompt->unk_120 = 1;
    g_Interface->AddControl(this->img_prompt);

    this->btn_yes = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_yes->SetUnkFloats(252, 245, 0);
    this->btn_yes->unk_92 = 0;
    this->btn_yes->unk_120 = 1;
    this->btn_yes->SetImage("PU1YES_active", 0, 0, 64, 22, 0, 0, 0);
    this->btn_yes->LoadImage("PU1YES_select", 0, 0, 64, 22, 0, 0, 0);
    this->btn_yes->SetButtonUnk(0, 1, 1);
    // TODO: Call to vtable[0];
    g_Interface->AddControl(this->btn_yes);

    this->btn_no = (ButtonWidget*)CreateWidget(WIDGET_BUTTON, g_Interface);
    this->btn_no->SetUnkFloats(324, 245, 0);
    this->btn_no->unk_92 = 0;
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
    this->img_prompt->unk_92 = 1;
    this->btn_yes->unk_92 = 1;
    this->btn_no->unk_92 = 1;
    // TODO: g_Interface call
    return true;
}

// FUNCTION: REDLINE 0x004ABEBC
bool ConfirmationDialog::Deactivate() {
    // TODO: g_Interface call
    this->img_prompt->unk_92 = 0;
    this->btn_yes->unk_92 = 0;
    this->btn_no->unk_92 = 0;
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

