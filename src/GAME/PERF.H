/* PERF.H - declarations of what PERF.C defines. */

#ifndef PERF_H
#define PERF_H

struct Eng;

struct Car;

extern int g_gearSpeedPct[][5];
extern int g_rpmStepBase[][5];
extern int g_rpmBands[][2];
extern int g_carbManifoldFit[][5];
extern int g_engineScore[];
extern int g_transScore[];
extern int g_shiftRpm[];
extern int g_transGears[];
extern int g_5568[];
extern int g_modelScore[];
void near pf_drivetrain(struct Car *c, struct Eng *e);
void near pf_drivetrain_spec(int n, struct Eng *e);
int near pf_vmax(int n);

#endif
