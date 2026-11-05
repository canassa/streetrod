/**
 * @file BOBS.H
 * @brief Rival encounters at Bob's and the jail.
 */

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
#define SPEC_KING 25
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
struct CarSpec;

/**
 * @brief A rival driver's behavior, assigned car and encounter history.
 *
 * Each 18-byte record is loaded from the game data and saved intact. Ordinary
 * rivals receive a random car; the King keeps his assigned car.
 */
struct Racer {
    /** Row 0..2 of the stake-tolerance table; higher rows tolerate less. */
    signed char stakeToleranceRow;
    char earlyShiftPenalty; /**< Manual upshift RPM deduction, in hundreds. */
    /** Bend-speed deduction, halved when behind; also reduces ramming odds. */
    signed char drivingPenalty;
    char unused1;            /**< Unused byte preserved by load/save. */
    int portraitPic;         /**< In-car driver portrait picture ID. */
    int nameOffset;          /**< Name byte offset in description text. */
    int specIndex;           /**< Car spec index; -1 before assignment. */
    signed char paintScheme; /**< Body-colour pair; @ref PaintScheme. */
    char status;             /**< Availability byte; @ref RacerStatus. */
    signed char betRaces;    /**< Wagered races; reset with a new car. */
    char rivalBetWins;       /**< Rival wins in counted wagered races. */
    char bodyMods;           /**< Body overlay bits, @ref BodyMod. */
    signed char sticker;     /**< Sticker choice 1..8, or 0 for none. */
    char lostCarOnce;        /**< Nonzero after receiving a replacement car. */
    char unused2;            /**< Unused byte preserved by load/save. */
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

/**
 * @brief Rival availability for encounters and the call-rival menu.
 */
enum RacerStatus {
    RACER_EMPTY = -1,  /**< Eliminated from the ordinary rival pool. */
    RACER_NOT_MET = 0, /**< Not callable; the King may still be ineligible. */
    RACER_MET = 1      /**< Callable; the King is eligible to race. */
};

extern struct Racer g_racerRecordsArray[];
extern int g_bannerX;
extern struct Btn g_jailBars[7];
extern struct Pair g_jailGaps[];
extern struct Bitmap far *g_barJailPixels;
extern int g_opponentSpecIndex;
extern int g_resultWin;
extern int g_isKingState;
extern struct CarSpec *g_oppSpecPointer;
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

#endif
