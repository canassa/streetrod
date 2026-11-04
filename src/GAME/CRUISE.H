/* CRUISE.H - declarations of what CRUISE.C defines. */

#ifndef CRUISE_H
#define CRUISE_H
#include "STRUCTS.H"

#define RIVAL_PAINT_RESTORE (-1)

enum DrivePhase {
    DRIVE_START = 0,
    DRIVE_STAGING = 1,
    DRIVE_CRUISING = 1,
    DRIVE_WAIT_FOR_MOTION = 2,
    DRIVE_RUNNING = 3
};


extern char g_savedPal12;                               /* DS:51AC */
extern char g_savedPal3;                                /* DS:51AD */
extern enum DrivePhase g_phaseDriveRunState;                                      /* DS:79A6 */
extern int g7d66n;                                      /* DS:7D66 */
void cr_go(void);                                   /* 0x08D26 */
void cr_rival_paint(int mode);                      /* 0x08D9C */
int cr_drive(void);                                 /* 0x08E2D */

#endif
