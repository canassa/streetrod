/**
 * @file DRVTICK.H
 * @brief Race starting sequence and automatic town driving.
 */

#ifndef DRVTICK_H
#define DRVTICK_H

/**
 * @name Town automatic throttle
 * @brief RPM cap and strict upshift threshold while town cruising.
 * @{
 */
#define CRUISE_MAX_RPM 4700
#define CRUISE_SHIFT_RPM 4500
/** @} */
/**
 * @name Race start delay
 * @brief Timer ticks before Go: 110 plus a random 0..102 ticks.
 * @{
 */
#define START_WAIT_RANDOM_TICKS 103
#define START_WAIT_BASE_TICKS 110
/** @} */

#include "SYS.H"

extern int g_keepCruiseMusic;
extern struct DrivingState g_oppEngState;
int dt_step(void);

#endif
