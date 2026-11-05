/**
 * @file CRUISE.H
 * @brief Town driving and rival body colours.
 */

#ifndef CRUISE_H
#define CRUISE_H

#include "GARAGE.H"

#define TOWN_TRIP_FUEL 2

/** Restore the palette saved by the first rival recolour. */
#define RIVAL_PAINT_RESTORE PAINT_UNASSIGNED

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
/**
 * @brief Apply rival body colours or restore the palette from before the race.
 *
 * Only EGA and Tandy displays use these colours. Recolouring preserves the
 * palette saved by the first call until @ref RIVAL_PAINT_RESTORE consumes it.
 *
 * @param scheme Body-colour pair, or @ref RIVAL_PAINT_RESTORE.
 */
void cr_rival_paint(enum PaintScheme scheme);
int cr_drive(void);

#endif
