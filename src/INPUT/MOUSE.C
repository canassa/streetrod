#include <dos.h>
#include "MOUSE.H"
#include "INPUTDAT.H"
#include "PC.H"
#include "SYS.H"

/* Mouse driver present? Returns the INT 33h reset result (0 if the vector is empty) */

int mo_reset(void)
{
    union REGS r;

    if (*MOUSE_VECTOR == 0L)
        return 0;
    r.h.ah = 0;
    r.h.al = MOUSE_RESET;
    int86(MOUSE_INT, &r, &r);
    return r.x.ax;
}

/* Mouse function 3, buttons: bl bit 0 and bit 1 stored in two globals */

void mo_buttons(void)
{
    union REGS r;

    r.h.ah = 0;
    r.h.al = MOUSE_GET_POSITION;
    int86(MOUSE_INT, &r, &r);
    g_mouse.left = r.h.bl & MOUSE_LEFT_BUTTON;
    g_mouse.right = r.h.bl & MOUSE_RIGHT_BUTTON;
}

/* Mouse function 0Bh, motion counters returned through two pointers */

void mo_motion(int *dx, int *dy)
{
    union REGS r;

    r.h.ah = 0;
    r.h.al = MOUSE_READ_MOTION;
    int86(MOUSE_INT, &r, &r);
    *dx = r.x.cx;
    *dy = r.x.dx;
}

/* Mouse motion polling; accumulates mickeys and converts to pixel steps */

void mo_poll(void)
{
    int mx, my;
    int mag, neg;

    mo_motion(&mx, &my);
    g_mouse.mickeysX += mx;
    g_mouse.mickeysY += my;
    if (g_mouse.mickeysX < 0) {
        mag = -g_mouse.mickeysX;
        neg = -1;
    } else {
        mag = g_mouse.mickeysX;
        neg = 0;
    }
    g_mouse.dx = mag >> g_mouse.shiftX;
    if (g_mouse.dx) {
        if (neg)
            g_mouse.dx = -g_mouse.dx;
        g_mouse.mickeysX = 0;
    }
    if (g_mouse.mickeysY < 0) {
        mag = -g_mouse.mickeysY;
        neg = -1;
    } else {
        mag = g_mouse.mickeysY;
        neg = 0;
    }
    g_mouse.dy = mag >> g_mouse.shiftY;
    if (g_mouse.dy) {
        if (neg)
            g_mouse.dy = -g_mouse.dy;
        g_mouse.mickeysY = 0;
    }
    mo_buttons();
    if (g_mouse.left) {
        if (g_buttons == 0) {
            g_click = 1;
            g_clickX = g_cursorX;
            g_clickY = g_cursorY;
        }
        g_buttons |= 4;
    } else
        g_buttons &= ~4;
}
