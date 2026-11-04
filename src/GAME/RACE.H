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

extern struct BodyPoints g_featurePos; /* DS:8264 */

extern int g_gasSpotIds[];                              /* DS:53B6 */
extern int g_winMsgs[];                                 /* DS:53C8 */
extern struct Spot g_crashPics[];                       /* DS:53D4 */
extern int g_statsDone;                                 /* DS:53E6 */
extern int g_tireSlip[][5];                             /* DS:54B2 */
extern int g_lastTicks;                                 /* DS:54D0 */
extern int g_bumpAlternate;                                      /* DS:54D2 */
extern int g_rivalLeft;                                      /* DS:54D4 */
extern int g_rivalCurveBrake;                                      /* DS:54D6 */
extern int g_contactFrames;                                      /* DS:54D8 */
extern int g_brakeTicks;                                      /* DS:54DA */
extern struct State70C0 al70c0x;                        /* DS:70C0 */
extern int by72f2;                                      /* DS:72F2 */
extern struct Bitmap far *ab7526x[13];                  /* DS:7526 */
extern int g_idxAtCop;                                      /* DS:755A */
extern int g_finishMeanSpeedVal;                                      /* DS:7644 */
extern char g_failureTire;                                    /* DS:787C */
extern int ap7d7cv;                                     /* DS:7D7C */
extern int g_ramAllowed;                                      /* DS:7D7E */
extern int g_leftEdgeX;                                      /* DS:7D82 */
extern enum CopPhase a_7f88d;                                     /* DS:7F88 */
extern int g_steeringOld;                                      /* DS:82C2 */
extern int dz82c8x;                                     /* DS:82C8 */
extern int bz8630z;                                     /* DS:8630 */
extern int g_finishAheadPlayerState;                                      /* DS:8632 */
extern int g_swapArmedRivalValue;                                      /* DS:8648 */
extern struct Bitmap far *g8acew[13];                   /* DS:8ACE */
extern int ay8bc2z;                                     /* DS:8BC2 */
extern float g_raceSecsElapsedValue;                                    /* DS:8BC4 */
extern int g_edgeXR;                                     /* DS:8BD0 */
void ra_gas_station(void);                            /* 0x0B8A1 */
void ra_finish_stats(void);                           /* 0x0C407 */
int near ra_outcome(void);                            /* 0x0C613 */
void near ra_reset_eng(struct Eng *p);                /* 0x0CDAF */
void near ra_run(void);                               /* 0x0D031 */
int ra_speed_step(int rpm);                           /* 0x0D544 */
void ra_tick(void);                                   /* 0x0DA25 */

#endif
