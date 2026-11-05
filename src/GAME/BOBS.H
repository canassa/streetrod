/**
 * @file BOBS.H
 * @brief Rival encounters at Bob's and the jail.
 */

#ifndef BOBS_H
#define BOBS_H

/**
 * @name King encounter odds
 * @brief Percent chances by eligibility and previous contact.
 * @{
 */
#define KING_NOVICE_CHANCE 5
#define KING_NEED_WINS_CHANCE 8
#define KING_UNREADY_CHANCE 12
#define KING_MET_CHANCE 10
#define KING_FIRST_CHANCE 50
/** @} */
/** Departing cars required before the carhop can arrive. */
#define CARHOP_MIN_PASSES 3
/**
 * @name Carhop arrival odds
 * @brief Arrival percentage points for orders, jail visits and base odds.
 * @{
 */
#define CARHOP_ORDER_BONUS 75
#define CARHOP_JAIL_BONUS 10
#define CARHOP_BASE_CHANCE 5
/** @} */
/**
 * @name Carhop departure odds
 * @brief Percent departure chances at successive car-movement stages.
 * @{
 */
/** Percent chance of leaving immediately after arrival. */
#define CARHOP_QUICK_LEAVE_CHANCE 10
/** Percent chance of departure before an arriving car moves. */
#define CARHOP_BUSY_LEAVE_CHANCE 20
/** Percent chance of departure after a car finishes moving. */
#define CARHOP_LEAVE_CHANCE 40
/** Percent chance of leaving immediately when no rival remains. */
#define CARHOP_EMPTY_LEAVE_CHANCE 8
/** @} */
/**
 * @name Jail animation odds
 * @brief Percent chance on entering Bob's or on a jail-visit timeout.
 * @{
 */
#define BOBS_CELL_CHANCE 25
#define JAIL_CELL_CHANCE 6
/** @} */
/**
 * @name Wager acceptance bonuses
 * @brief Acceptance percentage points for stickers and demo play.
 * @{
 */
#define BET_STICKER_LOW_BONUS 4
#define BET_STICKER_HIGH_BONUS 6
#define BET_DEMO_BONUS 10
/** @} */
/**
 * Low bit of the packed sticker choice; selects the smaller acceptance bonus.
 */
#define BET_STICKER_LOW_BIT 0x100
/**
 * Initial upper bound when seeking the smallest top-speed difference, in mph.
 */
#define RIVAL_POOL_DISTANCE_INIT 0x7d00

/** Input timeout while viewing a rival engine, in timer ticks. */
#define RIVAL_HOOD_TIMEOUT_TICKS 800

/** Model ranks below this use the lowest wager and assignment class. */
#define CAR_CLASS_LOW_LIMIT 3
/** Model ranks at or above this use the highest wager class. */
#define BET_CLASS_HIGH_RANK 7
/**
 * Model ranks at or above this enter the highest initial rival-assignment
 * group.
 */
#define RACER_GROUP_HIGH_RANK 6
/**
 * @name Ordinary encounter pool
 * @brief Closest top speeds, filtered by model-rank difference.
 * @{
 */
#define RIVAL_POOL_LIMIT 9
/** Beyond this pool position, replace excessive rank gaps with early picks. */
#define RIVAL_POOL_REPLACE_ABOVE 6
#define RIVAL_RANK_DIFFERENCE 1
/** @} */
/**
 * @name Rival car specifications
 * @brief Ordinary indices 0..24; King and police use index 25.
 * @{
 */
#define SPEC_ORDINARY_COUNT 25
#define SPEC_KING 25
/** @} */
/**
 * @name Rival modification draws
 * @brief Exclusive draw bounds compared to (model rank / 2) + 1.
 * @{
 */
#define RIVAL_STICKER_CHANCE 20
#define RIVAL_RESTICKER_CHANCE 40
#define RIVAL_BUMPER_CHANCE 25
#define RIVAL_ROOF_CHANCE 30
/** @} */
/**
 * @name Rival modification eligibility
 * @brief Wagered-race counts must exceed these change thresholds.
 * @{
 */
#define RIVAL_STICKER_RACES 5
#define RIVAL_RESTICKER_RACES 10
#define RIVAL_BODY_RACES 8
/** @} */
/** Exclusive random choice bound, including zero for no sticker. */
#define STICKER_CHOICE_COUNT 9

/** @brief Player rating relative to the rival, for wager acceptance. */
enum RatingRelation {
    /** Player exceeds the rival rating plus stake tolerance. */
    RATING_TOO_STRONG = 0,
    /** Player is at least as strong, within stake tolerance. */
    RATING_WITHIN_TOLERANCE = 1,
    RATING_RIVAL_STRONGER = 2 /**< Player rating is below the rival rating. */
};

/** @brief Rows of the wager acceptance probability table. */
enum AcceptanceStake {
    ACCEPT_FREE = 0,       /**< No money or car at stake. */
    ACCEPT_SMALL_CASH = 1, /**< Ten-dollar drag or fifty-dollar road wager. */
    /** Twenty-five-dollar drag or hundred-dollar road wager. */
    ACCEPT_LARGE_CASH = 2,
    ACCEPT_CAR = 3 /**< Ownership of the losing car changes hands. */
};

/** @brief Columns selecting rival rating tolerance by stake. */
enum StakeClass {
    STAKE_FREE = 0, /**< Unwagered race tolerance. */
    STAKE_CASH = 1, /**< Money-wager tolerance, independent of amount. */
    STAKE_CAR = 2   /**< Pink-slip tolerance. */
};

/** @brief Model-rank bands used with engine type to rate wager strength. */
enum CarClass {
    CAR_CLASS_LOW = 0, /**< Model rank below three. */
    CAR_CLASS_MID = 1, /**< Model ranks three through six. */
    CAR_CLASS_HIGH = 2 /**< Model rank seven or eight. */
};

/** @brief Dollar stakes offered during race negotiation. */
enum WagerAmount {
    WAGER_10_DOLLARS = 10,  /**< Smaller drag stake. */
    WAGER_25_DOLLARS = 25,  /**< Larger drag stake. */
    WAGER_50_DOLLARS = 50,  /**< Smaller road stake. */
    WAGER_100_DOLLARS = 100 /**< Larger road stake. */
};

/**
 * @name King eligibility thresholds
 * @brief Wagered wins required before considering win rate.
 * @{
 */
#define KING_NOVICE_WINS 3
#define KING_TOTAL_WINS 7
#define KING_WINS_PER_TYPE 6
/** @} */
/** Timer ticks allowed for each race-type or stake selection. */
#define WAGER_TIMEOUT_TICKS 800
/** Maximum stake proposals before negotiation ends. */
#define WAGER_TRIES 2
/**
 * @name King defeat penalties
 * @brief Wins deducted after losing either race against the King.
 * @{
 */
#define DRAG_LOSS_KING_PENALTY 3
#define ROAD_LOSS_KING_PENALTY 2
/** @} */

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

/** @brief Negotiated stake used for acceptance and race settlement. */
enum BetKind {
    BET_NONE, /**< Race for kicks; excludes ordinary win-history updates. */
    BET_10,   /**< Ten-dollar drag wager. */
    BET_50,   /**< Fifty-dollar road wager. */
    BET_25,   /**< Twenty-five-dollar drag wager. */
    BET_100,  /**< Hundred-dollar road wager. */
    BET_CAR   /**< Winner takes the losing car. */
};

struct Wager {
    enum BetKind kind;
    int amount;
};

extern struct Wager g_raceWagerTerms;

/**
 * @name Racer table layout
 * @brief Three seven-driver groups, then the King; police borrow entry 10.
 * @{
 */
#define NUM_RACERS 22
#define RACER_KING 21
#define RACER_COP_ENTRY 10
#define RACERS_PER_GROUP 7
/** @} */

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

/** @brief Carhop availability during rival movement. */
enum CarhopState {
    /** Suppress arrival until the player has pulled in. */
    CARHOP_DISABLED = -1,
    /** Arrival may be attempted after enough departing cars. */
    CARHOP_ABSENT = 0,
    /** Order hotspot hidden while she stands beside the cars. */
    CARHOP_PRESENT = 1
};

/** @brief Reason a rival enters the encounter scene. */
enum RivalArrival {
    RIVAL_REQUESTED = 0, /**< Called from the met-rivals list. */
    RIVAL_PASS_BY = 1,   /**< Drive through without stopping, as at jail. */
    /** Unsolicited visit; a novice sees the King pass by. */
    RIVAL_CHALLENGE = 2
};

/** @brief Eligibility result selecting the King's response to a challenge. */
enum KingStatus {
    /** May race, including when all ordinary rivals are gone. */
    KING_READY = 0,
    KING_NOVICE = 1, /**< Fewer than three counted wins; the King passes by. */
    /** Fewer than seven wins and fewer than six in either discipline. */
    KING_NEED_WINS = 2,
    /** Needs more wins overall or a win rate of at least half. */
    KING_NEED_WIN_RATE = 3,
    KING_NEED_DRAG_WINS = 4, /**< Fewer than six counted drag wins. */
    KING_NEED_ROAD_WINS = 5  /**< Fewer than six counted road wins. */
};

#endif
