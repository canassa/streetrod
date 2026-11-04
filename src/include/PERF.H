/* PERF.H - declarations of what PERF.C defines. */

#ifndef PERF_H
#define PERF_H
#include "STRUCTS.H"

extern int g_gearSpeedPct[][5];                         /* DS:54DC */
extern int g_rpmStepBase[][5];                                 /* DS:5504 */
extern int g_rpmBands[][2];                             /* DS:5522 */
extern int g_carbManifoldFit[][5];                      /* DS:552E */
extern int g_engineScore[];                             /* DS:554C */
extern int g_transScore[];                              /* DS:5552 */
extern int g_shiftRpm[];                                /* DS:555A */
extern int g_transGears[];                              /* DS:5560 */
extern int g_5568[];                                    /* DS:5568 */
extern int g_modelScore[];                              /* DS:556E */
void near perf_drivetrain(struct Car *c, struct Eng *e);/* 0x0DCBE */
void near perf_drivetrain_spec(int n, struct Eng *e);   /* 0x0E0E2 */
int near perf_vmax(int n);                                /* 0x0E218 */

#endif
