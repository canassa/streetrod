/* RACE.H - declarations of what RACE.C defines. */

#ifndef RACE_H
#define RACE_H
#include "STRUCTS.H"

enum CopPhase {
    COP_ABSENT = 0,
    COP_WAITING = 1,
    COP_APPROACHING = 2,
    COP_PURSUING = 3
};

#define STOP_LOST 0x01
#define STOP_COLLISION 0x02
#define STOP_ENGINE 0x04
#define STOP_TIRES 0x08
#define STOP_TRANSMISSION 0x10
#define STOP_ROADSIDE_CRASH 0x20
#define STOP_POLICE 0x40
#define STOP_JUMP_START 0x80

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

extern struct BodyPoints g_featurePos;

extern int g_gasSpotIds[];
extern int g_winMsgs[];
extern struct Spot g_crashPics[];
extern int g_statsDone;
extern int g_tireSlip[][5];
extern int g_lastTicks;
extern int g_bumpAlternate;
extern int g_rivalLeft;
extern int g_rivalCurveBrake;
extern int g_contactFrames;
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
int near ra_outcome(void);
void near ra_reset_eng(struct Eng *p);
void near ra_run(void);
int ra_speed_step(int rpm);
void ra_tick(void);

#endif
