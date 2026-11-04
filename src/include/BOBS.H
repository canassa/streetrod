/* BOBS.H - declarations of what BOBS.C defines. */

#ifndef BOBS_H
#define BOBS_H
#include "STRUCTS.H"

#define NUM_RACERS 22
#define RACER_KING 21
#define RACER_COP_ENTRY 10
#define RACERS_PER_GROUP 7

enum RacerStatus {
    RACER_EMPTY = -1,
    RACER_NOT_MET = 0,
    RACER_MET = 1
};

extern struct Racer g_racerRecordsArray[];                           /* DS:7FF8 */
extern int g_bannerX;                                   /* DS:51EE */
extern int g_passCount;                                 /* DS:51F0 */
extern int g_lastRival;                                 /* DS:51F8 */
extern int g_refuseMsgs[][5];                           /* DS:51FC */
extern int g_acceptMsgs[][3];                           /* DS:5210 */
extern struct Step g_animSteps[];                       /* DS:521E */
extern char g_animCells[16];                            /* DS:525A */
extern struct Cell g_endCellsA[20];                     /* DS:526A */
extern struct Cell g_endCellsB[9];                      /* DS:52E2 */
extern int g_endPics[];                                 /* DS:5318 */
extern int g_endPics2[];                                /* DS:5328 */
extern struct Btn g_jailBars[7];                        /* DS:5338 */
extern struct Pair g_jailGaps[];                        /* DS:539A */
extern struct Bitmap far *g_barJailPixels;                       /* DS:6E64 */
extern int bn7648;                                      /* DS:7648 */
extern int g_resultWin;                                      /* DS:7676 */
extern int g_classBetValue;                                     /* DS:79A2 */
extern int d_79a4;                                      /* DS:79A4 */
extern int g_isKingState;                                      /* DS:7D40 */
extern struct Spec *g_oppSpecPointer;                            /* DS:7D7A */
extern struct Racer *g_selectedRacerPointer;                            /* DS:7D84 */
extern int g_driverIdxSelVal;                                      /* DS:7F82 */
extern int g_rivalHereState;                                      /* DS:7F86 */
extern struct Bitmap far *g_edgeBmpJail;                       /* DS:82B8 */
void bobs_visit(void);                                  /* 0x0AB8B */
void bobs_jail(void);                                   /* 0x0B08C */

#endif
