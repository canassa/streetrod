/* DRVTICK.H - declarations of what DRVTICK.C defines. */

#ifndef DRVTICK_H
#define DRVTICK_H

#define CRUISE_MAX_RPM 4700
#define CRUISE_SHIFT_RPM 4500
#define START_WAIT_RANDOM_TICKS 103
#define START_WAIT_BASE_TICKS 110

#include "SYS.H"

extern int g_keepCruiseMusic;
extern struct DrivingState g_oppEngState;
int dt_step(void);

#endif
