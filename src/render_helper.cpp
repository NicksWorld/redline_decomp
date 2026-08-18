#include "render_helper.h"

#include "globals.h"
#include "render.h"
#include "texture_mgr.h"

#include <d3dtypes.h>
#include <windows.h>

// GLOBAL: REDLINE 0x005C7538
RenderHelper* g_RenderHelper = NULL;

// FUNCTION: REDLINE 0x0048F48B
short SupportsBlend2() {
    return g_SupportsBlend2;
}

// FUNCTION: REDLINE 0x0048ED71
void RenderDimensions(int* width, int* height) {
    D3dRenderer* renderer = g_Direct3d;
    *width = renderer->width;
    *height = renderer->height;
}

// GLOBAL: REDLINE 0x005A8F30
short g_AlphaBlendEnabled = 0;

// FUNCTION: REDLINE 0x0048FCAF
void SetAlphaBlendEnable(short en) {
    if (g_AlphaBlendEnabled != en) {
        g_AlphaBlendEnabled = en;
        g_Direct3d->D3dDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, en != 0);
    }
}

//GLOBAL: REDLINE 0x005A8F2A
short g_ZEnabled;

// FUNCTION: REDLINE 0x0048FBBA
void SetZEnable(short en) {
    if (g_ZEnabled != en) {
        g_ZEnabled = en;
        g_Direct3d->D3dDevice()->SetRenderState(D3DRENDERSTATE_ZENABLE, en != 0);
    }
}

// GLOBAL: REDLINE 0x005A8F36
short g_BlendMode = 0;

// FUNCTION: REDLINE 0x0048F877
void SetBlendMode(short mode) {
    struct Locals {
        int dst;
        int src;
    } l;
    if (g_BlendMode == 3 && !g_SupportsBlend)
        g_BlendMode = 2;
    if (g_BlendMode != mode) {
        g_BlendMode = mode;

        switch (mode) {
            case 2:
                l.src = D3DBLEND_ONE;
                l.dst = D3DBLEND_ONE;
                break;
            case 3:
                l.src = D3DBLEND_ZERO;
                l.dst = D3DBLEND_INVSRCCOLOR;
                break;
            case 6:
                l.src = D3DBLEND_BOTHSRCALPHA;
                l.dst = D3DBLEND_BOTHSRCALPHA;
                if (!g_SupportsBlend2) {
                    l.src = D3DBLEND_INVSRCALPHA;
                    l.dst = D3DBLEND_SRCALPHA;
                }
                break;
            case 4:
                l.src = D3DBLEND_DESTCOLOR;
                l.dst = D3DBLEND_ZERO;
                break;
            case 5:
                l.src = D3DBLEND_SRCALPHA;
                l.dst = D3DBLEND_ONE;
                break;
            default:
                l.src = D3DBLEND_SRCALPHA;
                l.dst = D3DBLEND_INVSRCALPHA;
                break;
        }
        g_Direct3d->D3dDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND, l.src);
        g_Direct3d->D3dDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, l.dst);
    }
}

// GLOBAL: REDLINE 0x005A54D0
int g_DrawnTriCount = 0;

// FUNCTION: REDLINE 0x0048FD75
void DrawPrimitive(D3DPRIMITIVETYPE ty, int vertexTyDesc, void* verts, unsigned int vertCount, int flags) {
    g_Direct3d->D3dDevice()->DrawPrimitive(ty, vertexTyDesc, verts, vertCount, flags);

    g_DrawnTriCount += (int)(vertCount * 0.333f);
}

// FUNCTION: REDLINE 0x004FF552
void RenderHelper::DrawScreenOverlay(int flags, unsigned char opacity, unsigned char force) {
    if (g_DisplayScreenFlash == 1 || force) {
        if (!SupportsBlend2())
            opacity = -1 - opacity;
        
        int width, height;
        float fwidth, fheight;
        RenderDimensions(&width, &height);
        fwidth = width;
        fheight = height;

        SetAlphaBlendEnable(true);
        SetBlendMode(6);
        SetTexture(0);
        SetZEnable(false);
        D3DTLVERTEX verts[4];

        verts[0].sx = 0.0;
        verts[0].sy = 0.0;
        verts[0].sz = 0.0099999998;
        verts[0].rhw = 1.0;
        verts[0].color = (opacity << 24) | flags;
        verts[0].specular = 0;
        verts[0].tu = 0.0;
        verts[0].tv = 0.0;
        verts[1].sx = fwidth;
        verts[1].sy = 0.0;
        verts[1].sz = 0.0099999998;
        verts[1].rhw = 1.0;
        verts[1].color = verts[0].color;
        verts[1].specular = 0;
        verts[1].tu = 0.0;
        verts[1].tv = 0.0;
        verts[2].sx = 0.0;
        verts[2].sy = fheight;
        verts[2].sz = 0.0099999998;
        verts[2].rhw = 1.0;
        verts[2].color = verts[0].color;
        verts[2].specular = 0;
        verts[2].tu = 0.0;
        verts[2].tv = 0.0;
        verts[3].sx = fwidth;
        verts[3].sy = fheight;
        verts[3].sz = 0.0099999998;
        verts[3].rhw = 1.0;
        verts[3].color = verts[0].color;
        verts[3].specular = 0;
        verts[3].tu = 0.0;
        verts[3].tv = 0.0;

        DrawPrimitive(
                D3DPT_TRIANGLESTRIP,
                D3DFVF_TLVERTEX,
                verts,
                4,
                4);
        SetAlphaBlendEnable(false);
        SetBlendMode(1);
        SetZEnable(true);
    }
}

// FUNCTION: REDLINE 0x0043CDD0
void DrawScreenOverlay(unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned char force) {
    g_RenderHelper->DrawScreenOverlay(b | (g << 8) | (r << 16), a, force);
}
