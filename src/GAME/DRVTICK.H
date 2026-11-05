/* DRVTICK.H - declarations of what DRVTICK.C defines. */

#ifndef DRVTICK_H
#define DRVTICK_H

#include "SYS.H"

extern int g_keepCruiseMusic;
extern struct Eng g_oppEngState;
int dt_step(void);

#endif
