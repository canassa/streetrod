/**
 * @file RACE.H
 * @brief Race simulation, results and gas-station visits.
 */

#ifndef RACE_H
#define RACE_H

#include "DISK.H"
#include "GAME.H"
#include "STRUCTS.H"

/**
 * @name Crash repair offers
 * @brief Base dollars plus an exclusive random surcharge by severity.
 * @{
 */
#define REPAIR_BASE_PRICE 100
#define MAJOR_REPAIR_PRICE_RANGE 0x28
#define MINOR_REPAIR_PRICE_RANGE 0x14
/** @} */

/** Dollars added to one tenth of scrap value after a total-loss crash. */
#define TOTAL_LOSS_BASE_VALUE 0x13

/** Fraction of car value paid when declining crash repairs. */
#define JUNK_VALUE_FACTOR 0.4

/** Exclusive initial tire-wear bound for a car won on pink slips. */
#define WON_TIRE_WEAR_RANGE 500

/**
 * @name Post-race tire wear
 * @brief Standard tires use high bases; others use low, plus 0..299 wear.
 * @{
 */
#define ROAD_TIRE_WEAR_HIGH 700
#define DRAG_TIRE_WEAR_HIGH 300
#define ROAD_TIRE_WEAR_LOW 400
#define DRAG_TIRE_WEAR_LOW 200
#define RACE_TIRE_WEAR_RANGE 300
/** @} */

/** Simulation-update frequency used to convert elapsed ticks to seconds. */
#define RACE_TICKS_PER_SECOND 6.068827333333334

/** Completion percentage cap when the car has not finished. */
#define RACE_INCOMPLETE_PCT_MAX 99

/** One in this many ordinary road races schedules a police appearance. */
#define COP_RACE_CHANCE 4

/**
 * @name Police trigger position
 * @brief Road-sample index is the base plus an exclusive random offset.
 * @{
 */
#define COP_APPEAR_RANGE 255
#define COP_APPEAR_BASE 0x1fe
/** @} */

/** @brief Destination selected by race settlement. */
enum RaceExit {
    /** Return to the garage, possibly with a won car or repairs. */
    RACE_EXIT_GARAGE = 1,
    /** Skip return driving after car loss or unaffordable replacement tires. */
    RACE_EXIT_STOPPED = 2,
    RACE_EXIT_BOBS = 3, /**< Resume the encounter at Bob's. */
    /** Visit jail after an unaffordable fine or replacement tires. */
    RACE_EXIT_JAIL = 4
};

/**
 * @name Race course positions
 * @brief Road-sample indices for simulation and completion statistics.
 * @{
 */
#define DRAG_FINISH_INDEX 0x17c
#define ROAD_FINISH_INDEX 0x3fc
#define RACE_START_INDEX 200
/** @} */

/**
 * @name Race fuel budget
 * @brief Tenths of a gallon deducted after racing or required to enter.
 * @{
 */
#define DRAG_RACE_FUEL 5
#define ROAD_RACE_FUEL 15
#define MIN_DRAG_FUEL 7
#define MIN_ROAD_FUEL 17
#define MIN_KING_FUEL 22
/** @} */

/**
 * @name Pump delivery
 * @brief Tenths of a gallon: 30 per fill, clamped to 170 on departure.
 * @{
 */
#define FUEL_TANK_CAPACITY 0xaa
#define FUEL_FILL_AMOUNT 0x1e
/** Fill count is (this value - current fuel) / fuel delivered per fill. */
#define FUEL_FILL_COUNT_BASE 0xbe
/** @} */

/**
 * Dollar charge for leaving without returning the nozzle, capped at available
 * cash.
 */
#define NOZZLE_PENALTY 10

/** Dollars charged after a failure when no spare tires are available. */
#define REPLACEMENT_TIRE_PRICE 0x32

/**
 * @name Tire condition bands
 * @brief Failure requires wear plus a 0..1999 draw to exceed 9500.
 * @{
 */
#define TIRE_FAILURE_WEAR_MIN 8000
#define TIRE_FAILURE_WEAR_LIMIT 9500
/** Wear units per slip-table column and exclusive random failure-test bound. */
#define TIRE_WEAR_BAND 2000
/** @} */

/**
 * @name Crash severity
 * @brief Strict mph thresholds for total loss, major and minor repairs.
 * @{
 */
#define TOTAL_LOSS_MPH 0x5a
#define MAJOR_REPAIR_MPH 0x3c
#define MIN_REPAIR_MPH 0x1e
/** @} */

/**
 * @name Speeding fines
 * @brief Strict mph thresholds select high, middle or low dollar charges.
 * @{
 */
#define FINE_HIGH_MPH 0x50
#define FINE_MID_MPH 0x32
#define FINE_HIGH_PRICE 0x4b
#define FINE_MID_PRICE 0x32
#define FINE_LOW_PRICE 0x14
/** @} */

/**
 * @name Rival RPM targets
 * @brief Automatic upshift and manual top-gear targets.
 * @{
 */
#define RIVAL_AUTO_SHIFT_RPM 0x14b4
/** Slow non-Tandy hosts use the automatic target instead. */
#define RIVAL_TOP_GEAR_RPM 0x1518
/** @} */

struct Bitmap;
struct DrivingState;

/** @brief Cursor shapes shared with the gas-station held-tool state. */
enum GasHand {
    GAS_HAND_GARAGE = 0, /**< Garage pointer. */
    GAS_HAND_EMPTY = 1,  /**< Empty hand. */
    GAS_HAND_NOZZLE = 2, /**< Holding the pump nozzle. */
    GAS_HAND_RAG = 3,    /**< Holding the cleaning rag. */
    GAS_HAND_COUNT = 4   /**< Number of cursor shapes. */
};

/**
 * @brief Car, saved backgrounds and refuelling state for one gas-station visit.
 *
 * The cap backgrounds belong to the visit's arena storage and account for
 * whether the nozzle is on the pump.
 */
struct GasStationVisit {
    int specIndex; /**< Current car's specification-table index. */
    /** Body-overlay bits: @ref BODY_ROOF, @ref BODY_REAR_BUMPER,
     * @ref BODY_FRONT_BUMPER and @ref BODY_LARGE_ENGINE. */
    enum BodyMod bodyMods;
    /** Cap-area background with the nozzle on the pump. */
    struct Bitmap far *capBgNozzleOnPump;
    /** Cap-area background with the nozzle off the pump. */
    struct Bitmap far *capBgNozzleOffPump;
    enum GasHand hand; /**< Held tool and cursor shape. */
    int fillsLeft;     /**< Remaining three-gallon fills. */
    BOOL selfService;  /**< TRUE until the attendant takes over. */
    BOOL nozzleInTank; /**< TRUE while the nozzle is in the tank. */
    BOOL nozzleOnPump; /**< TRUE while the nozzle is on the pump. */
    /** Written on entry and attendant takeover; never read. */
    int unused;
    /** TRUE permits a charge for an unreturned nozzle. */
    BOOL chargeNozzlePenalty;
    /** TRUE makes timeouts deliver fuel and debit payment. */
    BOOL pumping;
};

/** @brief Positioned blood artwork in the severe-crash sequence. */
struct CrashOverlay {
    enum PictureId pictureId; /**< Picture resource to load. */
    int x;                    /**< Left destination pixel. */
    int y;                    /**< Top destination row. */
};

/**
 * @brief Rival lookahead left boundary and projected mirror outer verges.
 *
 * The boundary is in world coordinates; the arrays are screen pixels,
 * ordered from nearest to farthest mirror sample.
 */
struct RoadEdges {
    int lookaheadLeftX;       /**< Left road boundary four samples ahead. */
    int mirrorLeftOuterX[8];  /**< Mirror projection of the left outer verge. */
    int mirrorRightOuterX[8]; /**< Mirror right outer-verge X. */
};

/**
 * @brief Window and fuel-cap anchors for the displayed car.
 *
 * Anchors begin as car-relative pixels and become screen pixels when placed.
 * A negative front-window X marks invalid anchors; rear-window presence is
 * tracked separately from its translated coordinates.
 */
struct BodyPoints {
    BOOL hasRearGlass; /**< TRUE when the model has a rear-window anchor. */
    int frontGlassX;   /**< Front-window anchor X, or negative when invalid. */
    int frontGlassY;   /**< Front-window anchor Y. */
    int rearGlassX;    /**< Optional rear-window anchor X. */
    int rearGlassY;    /**< Optional rear-window anchor Y. */
    int gasCapX;       /**< Fuel-cap anchor X. */
    int gasCapY;       /**< Fuel-cap anchor Y. */
};

/** @brief Police substitution and pursuit during an ordinary road race. */
enum CopPhase {
    COP_ABSENT = 0, /**< No pending or active pursuit. */
    /** Await the scheduled sample; cancel if the rival has not fallen behind.
     */
    COP_WAITING = 1,
    /** Police replace the rival and close to within fifteen samples. */
    COP_APPROACHING = 2,
    /** Stop the player once police are more than two samples ahead. */
    COP_PURSUING = 3
};

#define STOP_LOST 0x01           /**< This car lost the race. */
#define STOP_COLLISION 0x02      /**< Collision with the rival. */
#define STOP_ENGINE 0x04         /**< Engine failure. */
#define STOP_TIRES 0x08          /**< Tire failure. */
#define STOP_TRANSMISSION 0x10   /**< Transmission failure. */
#define STOP_ROADSIDE_CRASH 0x20 /**< Collision with the roadside. */
#define STOP_POLICE 0x40         /**< Stopped by police. */
#define STOP_JUMP_START 0x80     /**< Moved before the start signal. */

/** @brief Rival or police speed targets, in miles per hour. */
struct RivalPace {
    int topSpeed;    /**< Stock speed used as the curve-speed base. */
    int speedMargin; /**< Added to player speed when the rival is nearby. */
    int fastSpeed;   /**< Faster target while well ahead. */
    int slowSpeed;   /**< Slower lead target and nearby minimum target. */
};

/** @brief Accepted gas-station commands for the current interaction state. */
struct GasInteraction {
    BOOL capAvailable;    /**< TRUE accepts a fuel-cap command. */
    BOOL ragAvailable;    /**< TRUE accepts taking or returning the rag. */
    BOOL nozzleAvailable; /**< TRUE accepts taking or returning the nozzle. */
};

/** @brief Rival or police position along and across the generated track. */
struct RivalPosition {
    int roadIndex;    /**< Current track-sample index. */
    int roadFraction; /**< Eighths within the current sample. */
    int distanceStep; /**< Eighths of a sample advanced per movement update. */
    int lateralX;     /**< Position across the road in world coordinates. */
    char unused[8];   /**< Spare bytes with no member accesses. */
};

extern struct BodyPoints g_featurePos;

extern int g_rivalLeft;
extern int g_brakeTicks;
extern struct RoadEdges g_edgesRoad;
extern int g_carHit;
extern struct Bitmap far *g_policeFramesTable[13];
extern int g_idxAtCop;
extern int g_finishMeanSpeedVal;
extern char g_failureTire;
extern int g_picOpponent;
extern int g_ramAllowed;
extern int g_leftEdgeX;
extern enum CopPhase g_pursuitPhaseIndex;
extern int g_steeringOld;
extern int g_coveredRacePct;
extern int g_shownShiftPosition;
extern int g_finishAheadPlayerState;
extern int g_swapArmedRivalValue;
extern struct Bitmap far *g_spritesOpp[13];
extern int g_carCollisionRange;
extern float g_raceSecsElapsedValue;
extern int g_edgeXR;
void ra_gas_station(void);
void ra_finish_stats(void);
enum RaceExit near ra_outcome(void);
void near ra_reset_eng(struct DrivingState *p);
void near ra_run(void);
int ra_speed_step(int rpm);
void ra_tick(void);

#endif
