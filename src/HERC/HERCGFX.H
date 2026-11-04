/* HERCGFX.H - declarations of what HERCGFX.ASM defines. */

#ifndef HERCGFX_H
#define HERCGFX_H
#include "STRUCTS.H"

extern int g_hercCrtc[10];                              /* DS:5ABC */
extern char g_hercBiosData[30];                         /* DS:5AD0 */
void far hg_set_mode(void);                        /* 0x0E5D2 */

#endif
