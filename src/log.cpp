#include "log.h"

#include "globals.h"

#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

// GLOBAL: REDLINE 0x00587578
int g_protection = 0x1a4;

// Stack is a bit scrambled, and inlining isn't applying on ofstream methods
// FUNCTION: REDLINE 0x004a8e90
void Log::Open(const char *filename, int truncate) {
    struct Locals {
        char msg[128];
        ofstream *ostream;
        time_t time_base;
        struct tm *time;
        int flags;
    } l;

    if (filename != NULL && strlen(filename) != 0 && strlen(filename) < 0x100) {
        strcpy(this->filename, filename);
        l.ostream = NULL;
        l.flags = ios::out | ios::binary;
        if (truncate != 0) {
            l.flags |= ios::trunc;
        } else {
            l.flags |= ios::app;
        }
        l.ostream = new ofstream(filename, l.flags, g_protection);
        if (l.ostream == NULL)
            goto cleanup;
        if (l.ostream->fail() == 2) {
            goto cleanup;
        }

        this->unk = 0;
        time(&l.time_base);
        l.time = localtime(&l.time_base);
        strcpy(l.msg, asctime(l.time));
        if (strlen(l.msg) > 2) {
            l.msg[strlen(l.msg) - 1] = '\0';
        }
        *l.ostream << "// Program started on " << l.msg << "\r\n";
cleanup:
        if (l.ostream != NULL) {
            delete l.ostream;
            l.ostream = NULL;
        }
    }
}

// FUNCTION: REDLINE 0x004A936C
void Log::Print(const char *msg) {
    ofstream *stream;
    if (this->unk)
        return;
    if (!msg || !strlen(msg)) {
        return;
    }
    stream = NULL;
    stream = new ofstream(this->filename, 138, g_protection);
    if (stream == NULL)
        goto cleanup;
    if (stream->fail() == 2)
        goto cleanup;
    *stream << msg << "\r\n";
cleanup:
    if (stream != NULL) {
        delete stream;
        stream = NULL;
    }
}

// FUNCTION: REDLINE 0x004a94a1
void Log::Debug(const char *msg) {
    // Really, this method is completely empty
    MessageBoxA(NULL, msg, NULL, MB_ICONWARNING);
}

// STUB: REDLINE 0x0043CBE0
void SomeDebug(const char* msg) {
    // In reality this is empty
    g_Log.Debug(msg);
}

// FUNCTION: REDLINE 0x004A97A6
void Log::DxErr(const char *msg, int err) {
    char err_name[256];
    char buf[256];
    this->DxErrToString(err, err_name);
    sprintf(buf, "*Error: %s (%s)", msg, err_name);
    this->Debug(buf);
}

// FUNCTION: REDLINE 0x004A9802
void Log::D3dErr(const char *msg, int err) {
    char err_name[256];
    char buf[256];
    this->D3dErrToString(err, err_name);
    sprintf(buf, "*Error: %s (%s)", msg, err_name);
    this->Debug(buf);
}

#include "ddraw.h"

// FUNCTION: REDLINE 0x004A985E
void Log::DxErrToString(int err, char *buf) {
    switch (err) {
    case MAKE_DDHRESULT(5):
        sprintf(buf, "DDERR_ALREADYINITIALIZED");
        break;
    case MAKE_DDHRESULT(10):
        sprintf(buf, "DDERR_CANNOTATTACHSURFACE");
        break;
    case MAKE_DDHRESULT(20):
        sprintf(buf, "DDERR_CANNOTDETACHSURFACE");
        break;
    case MAKE_DDHRESULT(40):
        sprintf(buf, "DDERR_CURRENTLYNOTAVAIL");
        break;
    case MAKE_DDHRESULT(55):
        sprintf(buf, "DDERR_EXCEPTION");
        break;
    case E_FAIL:
        sprintf(buf, "DDERR_GENERIC");
        break;
    case MAKE_DDHRESULT(90):
        sprintf(buf, "DDERR_HEIGHTALIGN");
        break;
    case MAKE_DDHRESULT(95):
        sprintf(buf, "DDERR_INCOMPATIBLEPRIMARY");
        break;
    case MAKE_DDHRESULT(100):
        sprintf(buf, "DDERR_INVALIDCAPS");
        break;
    case MAKE_DDHRESULT(110):
        sprintf(buf, "DDERR_INVALIDCLIPLIST");
        break;
    case MAKE_DDHRESULT(120):
        sprintf(buf, "DDERR_INVALIDMODE");
        break;
    case MAKE_DDHRESULT(130):
        sprintf(buf, "DDERR_INVALIDOBJECT");
        break;
    case E_INVALIDARG:
        sprintf(buf, "DDERR_INVALIDPARAMS");
        break;
    case MAKE_DDHRESULT(145):
        sprintf(buf, "DDERR_INVALIDPIXELFORMAT");
        break;
    case MAKE_DDHRESULT(150):
        sprintf(buf, "DDERR_INVALIDRECT");
        break;
    case MAKE_DDHRESULT(160):
        sprintf(buf, "DDERR_LOCKEDSURFACES");
        break;
    case MAKE_DDHRESULT(170):
        sprintf(buf, "DDERR_NO3D");
        break;
    case MAKE_DDHRESULT(180):
        sprintf(buf, "DDERR_NOALPHAHW");
        break;
    case MAKE_DDHRESULT(205):
        sprintf(buf, "DDERR_NOCLIPLIST");
        break;
    case MAKE_DDHRESULT(210):
        sprintf(buf, "DDERR_NOCOLORCONVHW");
        break;
    case MAKE_DDHRESULT(212):
        sprintf(buf, "DDERR_NOCOOPERATIVELEVELSET");
        break;
    case MAKE_DDHRESULT(215):
        sprintf(buf, "DDERR_NOCOLORKEY");
        break;
    case MAKE_DDHRESULT(220):
        sprintf(buf, "DDERR_NOCOLORKEYHW");
        break;
    case MAKE_DDHRESULT(222):
        sprintf(buf, "DDERR_NODIRECTDRAWSUPPORT");
        break;
    case MAKE_DDHRESULT(225):
        sprintf(buf, "DDERR_NOEXCLUSIVEMODE");
        break;
    case MAKE_DDHRESULT(230):
        sprintf(buf, "DDERR_NOFLIPHW");
        break;
    case MAKE_DDHRESULT(240):
        sprintf(buf, "DDERR_NOGDI");
        break;
    case MAKE_DDHRESULT(250):
        sprintf(buf, "DDERR_NOMIRRORHW");
        break;
    case MAKE_DDHRESULT(255):
        sprintf(buf, "DDERR_NOTFOUND");
        break;
    case MAKE_DDHRESULT(260):
        sprintf(buf, "DDERR_NOOVERLAYHW");
        break;
    case MAKE_DDHRESULT(280):
        sprintf(buf, "DDERR_NORASTEROPHW");
        break;
    case MAKE_DDHRESULT(290):
        sprintf(buf, "DDERR_NOROTATIONHW");
        break;
    case MAKE_DDHRESULT(310):
        sprintf(buf, "DDERR_NOSTRETCHHW");
        break;
    case MAKE_DDHRESULT(316):
        sprintf(buf, "DDERR_NOT4BITCOLOR");
        break;
    case MAKE_DDHRESULT(317):
        sprintf(buf, "DDERR_NOT4BITCOLORINDEX");
        break;
    case MAKE_DDHRESULT(320):
        sprintf(buf, "DDERR_NOT8BITCOLOR");
        break;
    case MAKE_DDHRESULT(330):
        sprintf(buf, "DDERR_NOTEXTUREHW");
        break;
    case MAKE_DDHRESULT(335):
        sprintf(buf, "DDERR_NOVSYNCHW");
        break;
    case MAKE_DDHRESULT(340):
        sprintf(buf, "DDERR_NOZBUFFERHW");
        break;
    case MAKE_DDHRESULT(350):
        sprintf(buf, "DDERR_NOZOVERLAYHW");
        break;
    case MAKE_DDHRESULT(360):
        sprintf(buf, "DDERR_OUTOFCAPS");
        break;
    case E_OUTOFMEMORY:
        sprintf(buf, "DDERR_OUTOFMEMORY");
        break;
    case MAKE_DDHRESULT(380):
        sprintf(buf, "DDERR_OUTOFVIDEOMEMORY");
        break;
    case MAKE_DDHRESULT(382):
        sprintf(buf, "DDERR_OVERLAYCANTCLIP");
        break;
    case MAKE_DDHRESULT(384):
        sprintf(buf, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE");
        break;
    case MAKE_DDHRESULT(387):
        sprintf(buf, "DDERR_PALETTEBUSY");
        break;
    case MAKE_DDHRESULT(400):
        sprintf(buf, "DDERR_COLORKEYNOTSET");
        break;
    case MAKE_DDHRESULT(410):
        sprintf(buf, "DDERR_SURFACEALREADYATTACHED");
        break;
    case MAKE_DDHRESULT(420):
        sprintf(buf, "DDERR_SURFACEALREADYDEPENDENT");
        break;
    case MAKE_DDHRESULT(430):
        sprintf(buf, "DDERR_SURFACEBUSY");
        break;
    case MAKE_DDHRESULT(435):
        sprintf(buf, "DDERR_CANTLOCKSURFACE");
        break;
    case MAKE_DDHRESULT(440):
        sprintf(buf, "DDERR_SURFACEISOBSCURED");
        break;
    case MAKE_DDHRESULT(450):
        sprintf(buf, "DDERR_SURFACELOST");
        break;
    case MAKE_DDHRESULT(460):
        sprintf(buf, "DDERR_SURFACENOTATTACHED");
        break;
    case MAKE_DDHRESULT(470):
        sprintf(buf, "DDERR_TOOBIGHEIGHT");
        break;
    case MAKE_DDHRESULT(480):
        sprintf(buf, "DDERR_TOOBIGSIZE");
        break;
    case MAKE_DDHRESULT(490):
        sprintf(buf, "DDERR_TOOBIGWIDTH");
        break;
    case E_NOTIMPL:
        sprintf(buf, "DDERR_UNSUPPORTED");
        break;
    case MAKE_DDHRESULT(510):
        sprintf(buf, "DDERR_UNSUPPORTEDFORMAT");
        break;
    case MAKE_DDHRESULT(520):
        sprintf(buf, "DDERR_UNSUPPORTEDMASK");
        break;
    case MAKE_DDHRESULT(537):
        sprintf(buf, "DDERR_VERTICALBLANKINPROGRESS");
        break;
    case MAKE_DDHRESULT(540):
        sprintf(buf, "DDERR_WASSTILLDRAWING");
        break;
    case MAKE_DDHRESULT(560):
        sprintf(buf, "DDERR_XALIGN");
        break;
    case MAKE_DDHRESULT(561):
        sprintf(buf, "DDERR_INVALIDDIRECTDRAWGUID");
        break;
    case MAKE_DDHRESULT(562):
        sprintf(buf, "DDERR_DIRECTDRAWALREADYCREATED");
        break;
    case MAKE_DDHRESULT(563):
        sprintf(buf, "DDERR_NODIRECTDRAWHW");
        break;
    case MAKE_DDHRESULT(564):
        sprintf(buf, "DDERR_PRIMARYSURFACEALREADYEXIST");
        break;
    case MAKE_DDHRESULT(565):
        sprintf(buf, "DDERR_NOEMULATION");
        break;
    case MAKE_DDHRESULT(566):
        sprintf(buf, "DDERR_REGIONTOOSMALL");
        break;
    case MAKE_DDHRESULT(567):
        sprintf(buf, "DDERR_CLIPPERISUSINGHWND");
        break;
    case MAKE_DDHRESULT(568):
        sprintf(buf, "DDERR_NOCLIPPERATTACHED");
        break;
    case MAKE_DDHRESULT(569):
        sprintf(buf, "DDERR_NOHWND");
        break;
    case MAKE_DDHRESULT(570):
        sprintf(buf, "DDERR_HWNDSUBCLASSED");
        break;
    case MAKE_DDHRESULT(571):
        sprintf(buf, "DDERR_HWNDALREADYSET");
        break;
    case MAKE_DDHRESULT(572):
        sprintf(buf, "DDERR_NOPALETTEATTACHED");
        break;
    case MAKE_DDHRESULT(573):
        sprintf(buf, "DDERR_NOPALETTEHW");
        break;
    case MAKE_DDHRESULT(574):
        sprintf(buf, "DDERR_BLTFASTCANTCLIP");
        break;
    case MAKE_DDHRESULT(575):
        sprintf(buf, "DDERR_NOBLTHW");
        break;
    case MAKE_DDHRESULT(576):
        sprintf(buf, "DDERR_NODDROPSHW");
        break;
    case MAKE_DDHRESULT(577):
        sprintf(buf, "DDERR_OVERLAYNOTVISIBLE");
        break;
    case MAKE_DDHRESULT(578):
        sprintf(buf, "DDERR_NOOVERLAYDEST");
        break;
    case MAKE_DDHRESULT(579):
        sprintf(buf, "DDERR_INVALIDPOSITION");
        break;
    case MAKE_DDHRESULT(580):
        sprintf(buf, "DDERR_NOTAOVERLAYSURFACE");
        break;
    case MAKE_DDHRESULT(581):
        sprintf(buf, "DDERR_EXCLUSIVEMODEALREADYSET");
        break;
    case MAKE_DDHRESULT(582):
        sprintf(buf, "DDERR_NOTFLIPPABLE");
        break;
    case MAKE_DDHRESULT(583):
        sprintf(buf, "DDERR_CANTDUPLICATE");
        break;
    case MAKE_DDHRESULT(584):
        sprintf(buf, "DDERR_NOTLOCKED");
        break;
    case MAKE_DDHRESULT(585):
        sprintf(buf, "DDERR_CANTCREATEDC");
        break;
    case MAKE_DDHRESULT(586):
        sprintf(buf, "DDERR_NODC");
        break;
    case MAKE_DDHRESULT(587):
        sprintf(buf, "DDERR_WRONGMODE");
        break;
    case MAKE_DDHRESULT(588):
        sprintf(buf, "DDERR_IMPLICITLYCREATED");
        break;
    case MAKE_DDHRESULT(589):
        sprintf(buf, "DDERR_NOTPALETTIZED");
        break;
    case MAKE_DDHRESULT(590):
        sprintf(buf, "DDERR_UNSUPPORTEDMODE");
        break;
    case MAKE_DDHRESULT(591):
        sprintf(buf, "DDERR_NOMIPMAPHW");
        break;
    case MAKE_DDHRESULT(592):
        sprintf(buf, "DDERR_INVALIDSURFACETYPE");
        break;
    // case MAKE_DDHRESULT(600):
    //     sprintf(buf, "DDERR_NOOPTIMIZEHW");
    //     break;
    // case MAKE_DDHRESULT(601):
    //     sprintf(buf, "DDERR_NOTLOADED");
    //     break;
    // case MAKE_DDHRESULT(602):
    //     sprintf(buf, "DDERR_NOFOCUSWINDOW");
    //     break;
    case MAKE_DDHRESULT(620):
        sprintf(buf, "DDERR_DCALREADYCREATED");
        break;
    // case MAKE_DDHRESULT(630):
    //     sprintf(buf, "DDERR_NONONLOCALVIDMEM");
    //     break;
    case MAKE_DDHRESULT(640):
        sprintf(buf, "DDERR_CANTPAGELOCK");
        break;
    case MAKE_DDHRESULT(660):
        sprintf(buf, "DDERR_CANTPAGEUNLOCK");
        break;
    case MAKE_DDHRESULT(680):
        sprintf(buf, "DDERR_NOTPAGELOCKED");
        break;
    // case MAKE_DDHRESULT(690):
    //     sprintf(buf, "DDERR_MOREDATA");
    //     break;
    // case MAKE_DDHRESULT(695):
    //     sprintf(buf, "DDERR_VIDEONOTACTIVE");
    //     break;
    // case MAKE_DDHRESULT(699):
    //     sprintf(buf, "DDERR_DEVICEDOESNTOWNSURFACE");
    //     break;
    case CO_E_NOTINITIALIZED:
        sprintf(buf, "DDERR_NOTINITIALIZED");
        break;
    default:
        sprintf(buf, "Unknown Error");
        break;
    }
}

#include <d3d.h>

// FUNCTION: REDLINE 0x004AA65B
void Log::D3dErrToString(int err, char *buf) {
    switch (err) {
    case D3DERR_BADMAJORVERSION:
        sprintf(buf, "D3DERR_BADMAJORVERSION");
        break;
    case D3DERR_BADMINORVERSION:
        sprintf(buf, "D3DERR_BADMINORVERSION");
        break;
    case D3DERR_EXECUTE_CREATE_FAILED:
        sprintf(buf, "D3DERR_EXECUTE_CREATE_FAILED");
        break;
    case D3DERR_EXECUTE_DESTROY_FAILED:
        sprintf(buf, "D3DERR_EXECUTE_DESTROY_FAILED");
        break;
    case D3DERR_EXECUTE_LOCK_FAILED:
        sprintf(buf, "D3DERR_EXECUTE_LOCK_FAILED");
        break;
    case D3DERR_EXECUTE_UNLOCK_FAILED:
        sprintf(buf, "D3DERR_EXECUTE_UNLOCK_FAILED");
        break;
    case D3DERR_EXECUTE_LOCKED:
        sprintf(buf, "D3DERR_EXECUTE_LOCKED");
        break;
    case D3DERR_EXECUTE_NOT_LOCKED:
        sprintf(buf, "D3DERR_EXECUTE_NOT_LOCKED");
        break;
    case D3DERR_EXECUTE_FAILED:
        sprintf(buf, "D3DERR_EXECUTE_FAILED");
        break;
    case D3DERR_EXECUTE_CLIPPED_FAILED:
        sprintf(buf, "D3DERR_EXECUTE_CLIPPED_FAILED");
        break;
    case D3DERR_TEXTURE_NO_SUPPORT:
        sprintf(buf, "D3DERR_TEXTURE_NO_SUPPORT");
        break;
    case D3DERR_TEXTURE_CREATE_FAILED:
        sprintf(buf, "D3DERR_TEXTURE_CREATE_FAILED");
        break;
    case D3DERR_TEXTURE_DESTROY_FAILED:
        sprintf(buf, "D3DERR_TEXTURE_DESTROY_FAILED");
        break;
    case D3DERR_TEXTURE_LOCK_FAILED:
        sprintf(buf, "D3DERR_TEXTURE_LOCK_FAILED");
        break;
    case D3DERR_TEXTURE_UNLOCK_FAILED:
        sprintf(buf, "D3DERR_TEXTURE_UNLOCK_FAILED");
        break;
    case D3DERR_TEXTURE_LOAD_FAILED:
        sprintf(buf, "D3DERR_TEXTURE_LOAD_FAILED");
        break;
    case D3DERR_TEXTURE_SWAP_FAILED:
        sprintf(buf, "D3DERR_TEXTURE_SWAP_FAILED");
        break;
    case D3DERR_TEXTURE_LOCKED:
        sprintf(buf, "D3DERR_TEXTURE_LOCKED");
        break;
    case D3DERR_TEXTURE_NOT_LOCKED:
        sprintf(buf, "D3DERR_TEXTURE_NOT_LOCKED");
        break;
    case D3DERR_TEXTURE_GETSURF_FAILED:
        sprintf(buf, "D3DERR_TEXTURE_GETSURF_FAILED");
        break;
    case D3DERR_MATRIX_CREATE_FAILED:
        sprintf(buf, "D3DERR_MATRIX_CREATE_FAILED");
        break;
    case D3DERR_MATRIX_DESTROY_FAILED:
        sprintf(buf, "D3DERR_MATRIX_DESTROY_FAILED");
        break;
    case D3DERR_MATRIX_SETDATA_FAILED:
        sprintf(buf, "D3DERR_MATRIX_SETDATA_FAILED");
        break;
    case D3DERR_MATRIX_GETDATA_FAILED:
        sprintf(buf, "D3DERR_MATRIX_GETDATA_FAILED");
        break;
    case D3DERR_SETVIEWPORTDATA_FAILED:
        sprintf(buf, "D3DERR_SETVIEWPORTDATA_FAILED");
        break;
    case D3DERR_MATERIAL_CREATE_FAILED:
        sprintf(buf, "D3DERR_MATERIAL_CREATE_FAILED");
        break;
    case D3DERR_MATERIAL_DESTROY_FAILED:
        sprintf(buf, "D3DERR_MATERIAL_DESTROY_FAILED");
        break;
    case D3DERR_MATERIAL_SETDATA_FAILED:
        sprintf(buf, "D3DERR_MATERIAL_SETDATA_FAILED");
        break;
    case D3DERR_MATERIAL_GETDATA_FAILED:
        sprintf(buf, "D3DERR_MATERIAL_GETDATA_FAILED");
        break;
    case D3DERR_LIGHT_SET_FAILED:
        sprintf(buf, "D3DERR_LIGHT_SET_FAILED");
        break;
    case D3DERR_SCENE_IN_SCENE:
        sprintf(buf, "D3DERR_SCENE_IN_SCENE");
        break;
    case D3DERR_SCENE_NOT_IN_SCENE:
        sprintf(buf, "D3DERR_SCENE_NOT_IN_SCENE");
        break;
    case D3DERR_SCENE_BEGIN_FAILED:
        sprintf(buf, "D3DERR_SCENE_BEGIN_FAILED");
        break;
    case D3DERR_SCENE_END_FAILED:
        sprintf(buf, "D3DERR_SCENE_END_FAILED");
        break;
    default:
        sprintf(buf, "Unknown Error");
        break;
    }
}
