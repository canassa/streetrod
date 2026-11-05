#include <math.h>
#include "PERF.H"
#include "DISK.H"
#include "GAME.H"
#include "GARAGE.H"
#include "INPUTDAT.H"
#include "POOLS.H"
#include "SCREEN.H"
#include "STRUCTS.H"

static int g_gearSpeedPct[4][DRIVE_GEAR_SLOTS] = {
    {0, 25, 50, 100, 0}, {0, 25, 50, 100, 0}, {0, 25, 50, 75, 100}, {0, 30, 50, 80, 100}};
static int g_rpmStepBase[3][DRIVE_GEAR_SLOTS] = {
    {500, 600, 300, 100, 70}, {600, 700, 350, 120, 90}, {600, 760, 380, 140, 90}};
static int g_rpmBands[3][2] = {{V6_LOW_BAND_RPM, V6_SHIFT_RPM},
                               {SMALL_V8_LOW_BAND_RPM, SMALL_V8_SHIFT_RPM},
                               {LARGE_V8_LOW_BAND_RPM, LARGE_V8_SHIFT_RPM}};
static int g_carbManifoldFit[3][5] = {{0, 0, 100, 0, 0}, {0, 50, 0, 100, 75}, {0, 0, 0, 50, 100}};
static int g_engineScore[3] = {0, 50, 100};
static int g_transScore[4] = {0, 33, 66, 100};
static enum EngineShiftRpm g_shiftRpm[3] = {V6_SHIFT_RPM, SMALL_V8_SHIFT_RPM, LARGE_V8_SHIFT_RPM};
static enum DriveGear g_transGears[4] = {DRIVE_THIRD, DRIVE_THIRD, DRIVE_FOURTH, DRIVE_FOURTH};
static int g_tireFactors[3] = {9, 8, 5};
static int g_modelScore[9] = {40, 44, 48, 60, 64, 68, 80, 84, 88};

/* Fills the drive train e (gear ranges, ratio table, limits) for the car c. The first half
   is the rating computation of pf_vmax. */

void near pf_drivetrain(struct Car *c, struct DrivingState *e)
{
    double w9, vl;
    struct Gear *gl;
    int a3, d;
    double xl;
    int c3, e1, v3;
    enum DriveGear i;
    enum RpmBand k;
    struct Node *m;
    int bl;
    struct Node **qn;
    struct Node *n6k;

    qn = c->rack.parts;
    n6k = c->transmission;
    m = c->tires;
    a3 = g_parts[qn[0]->idx].variant;
    bl = g_parts[qn[1]->idx].variant;
    c3 = g_parts[qn[2]->idx].variant;
    d = g_parts[n6k->idx].variant;
    e1 = g_parts[m->idx].variant;
    vl = ((c->appearance & APPEARANCE_ROOF) || g_models[c->specIndex].choppedRoofPic == 0) * ROOF_SPEED_BONUS +
         ((c->appearance & APPEARANCE_REAR_BUMPER) || g_models[c->specIndex].rearBumperRemovalPic == 0) *
             REAR_BUMPER_SPEED_BONUS +
         ((c->appearance & APPEARANCE_FRONT_BUMPER) || g_models[c->specIndex].frontBumperRemovalPic == 0) *
             FRONT_BUMPER_SPEED_BONUS +
         g_modelScore[g_carData[c->specIndex].rank] * MODEL_SPEED_WEIGHT / MODEL_SCORE_MAX +
         g_carbManifoldFit[c3][bl] * INTAKE_SPEED_WEIGHT / 100.0 + g_engineScore[a3] * ENGINE_SPEED_WEIGHT / 100.0 +
         g_transScore[d] * TRANSMISSION_SPEED_WEIGHT / 100.0 + (1.0 - fabs(c->timing / 8.0)) * TIMING_SPEED_WEIGHT;
    vl = ((double)PART_WEAR_FULL - qn[0]->wear) / (double)PART_WEAR_FULL * vl;
    vl = (d == TRANS_AUTOMATIC ? AUTO_SPEED_FACTOR : 1.0) * vl * PERFORMANCE_RATING_WEIGHT + PERFORMANCE_BASE_FACTOR;
    w9 = vl * PERFORMANCE_MAX_MPH;
    if (w9 > PERFORMANCE_MAX_MPH)
        w9 = PERFORMANCE_MAX_MPH;
    for (i = DRIVE_NEUTRAL; i <= DRIVE_GEAR_SLOTS - 1; i++) {
        xl = g_gearSpeedPct[d][i] * w9 / 100.0;
        if (xl == 0.0)
            xl = 0.2;
        e->gearRpmPerSpeed[i] = (int)((double)ENGINE_REDLINE_RPM / xl);
        gl = &e->gear[i];
        gl->bands[RPM_BAND_LOW].minRpm = 0;
        gl->bands[RPM_BAND_MIDDLE].minRpm = gl->bands[RPM_BAND_LOW].maxRpm = g_rpmBands[a3][0];
        gl->bands[RPM_BAND_HIGH].minRpm = gl->bands[RPM_BAND_MIDDLE].maxRpm = g_rpmBands[a3][1];
        gl->bands[RPM_BAND_HIGH].maxRpm = ENGINE_REDLINE_RPM;
        v3 = (int)(g_rpmStepBase[a3][i] * vl);
        gl->bands[RPM_BAND_LOW].accelStep = ((i == DRIVE_FIRST && e1 == TIRES_RACING_SLICKS) ? 3 : 1) *
                                            ((i == DRIVE_FIRST && e1 != TIRES_RACING_SLICKS) ? 2 : 1) *
                                            (v3 / (i ? 5 : 1));
        gl->bands[RPM_BAND_MIDDLE].accelStep = v3;
        gl->bands[RPM_BAND_HIGH].accelStep = v3 / (i ? 10 : 1);
        for (k = RPM_BAND_LOW; k < RPM_BAND_COUNT; k++)
            gl->bands[k].decelStep = i ? v3 / (DRIVE_GEAR_SLOTS - i) : v3;
    }
    e->automatic = e->autoSelector = (d == TRANS_AUTOMATIC);
    if (e->automatic) {
        e->unused2 = e->unused3 = -1;
        e->autoShiftRpm = gl->bands[RPM_BAND_MIDDLE].maxRpm;
    }
    e->shiftRpm = g_shiftRpm[a3];
    e->gearRpmPerSpeed[DRIVE_GEAR_SLOTS] =
        (int)((int)(0x82 - g_carData[c->specIndex].rank) * 5.0 / 50.0 + g_tireFactors[g_parts[m->idx].variant]);
    e->topGear = g_transGears[d];
    e->shifterDirty = TRUE;
}

/* Builds a struct Car from the spec n of g_carData and hands it to pf_drivetrain. */

void near pf_drivetrain_spec(int n, struct DrivingState *e)
{
    struct Car car;
    struct Node nd[5];
    struct Node na, lo;
    struct CarSpec *s;
    int t, i, j;

    car.timing = 0;
    car.specIndex = n;
    car.transmission = &na;
    car.tires = &lo;
    s = &g_carData[n];
    car.rack.parts[3] = car.rack.parts[4] = 0;
    t = ((s->stockParts >> SPEC_CARB_SHIFT) & SPEC_CARB_MASK) + PART_CARBURETOR_FIRST;
    for (i = g_nCarb[(s->stockParts >> SPEC_MANIFOLD_SHIFT) & SPEC_MANIFOLD_MASK] + 1; i >= 0; i--) {
        car.rack.parts[i] = &nd[i];
        nd[i].wear = g_partWear[PART_CARBURETOR];
        nd[i].idx = t;
    }
    i = (s->stockParts >> SPEC_TRANS_SHIFT) & SPEC_TRANS_MASK;
    na.idx = (t = (s->stockParts & SPEC_FIT_MASK) >> 1) + (i == TRANS_RACING) + i * 3 + PART_TRANSMISSION_FIRST;
    lo.idx = ((s->stockParts >> SPEC_TIRES_SHIFT) & SPEC_TIRES_MASK) + PART_TIRES_FIRST;
    nd[0].idx = ((s->stockParts >> SPEC_ENGINE_SHIFT) & SPEC_ENGINE_MASK) * 3 + t;
    nd[1].idx = (j = (s->stockParts >> SPEC_MANIFOLD_SHIFT) & SPEC_MANIFOLD_MASK) * 3 + t + PART_MANIFOLD_FIRST;
    pf_drivetrain(&car, e);
}

/* Rating of the car spec n (the car in g_driver.currentCar when n is negative): builds the
   car the way pf_drivetrain_spec does, then folds its parts into one number. */

int near pf_vmax(int n)
{
    double w9, vl;
    struct CarSpec *s;
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
        p6 = g_driver.currentCar;
    else {
        car.timing = 0;
        car.specIndex = n;
        car.transmission = &na;
        car.tires = &lon;
        s = &g_carData[n];
        car.rack.parts[3] = car.rack.parts[4] = 0;
        t = ((s->stockParts >> SPEC_CARB_SHIFT) & SPEC_CARB_MASK) + PART_CARBURETOR_FIRST;
        for (i = g_nCarb[(s->stockParts >> SPEC_MANIFOLD_SHIFT) & SPEC_MANIFOLD_MASK] + 1; i >= 0; i--) {
            car.rack.parts[i] = &ndm[i];
            ndm[i].wear = g_partWear[PART_CARBURETOR];
            ndm[i].idx = t;
        }
        i = (s->stockParts >> SPEC_TRANS_SHIFT) & SPEC_TRANS_MASK;
        na.idx = (t = (s->stockParts & SPEC_FIT_MASK) >> 1) + (i == TRANS_RACING) + i * 3 + PART_TRANSMISSION_FIRST;
        lon.idx = ((s->stockParts >> SPEC_TIRES_SHIFT) & SPEC_TIRES_MASK) + PART_TIRES_FIRST;
        ndm[0].idx = ((s->stockParts >> SPEC_ENGINE_SHIFT) & SPEC_ENGINE_MASK) * 3 + t;
        ndm[1].idx = (j = (s->stockParts >> SPEC_MANIFOLD_SHIFT) & SPEC_MANIFOLD_MASK) * 3 + t + PART_MANIFOLD_FIRST;
        p6 = &car;
    }
    qn = p6->rack.parts;
    n6k = p6->transmission;
    m = p6->tires;
    a3 = g_parts[qn[0]->idx].variant;
    bl = g_parts[qn[1]->idx].variant;
    c3 = g_parts[qn[2]->idx].variant;
    d = g_parts[n6k->idx].variant;
    e1 = g_parts[m->idx].variant;
    vl = ((p6->appearance & APPEARANCE_ROOF) || g_models[p6->specIndex].choppedRoofPic == 0) * ROOF_SPEED_BONUS +
         ((p6->appearance & APPEARANCE_REAR_BUMPER) || g_models[p6->specIndex].rearBumperRemovalPic == 0) *
             REAR_BUMPER_SPEED_BONUS +
         ((p6->appearance & APPEARANCE_FRONT_BUMPER) || g_models[p6->specIndex].frontBumperRemovalPic == 0) *
             FRONT_BUMPER_SPEED_BONUS +
         g_modelScore[g_carData[p6->specIndex].rank] * MODEL_SPEED_WEIGHT / MODEL_SCORE_MAX +
         g_carbManifoldFit[c3][bl] * INTAKE_SPEED_WEIGHT / 100.0 + g_engineScore[a3] * ENGINE_SPEED_WEIGHT / 100.0 +
         g_transScore[d] * TRANSMISSION_SPEED_WEIGHT / 100.0 + (1.0 - fabs(p6->timing / 8.0)) * TIMING_SPEED_WEIGHT;
    vl = ((double)PART_WEAR_FULL - qn[0]->wear) / (double)PART_WEAR_FULL * vl;
    vl = (d == TRANS_AUTOMATIC ? AUTO_SPEED_FACTOR : 1.0) * vl * PERFORMANCE_RATING_WEIGHT + PERFORMANCE_BASE_FACTOR;
    w9 = vl * PERFORMANCE_MAX_MPH;
    if (w9 > PERFORMANCE_MAX_MPH)
        w9 = PERFORMANCE_MAX_MPH;
    return (int)w9;
}
