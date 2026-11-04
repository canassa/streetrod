/* MOUSE.H - declarations of what MOUSE.C defines. */

#ifndef MOUSE_H
#define MOUSE_H
#include "STRUCTS.H"

int mo_reset(void);
void mo_buttons(void);
void mo_motion(int *dx, int *dy);
void mo_poll(void);

#endif
