/* MOUSE.H - declarations of what MOUSE.C defines. */

#ifndef MOUSE_H
#define MOUSE_H
#include "STRUCTS.H"

int mo_reset(void);                                  /* 0x16D1A */
void mo_buttons(void);                               /* 0x16D57 */
void mo_motion(int *dx, int *dy);                    /* 0x16D94 */
void mo_poll(void);                                  /* 0x16DC7 */

#endif
