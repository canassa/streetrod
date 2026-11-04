/* RACE.H - declarations of what RACE.C defines. */

#ifndef RACE_H
#define RACE_H
#include "STRUCTS.H"

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
extern struct State70C0 al70c0x;
extern int by72f2;
extern struct Bitmap far *ab7526x[13];
extern int g_idxAtCop;
extern int g_finishMeanSpeedVal;
extern char g_failureTire;
extern int ap7d7cv;
extern int g_ramAllowed;
extern int g_leftEdgeX;
extern int a_7f88d;
extern int g_steeringOld;
extern int dz82c8x;
extern int bz8630z;
extern int g_finishAheadPlayerState;
extern int g_swapArmedRivalValue;
extern struct Bitmap far *g8acew[13];
extern int ay8bc2z;
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
