/* MOUSE.H - declarations of what MOUSE.C defines. */

#ifndef MOUSE_H
#define MOUSE_H
#include "STRUCTS.H"

int mouse_reset(void);                                  /* 0x16D1A */
void mouse_buttons(void);                               /* 0x16D57 */
void mouse_motion(int *dx, int *dy);                    /* 0x16D94 */
void mouse_poll(void);                                  /* 0x16DC7 */

#endif
