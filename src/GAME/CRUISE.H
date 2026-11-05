/* CRUISE.H - declarations of what CRUISE.C defines. */

#ifndef CRUISE_H
#define CRUISE_H

#define RIVAL_PAINT_RESTORE (-1)

enum DrivePhase {
    DRIVE_START = 0,
    DRIVE_STAGING = 1,
    DRIVE_CRUISING = 1,
    DRIVE_WAIT_FOR_MOTION = 2,
    DRIVE_RUNNING = 3
};

extern enum DrivePhase g_phaseDriveRunState;
extern int g_startingGoActive;
void cr_go(void);
void cr_rival_paint(int mode);
int cr_drive(void);

#endif
