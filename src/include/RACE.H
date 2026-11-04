/* RACE.H - declarations of what RACE.C defines. */

#ifndef RACE_H
#define RACE_H
#include "STRUCTS.H"

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
extern int a_7f88d;                                     /* DS:7F88 */
extern int g_hasGlassRear;                                      /* DS:8264 */
extern int g_backGlassXState;                                      /* DS:826A */
extern int g_winRearPointY;                                     /* DS:826C */
extern int g_gasCapXVal;                                      /* DS:826E */
extern int g_gasCapYValue;                                      /* DS:8270 */
extern int g_steeringOld;                                      /* DS:82C2 */
extern int dz82c8x;                                     /* DS:82C8 */
extern int bz8630z;                                     /* DS:8630 */
extern int g_finishAheadPlayerState;                                      /* DS:8632 */
extern int g_swapArmedRivalValue;                                      /* DS:8648 */
extern struct Bitmap far *g8acew[13];                   /* DS:8ACE */
extern int ay8bc2z;                                     /* DS:8BC2 */
extern float g_raceSecsElapsedValue;                                    /* DS:8BC4 */
extern int g_edgeXR;                                     /* DS:8BD0 */
void race_gas_station(void);                            /* 0x0B8A1 */
void race_finish_stats(void);                           /* 0x0C407 */
int near race_outcome(void);                            /* 0x0C613 */
void near race_reset_eng(struct Eng *p);                /* 0x0CDAF */
void near race_run(void);                               /* 0x0D031 */
int race_speed_step(int rpm);                           /* 0x0D544 */
void race_tick(void);                                   /* 0x0DA25 */

#endif
