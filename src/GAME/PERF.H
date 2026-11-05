/* PERF.H - declarations of what PERF.C defines. */

#ifndef PERF_H
#define PERF_H

#define ROOF_SPEED_BONUS 0.09
#define REAR_BUMPER_SPEED_BONUS 0.05
#define FRONT_BUMPER_SPEED_BONUS 0.01
#define MODEL_SPEED_WEIGHT 0.1
#define MODEL_SCORE_MAX 88.0
#define INTAKE_SPEED_WEIGHT 0.15
#define ENGINE_SPEED_WEIGHT 0.25
#define TRANSMISSION_SPEED_WEIGHT 0.15
#define TIMING_SPEED_WEIGHT 0.2
#define AUTO_SPEED_FACTOR 0.8
#define PERFORMANCE_RATING_WEIGHT 0.3
#define PERFORMANCE_BASE_FACTOR 0.7

#define RPM_BAND_COUNT 3
#define DRIVE_GEAR_SLOTS 5
#define V6_LOW_BAND_RPM 2900
#define SMALL_V8_LOW_BAND_RPM 2900
#define LARGE_V8_LOW_BAND_RPM 2500

enum EngineShiftRpm {
    V6_SHIFT_RPM = 4500,
    SMALL_V8_SHIFT_RPM = 5000,
    LARGE_V8_SHIFT_RPM = 5200
};

#define ENGINE_REDLINE_RPM 5700
#define ENGINE_WEAR_RPM 5000
#define PART_WEAR_LIMIT 9900
#define PART_WEAR_FULL 10000
#define ENGINE_IDLE_RPM 1500
#define AUTO_MAX_RPM 5690
#define AUTO_DOWNSHIFT_RPM 2000
#define PERFORMANCE_MAX_MPH 130.0


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
