/**
 * @file RACE.H
 * @brief Race simulation, outcomes and gas-station interactions.
 */

#ifndef RACE_H
#define RACE_H

#define REPAIR_BASE_PRICE 100
#define MAJOR_REPAIR_PRICE_RANGE 0x28
#define MINOR_REPAIR_PRICE_RANGE 0x14
#define TOTAL_LOSS_BASE_VALUE 0x13
#define JUNK_VALUE_FACTOR 0.4
#define WON_TIRE_WEAR_RANGE 500
#define ROAD_TIRE_WEAR_HIGH 700
#define DRAG_TIRE_WEAR_HIGH 300
#define ROAD_TIRE_WEAR_LOW 400
#define DRAG_TIRE_WEAR_LOW 200
#define RACE_TIRE_WEAR_RANGE 300
#define RACE_TICKS_PER_SECOND 6.068827333333334
#define RACE_INCOMPLETE_PCT_MAX 99
#define COP_RACE_CHANCE 4
#define COP_APPEAR_RANGE 255
#define COP_APPEAR_BASE 0x1fe

enum RaceExit {
    RACE_EXIT_GARAGE = 1,
    RACE_EXIT_STOPPED = 2,
    RACE_EXIT_BOBS = 3,
    RACE_EXIT_JAIL = 4
};

#define DRAG_FINISH_INDEX 0x17c
#define ROAD_FINISH_INDEX 0x3fc
#define RACE_START_INDEX 200
#define DRAG_RACE_FUEL 5
#define ROAD_RACE_FUEL 15
#define MIN_DRAG_FUEL 7
#define MIN_ROAD_FUEL 17
#define MIN_KING_FUEL 22
#define FUEL_TANK_CAPACITY 0xaa
#define FUEL_FILL_AMOUNT 0x1e
#define FUEL_FILL_COUNT_BASE 0xbe
#define NOZZLE_PENALTY 10
#define REPLACEMENT_TIRE_PRICE 0x32
#define TIRE_FAILURE_WEAR_MIN 8000
#define TIRE_FAILURE_WEAR_LIMIT 9500
#define TIRE_WEAR_BAND 2000
#define TOTAL_LOSS_MPH 0x5a
#define MAJOR_REPAIR_MPH 0x3c
#define MIN_REPAIR_MPH 0x1e
#define FINE_HIGH_MPH 0x50
#define FINE_MID_MPH 0x32
#define FINE_HIGH_PRICE 0x4b
#define FINE_MID_PRICE 0x32
#define FINE_LOW_PRICE 0x14
#define RIVAL_AUTO_SHIFT_RPM 0x14b4
#define RIVAL_TOP_GEAR_RPM 0x1518

struct Bitmap;
struct DrivingState;

enum GasHand {
    GAS_HAND_EMPTY = 1,
    GAS_HAND_NOZZLE = 2,
    GAS_HAND_RAG = 3
};

struct Dlg {
    int specIndex;
    int bodyMods;
    struct Bitmap far *bm1;
    struct Bitmap far *bm2;
    enum GasHand hand;
    int fillsLeft;
    int attendantAbsent;
    int nozzleInTank;
    int nozzleOnPump;
    int unused;
    int nozzlePenaltyDue;
    int pumping;
};

struct Spot {
    int id;
    int x;
    int y;
};

struct RoadEdges {
    int leftEdge;
    int leftOuterX[8];
    int rightOuterX[8];
};

struct BodyPoints {
    int hasRearGlass;
    int frontGlassX;
    int frontGlassY;
    int rearGlassX;
    int rearGlassY;
    int gasCapX;
    int gasCapY;
};

enum CopPhase {
    COP_ABSENT = 0,
    COP_WAITING = 1,
    COP_APPROACHING = 2,
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

struct RivalPace {
    int topSpeed;
    int speedMargin;
    int fastSpeed;
    int slowSpeed;
};

struct GasInteraction {
    int capAvailable;
    int ragAvailable;
    int nozzleAvailable;
};

struct RivalPosition {
    int roadIndex;
    int roadFraction;
    int distanceStep;
    int lateralX;
    char unused[8];
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
