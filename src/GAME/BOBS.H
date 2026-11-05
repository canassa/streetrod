/* BOBS.H - declarations of what BOBS.C defines. */

#ifndef BOBS_H
#define BOBS_H

#define KING_NOVICE_CHANCE 5
#define KING_NEED_WINS_CHANCE 8
#define KING_UNREADY_CHANCE 12
#define KING_MET_CHANCE 10
#define KING_FIRST_CHANCE 50
#define CARHOP_MIN_PASSES 3
#define CARHOP_ORDER_BONUS 75
#define CARHOP_JAIL_BONUS 10
#define CARHOP_BASE_CHANCE 5
#define CARHOP_QUICK_LEAVE_CHANCE 10
#define CARHOP_BUSY_LEAVE_CHANCE 20
#define CARHOP_LEAVE_CHANCE 40
#define CARHOP_EMPTY_LEAVE_CHANCE 8
#define BOBS_CELL_CHANCE 25
#define JAIL_CELL_CHANCE 6
#define BET_STICKER_LOW_BONUS 4
#define BET_STICKER_HIGH_BONUS 6
#define BET_DEMO_BONUS 10
#define BET_STICKER_LOW_BIT 0x100
#define RIVAL_POOL_DISTANCE_INIT 0x7d00

#define RIVAL_HOOD_TIMEOUT_TICKS 800

#define CAR_CLASS_LOW_LIMIT 3
#define BET_CLASS_HIGH_RANK 7
#define RACER_GROUP_HIGH_RANK 6
#define RIVAL_POOL_LIMIT 9
#define RIVAL_POOL_REPLACE_ABOVE 6
#define RIVAL_RANK_DIFFERENCE 1
#define SPEC_ORDINARY_COUNT 25
#define SPEC_POLICE 25
#define RIVAL_STICKER_CHANCE 20
#define RIVAL_RESTICKER_CHANCE 40
#define RIVAL_BUMPER_CHANCE 25
#define RIVAL_ROOF_CHANCE 30
#define RIVAL_STICKER_RACES 5
#define RIVAL_RESTICKER_RACES 10
#define RIVAL_BODY_RACES 8
#define STICKER_CHOICE_COUNT 9

enum RatingRelation {
    RATING_TOO_STRONG = 0,
    RATING_WITHIN_TOLERANCE = 1,
    RATING_RIVAL_STRONGER = 2
};

enum AcceptanceStake {
    ACCEPT_FREE = 0,
    ACCEPT_SMALL_CASH = 1,
    ACCEPT_LARGE_CASH = 2,
    ACCEPT_CAR = 3
};

enum StakeClass {
    STAKE_FREE = 0,
    STAKE_CASH = 1,
    STAKE_CAR = 2
};

enum CarClass {
    CAR_CLASS_LOW = 0,
    CAR_CLASS_MID = 1,
    CAR_CLASS_HIGH = 2
};

enum WagerAmount {
    WAGER_10_DOLLARS = 10,
    WAGER_25_DOLLARS = 25,
    WAGER_50_DOLLARS = 50,
    WAGER_100_DOLLARS = 100
};

#define KING_NOVICE_WINS 3
#define KING_TOTAL_WINS 7
#define KING_WINS_PER_TYPE 6
#define WAGER_TIMEOUT_TICKS 800
#define WAGER_TRIES 2
#define DRAG_LOSS_KING_PENALTY 3
#define ROAD_LOSS_KING_PENALTY 2

#include "CURSOR.H"
struct Bitmap;
struct Spec;

struct Racer {
    signed char betToleranceClass;
    char shiftPenalty;
    signed char drivingPenalty;
    char unused1;
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
    char unused2;
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

enum CarhopState {
    CARHOP_DISABLED = -1,
    CARHOP_ABSENT = 0,
    CARHOP_PRESENT = 1
};

enum RivalArrival {
    RIVAL_REQUESTED = 0,
    RIVAL_PASS_BY = 1,
    RIVAL_CHALLENGE = 2
};

enum KingStatus {
    KING_READY = 0,
    KING_NOVICE = 1,
    KING_NEED_WINS = 2,
    KING_NEED_WIN_RATE = 3,
    KING_NEED_DRAG_WINS = 4,
    KING_NEED_ROAD_WINS = 5
};


/**
 * @fn static void near animate_rival_car(enum CarAnimKind animation, int specIndex, int showPortrait, int carBottom, int useGasMask, int *tirePics)
 *
 * Animate the selected rival's car and maintain its banner and presence.
 *
 * Fits the rival's name and car description beside the cash display, dropping
 * final words when needed. Updates carhop activity around the movement. A
 * pending pass-by drives the car out again and clears its banner and presence.
 * The selected rival, car specification, portrait and paint must already be set
 * up, along with the scene's graphics and car backdrop state.
 *
 * @param[in] animation Movement kind: 4 for the first arrival in a scene, 1 for
 * subsequent arrivals, or 2 for departure. The first arrival initializes the
 * backdrop and enables carhop activity unless the rival passes straight by.
 * @param[in] specIndex Car specification index, 0 through 25, matching the
 * selected rival and its selected specification.
 * @param[in] showPortrait Zero to omit the occupant portrait, nonzero to include
 * the selected portrait. All callers include it.
 * @param[in] carBottom Inclusive bottom screen row of the car image, in pixels;
 * all callers use 163.
 * @param[in] useGasMask Nonzero to apply the gas-station foreground mask. All
 * callers use zero for the view of Bob's, including the view from jail.
 * @param[in] tirePics Borrowed array of four tire-frame picture IDs, read during
 * this call; a null pointer omits tires.
 */

#endif
