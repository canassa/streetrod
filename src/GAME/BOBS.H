/* BOBS.H - declarations of what BOBS.C defines. */

#ifndef BOBS_H
#define BOBS_H

#include "CURSOR.H"
struct Bitmap;
struct Spec;

struct Racer {
    signed char f0;
    char shiftPenalty;
    signed char f2;
    char f3;
    int overlayPic;
    int nameOffset;
    int specIndex;
    signed char paintScheme;
    char status;
    signed char betRaces;
    char betWins;
    char bodyMods;
    signed char sticker;
    char lostCarOnce;
    char pad11;
};

struct Btn {
    struct Bitmap far *near *src;
    int sx;
    int sy;
    int w;
    int h;
    int x;
    int y;
};

/* one step of an animation table (g_animSteps): frame number, then the delay in ticks before the next */
struct Step {
    unsigned char frame;
    unsigned char delay;
};

/* one candidate of build_rival_pool: rating difference to the current car, an "already taken" flag, kind difference */
struct Cand {
    int dr;
    int taken;
    int dk;
};

/* one cell of the intro picture tables g_endCellsA / g_endCellsB (6 bytes): source origin in the sheet,
   size, and the on-screen position (x is biased by 0x50) */
struct Cell {
    unsigned char sx;
    unsigned char sy;
    unsigned char w;
    unsigned char h;
    unsigned char x;
    unsigned char y;
};

enum BetKind {
    BET_NONE,
    BET_10,
    BET_50,
    BET_25,
    BET_100,
    BET_CAR
};

struct Wager {
    enum BetKind kind;
    int amount;
};

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
