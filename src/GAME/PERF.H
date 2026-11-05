/**
 * @file PERF.H
 * @brief Car performance curves and runtime driving state.
 */

#ifndef PERF_H
#define PERF_H

#include "STRUCTS.H"

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

/**
 * @brief Transmission mode and automatic selector position.
 */
enum AutoSelector {
    SELECTOR_MANUAL = 0,  /**< Manual transmission; no automatic selector. */
    SELECTOR_NEUTRAL = 1, /**< Automatic transmission in neutral. */
    SELECTOR_DRIVE = 2    /**< Automatic transmission in drive. */
};

/**
 * @brief Neutral or a forward gear in the drivetrain curves.
 */
enum DriveGear {
    DRIVE_NEUTRAL = 0,   /**< Neutral, with a free-rev engine curve. */
    DRIVE_FIRST = 1,     /**< First forward gear. */
    DRIVE_SECOND = 2,    /**< Second forward gear. */
    DRIVE_THIRD = 3,     /**< Third forward gear. */
    DRIVE_FOURTH = 4,    /**< Fourth forward gear. */
    DRIVE_GEAR_SLOTS = 5 /**< Number of neutral and forward-gear curve slots. */
};

/**
 * @brief Engine speed band within a gear's performance curve.
 */
enum RpmBand {
    RPM_BAND_LOW = 0,    /**< Below the engine's first curve breakpoint. */
    RPM_BAND_MIDDLE = 1, /**< Between the two curve breakpoints. */
    RPM_BAND_HIGH = 2,   /**< Above the second curve breakpoint. */
    RPM_BAND_COUNT = 3   /**< Number of performance bands per gear. */
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
    struct Range bands[RPM_BAND_COUNT];
};

/**
 * @brief One car's driving state and generated drivetrain curves.
 *
 * Used for both player and rival cars; steering and dashboard state serve the
 * player. Configuration comes from installed parts or a stock specification.
 */
struct DrivingState {
    int speed; /**< Speed in miles per hour. */
    /** Signed steering impulse, clamped to +/-24. */
    int steeringInput;
    /** Signed wheel deflection, decaying toward zero. */
    int steering;
    char unused1[4]; /**< Unused storage. */
    /** Road index beyond which a tire fails; zero disables. */
    int tireFailureRoadIndex;
    /** Added curve-drift factor from tire type and wear. */
    int tireSlip;
    int rpm; /**< Engine revolutions per minute. */
    /** Initial neutral band's lower bound; not read. */
    int minRpm;
    /** Cached band upper bound, refreshed by rival simulation. */
    int maxRpm;
    /** RPM added per simulation update under throttle. */
    int accelStep;
    /** RPM removed per simulation update while coasting. */
    int decelStep;
    enum DriveGear gearIndex; /**< Current neutral or forward gear. */
    /** TRUE requests shifter redraw and band-rate refresh. */
    BOOL shifterDirty;
    enum DriveGear topGear; /**< Highest forward gear available. */
    /** Recommended engine shift threshold. */
    enum EngineShiftRpm shiftRpm;
    /** TRUE for an automatic transmission. */
    BOOL automatic;
    enum EngineShiftRpm autoShiftRpm; /**< Automatic upshift threshold. */
    enum AutoSelector autoSelector;   /**< Manual mode or automatic selector. */
    int unused2;                      /**< Written as -1 on reset; not read. */
    int unused3;                      /**< Written as -1 on reset; not read. */
    /** Reset conversion ratio; no runtime writer. */
    int resetRpmPerSpeed;
    char unused4[8]; /**< Unused storage. */
    /** RPM per mph by gear; final entry is an unread tire/model value. */
    int gearRpmPerSpeed[DRIVE_GEAR_SLOTS + 1];
    /** Active RPM-per-mph conversion ratio; zero after reset. */
    int rpmPerSpeed;
    enum RpmBand rpmBand;               /**< Current performance-curve band. */
    struct Gear gear[DRIVE_GEAR_SLOTS]; /**< Neutral and forward-gear curves. */
    /**
     * Race result mask: @ref STOP_LOST, @ref STOP_COLLISION, @ref STOP_ENGINE,
     * @ref STOP_TIRES, @ref STOP_TRANSMISSION, @ref STOP_ROADSIDE_CRASH,
     * @ref STOP_POLICE and @ref STOP_JUMP_START; zero when clear.
     */
    int stopFlags;
    /** Write-only elapsed-update snapshot when driving stops. */
    int unused5;
    /** Simulation updates, about 6.069 per second in races. */
    int elapsedTicks;
};

void near pf_drivetrain(struct Car *c, struct DrivingState *e);
void near pf_drivetrain_spec(int n, struct DrivingState *e);
int near pf_vmax(int n);

#endif
