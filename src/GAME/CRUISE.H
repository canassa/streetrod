/* CRUISE.H - declarations of what CRUISE.C defines. */

#ifndef CRUISE_H
#define CRUISE_H
#include "STRUCTS.H"

extern char g_savedPal12;
extern char g_savedPal3;
extern int g_phaseDriveRunState;
extern int g7d66n;
void cr_go(void);
void cr_rival_paint(int mode);
int cr_drive(void);

#endif
