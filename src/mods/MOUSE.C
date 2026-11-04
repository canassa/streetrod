#include <dos.h>
#include "STRUCTS.H"
#include "MOUSE.H"
#include "M_02B9A.H"
#include "PC.H"
#include "SYS.H"

/* mouse_reset @ 0F38:799A (linear 0x16D1A, 61 bytes)

   Mouse driver present? Returns the INT 33h reset result (0 if the vector is empty) */

int mouse_reset(void)
{
    union REGS r;

    if (*MOUSE_VECTOR == 0L)
        return 0;
    r.h.ah = MOUSE_RESET >> 8;
    r.h.al = MOUSE_RESET;
    int86(MOUSE_INT, &r, &r);
    return r.x.ax;
}

/* mouse_buttons @ 0F38:79D7 (linear 0x16D57, 61 bytes)

   Mouse function 3, buttons: bl bit 0 and bit 1 stored in two globals */

void mouse_buttons(void)
{
    union REGS r;

    r.h.ah = MOUSE_GET_POSITION >> 8;
    r.h.al = MOUSE_GET_POSITION;
    int86(MOUSE_INT, &r, &r);
    g_mouseLeft = r.h.bl & MOUSE_LEFT_BUTTON;
    g_mouseRight = r.h.bl & MOUSE_RIGHT_BUTTON;
}

/* mouse_motion @ 0F38:7A14 (linear 0x16D94, 51 bytes)

   Mouse function 0Bh, motion counters returned through two pointers */

void mouse_motion(int *dx, int *dy)
{
    union REGS r;

    r.h.ah = MOUSE_READ_MOTION >> 8;
    r.h.al = MOUSE_READ_MOTION;
    int86(MOUSE_INT, &r, &r);
    *dx = r.x.cx;
    *dy = r.x.dx;
}

/* mouse_poll @ 0F38:7A47 (linear 0x16DC7, 218 bytes)

   Mouse motion polling; accumulates mickeys and converts to pixel steps */

void mouse_poll(void)
{
    int mx, my;
    int mag, neg;

    mouse_motion(&mx, &my);
    g_mouseAccX += mx;
    g_mouseAccY += my;
    if (g_mouseAccX < 0) {
        mag = -g_mouseAccX;
        neg = -1;
    } else {
        mag = g_mouseAccX;
        neg = 0;
    }
    g_mouseDx = mag >> g_mouseShiftX;
    if (g_mouseDx) {
        if (neg)
            g_mouseDx = -g_mouseDx;
        g_mouseAccX = 0;
    }
    if (g_mouseAccY < 0) {
        mag = -g_mouseAccY;
        neg = -1;
    } else {
        mag = g_mouseAccY;
        neg = 0;
    }
    g_mouseDy = mag >> g_mouseShiftY;
    if (g_mouseDy) {
        if (neg)
            g_mouseDy = -g_mouseDy;
        g_mouseAccY = 0;
    }
    mouse_buttons();
    if (g_mouseLeft) {
        if (g_buttons == 0) {
            g_click = 1;
            g_clickX = g_cursorX;
            g_clickY = g_cursorY;
        }
        g_buttons |= 4;
    } else
        g_buttons &= ~4;
}
