/* PERF.H - declarations of what PERF.C defines. */

#ifndef PERF_H
#define PERF_H


struct Car;

enum AutoSelector {
    SELECTOR_MANUAL = 0,
    SELECTOR_NEUTRAL = 1,
    SELECTOR_DRIVE = 2
};

/* one range of the engine curve: 8 bytes */
struct Range {
    int minRpm;
    int maxRpm;
    int accelStep;
    int decelStep;
};

/* one gear: three ranges */
struct Gear {
    struct Range bands[3];
};

/* drive train state; g_driveEng and g_oppEngState are two instances, 0xc2 bytes each */
struct Eng {
    int speed;
    int steeringInput;
    int steering;
    char unused1[4];
    int tireFailureIndex;
    int tireSlip;
    int rpm;
    int minRpm;
    int maxRpm;
    int accelStep;
    int decelStep;
    int gearIndex;
    int shifterDirty;
    int gearCount;
    int shiftRpm;
    int automatic;
    int autoShiftRpm;
    enum AutoSelector autoSelector;
    int unused2;
    int unused3;
    int initialRpmPerSpeed;
    char unused4[8];
    int gearRpmPerSpeed[6];
    int rpmPerSpeed;
    int rpmBand;
    struct Gear gear[5];
    int stopFlags;
    int unused5;
    int elapsedTicks;
};

void near pf_drivetrain(struct Car *c, struct Eng *e);
void near pf_drivetrain_spec(int n, struct Eng *e);
int near pf_vmax(int n);

#endif
