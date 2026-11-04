/* CRUISE.H - declarations of what CRUISE.C defines. */

#ifndef CRUISE_H
#define CRUISE_H
#include "STRUCTS.H"

extern char g_savedPal12;                               /* DS:51AC */
extern char g_savedPal3;                                /* DS:51AD */
extern int g_phaseDriveRunState;                                      /* DS:79A6 */
extern int g7d66n;                                      /* DS:7D66 */
void cruise_go(void);                                   /* 0x08D26 */
void cruise_rival_paint(int mode);                      /* 0x08D9C */
int cruise_drive(void);                                 /* 0x08E2D */

#endif
