#include <math.h>
#include "STRUCTS.H"
#include "PERF.H"
#include "DISK.H"
#include "GARAGE.H"
#include "M_02B9A.H"
#include "SCREEN.H"

int g_gearSpeedPct[4][5] = {{0, 25, 50, 100, 0}, {0, 25, 50, 100, 0}, {0, 25, 50, 75, 100}, {0, 30, 50, 80, 100}};   /* DS:54DC */
int g_rpmStepBase[3][5] = {{500, 600, 300, 100, 70}, {600, 700, 350, 120, 90}, {600, 760, 380, 140, 90}};   /* DS:5504 */
int g_rpmBands[3][2] = {{2900, 4500}, {2900, 5000}, {2500, 5200}};   /* DS:5522 */
int g_carbManifoldFit[3][5] = {{0, 0, 100, 0, 0}, {0, 50, 0, 100, 75}, {0, 0, 0, 50, 100}};   /* DS:552E */
int g_engineScore[3] = {0, 50, 100};   /* DS:554C */
int g_transScore[4] = {0, 33, 66, 100};   /* DS:5552 */
int g_shiftRpm[3] = {4500, 5000, 5200};   /* DS:555A */
int g_transGears[4] = {3, 3, 4, 4};   /* DS:5560 */
int g_5568[3] = {9, 8, 5};   /* DS:5568 */
int g_modelScore[9] = {40, 44, 48, 60, 64, 68, 80, 84, 88};   /* DS:556E */

/* perf_drivetrain @ 0000:DCBE (linear 0x0DCBE, 1060 bytes)

   Fills the drive train e (gear ranges, ratio table, limits) for the car c. The first half
   is the rating computation of F_0E218. */

void near perf_drivetrain(struct Car *c, struct Eng *e)
{
    double w9, vl;
    struct Gear *gl;
    int a3, d;
    double xl;
    int c3, e1, v3, i, k;
    struct Node *m;
    int bl;
    struct Node **qn;
    struct Node *n6k;

    qn = c->rack.parts;
    n6k = c->transmission;
    m = c->tires;
    a3 = g_4806[qn[0]->idx].variant;
    bl = g_4806[qn[1]->idx].variant;
    c3 = g_4806[qn[2]->idx].variant;
    d = g_4806[n6k->idx].variant;
    e1 = g_4806[m->idx].variant;
    vl = ((c->appearance & 0x2000) || w_8df0[c->specIndex].roofPic == 0) * 0.09
        + ((c->appearance & 0x8000) || w_8df0[c->specIndex].rearBumperPic == 0) * 0.05
        + ((c->appearance & 0x4000) || w_8df0[c->specIndex].frontBumperPic == 0) * 0.01
        + g_modelScore[g7d86n[c->specIndex].modelKind] * 0.1 / 88.0
        + g_carbManifoldFit[c3][bl] * 0.15 / 100.0
        + g_engineScore[a3] * 0.25 / 100.0
        + g_transScore[d] * 0.15 / 100.0
        + (1.0 - fabs(c->timing / 8.0)) * 0.2;
    vl = (10000.0 - qn[0]->wear) / 10000.0 * vl;
    vl = (d == 0 ? 0.8 : 1.0) * vl * 0.3 + 0.7;
    w9 = vl * 130.0;
    if (w9 > 130.0)
        w9 = 130.0;
    for (i = 0; i <= 4; i++) {
        xl = g_gearSpeedPct[d][i] * w9 / 100.0;
        if (xl == 0.0)
            xl = 0.2;
        e->f34[i] = (int)(5700.0 / xl);
        gl = &e->gear[i];
        gl->bands[0].minRpm = 0;
        gl->bands[1].minRpm = gl->bands[0].maxRpm = g_rpmBands[a3][0];
        gl->bands[2].minRpm = gl->bands[1].maxRpm = g_rpmBands[a3][1];
        gl->bands[2].maxRpm = 0x1644;
        v3 = (int)(g_rpmStepBase[a3][i] * vl);
        gl->bands[0].accelStep = ((i == 1 && e1 == 2) ? 3 : 1) * ((i == 1 && e1 != 2) ? 2 : 1) * (v3 / (i ? 5 : 1));
        gl->bands[1].accelStep = v3;
        gl->bands[2].accelStep = v3 / (i ? 10 : 1);
        for (k = 0; k < 3; k++)
            gl->bands[k].decelStep = i ? v3 / (5 - i) : v3;
    }
    e->automatic = e->autoSelector = (d == 0);
    if (e->automatic) {
        e->f26 = e->f28 = -1;
        e->autoShiftRpm = gl->bands[1].maxRpm;
    }
    e->shiftRpm = g_shiftRpm[a3];
    e->f34[5] = (int)((int)(0x82 - g7d86n[c->specIndex].modelKind) * 5.0 / 50.0 + g_5568[g_4806[m->idx].variant]);
    e->gearCount = g_transGears[d];
    e->shifterDirty = 1;
}

/* perf_drivetrain_spec @ 0000:E0E2 (linear 0x0E0E2, 310 bytes)

   Builds a struct Car from the spec n of g7d86n and hands it to perf_drivetrain. */

void near perf_drivetrain_spec(int n, struct Eng *e)
{
    struct Car car;
    struct Node nd[5];
    struct Node na, lo;
    struct Spec *s;
    int t, i, j;

    car.timing = 0;
    car.specIndex = n;
    car.transmission = &na;
    car.tires = &lo;
    s = &g7d86n[n];
    car.rack.parts[3] = car.rack.parts[4] = 0;
    t = ((s->parts >> 14) & 3) + 0x16;
    for (i = g_5126[(s->parts >> 11) & 7] + 1; i >= 0; i--) {
        car.rack.parts[i] = &nd[i];
        nd[i].wear = g_496a;
        nd[i].idx = t;
    }
    i = (s->parts >> 6) & 3;
    na.idx = (t = (s->parts & 0xf) >> 1) + (i == 3) + i * 3 + 9;
    lo.idx = ((s->parts >> 4) & 3) + 0x28;
    nd[0].idx = ((s->parts >> 8) & 7) * 3 + t;
    nd[1].idx = (j = (s->parts >> 11) & 7) * 3 + t + 0x19;
    perf_drivetrain(&car, e);
}

/* F_0E218 @ 0000:E218 (linear 0x0E218, 849 bytes)

   Rating of the car spec n (the car in a_7eaap.m7eb0 when n is negative): builds the
   car the way perf_drivetrain_spec does, then folds its parts into one number. */

int near F_0E218(int n)
{
    double w9, vl;
    struct Spec *s;
    int a3, t, d, c3;
    struct Car *p6;
    int e1;
    struct Car car;
    int i;
    struct Node lon;
    int j;
    struct Node *m;
    int bl;
    struct Node **qn;
    struct Node ndm[5];
    struct Node *n6k;
    struct Node na;

    if (n < 0)
        p6 = a_7eaap.m7eb0;
    else {
        car.timing = 0;
        car.specIndex = n;
        car.transmission = &na;
        car.tires = &lon;
        s = &g7d86n[n];
        car.rack.parts[3] = car.rack.parts[4] = 0;
        t = ((s->parts >> 14) & 3) + 0x16;
        for (i = g_5126[(s->parts >> 11) & 7] + 1; i >= 0; i--) {
            car.rack.parts[i] = &ndm[i];
            ndm[i].wear = g_496a;
            ndm[i].idx = t;
        }
        i = (s->parts >> 6) & 3;
        na.idx = (t = (s->parts & 0xf) >> 1) + (i == 3) + i * 3 + 9;
        lon.idx = ((s->parts >> 4) & 3) + 0x28;
        ndm[0].idx = ((s->parts >> 8) & 7) * 3 + t;
        ndm[1].idx = (j = (s->parts >> 11) & 7) * 3 + t + 0x19;
        p6 = &car;
    }
    qn = p6->rack.parts;
    n6k = p6->transmission;
    m = p6->tires;
    a3 = g_4806[qn[0]->idx].variant;
    bl = g_4806[qn[1]->idx].variant;
    c3 = g_4806[qn[2]->idx].variant;
    d = g_4806[n6k->idx].variant;
    e1 = g_4806[m->idx].variant;
    vl = ((p6->appearance & 0x2000) || w_8df0[p6->specIndex].roofPic == 0) * 0.09
        + ((p6->appearance & 0x8000) || w_8df0[p6->specIndex].rearBumperPic == 0) * 0.05
        + ((p6->appearance & 0x4000) || w_8df0[p6->specIndex].frontBumperPic == 0) * 0.01
        + g_modelScore[g7d86n[p6->specIndex].modelKind] * 0.1 / 88.0
        + g_carbManifoldFit[c3][bl] * 0.15 / 100.0
        + g_engineScore[a3] * 0.25 / 100.0
        + g_transScore[d] * 0.15 / 100.0
        + (1.0 - fabs(p6->timing / 8.0)) * 0.2;
    vl = (10000.0 - qn[0]->wear) / 10000.0 * vl;
    vl = (d == 0 ? 0.8 : 1.0) * vl * 0.3 + 0.7;
    w9 = vl * 130.0;
    if (w9 > 130.0)
        w9 = 130.0;
    return (int)w9;
}
