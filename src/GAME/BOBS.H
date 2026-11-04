/* BOBS.H - declarations of what BOBS.C defines. */

#ifndef BOBS_H
#define BOBS_H
#include "STRUCTS.H"

extern struct Wager g_raceWagerTerms;

#define NUM_RACERS 22
#define RACER_KING 21
#define RACER_COP_ENTRY 10
#define RACERS_PER_GROUP 7

enum RacerStatus {
    RACER_EMPTY = -1,
    RACER_NOT_MET = 0,
    RACER_MET = 1
};

extern struct Racer g_racerRecordsArray[];
extern int g_bannerX;
extern int g_passCount;
extern int g_lastRival;
extern int g_refuseMsgs[][5];
extern int g_acceptMsgs[][3];
extern struct Step g_animSteps[];
extern char g_animCells[16];
extern struct Cell g_endCellsA[20];
extern struct Cell g_endCellsB[9];
extern int g_endPics[];
extern int g_endPics2[];
extern struct Btn g_jailBars[7];
extern struct Pair g_jailGaps[];
extern struct Bitmap far *g_barJailPixels;
extern int g_opponentSpecIndex;
extern int g_resultWin;
extern int g_isKingState;
extern struct Spec *g_oppSpecPointer;
extern struct Racer *g_selectedRacerPointer;
extern int g_driverIdxSelVal;
extern int g_rivalHereState;
extern struct Bitmap far *g_edgeBmpJail;
void bo_visit(void);
void bo_jail(void);

#endif
