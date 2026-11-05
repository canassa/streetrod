/**
 * @file GARAGE.H
 * @brief Garage services and car display state.
 */

#ifndef GARAGE_H
#define GARAGE_H

#include "GAME.H"
#include "KEYMAPS.H"

/** Action ticks charged for trying an incompatible transmission. */
#define TIME_TRANSMISSION_MISFIT 0x888

/**
 * @name Season clock
 * @brief Shared action/BIOS clock: twelve game hours per day, 91 days.
 * @{
 */
#define GAME_DAY_TICKS 6552
#define GAME_HOUR_TICKS 546
#define GAME_LAST_DAY 91
#define GAME_LAST_TICK 0x91908L
/** @} */

/**
 * @name Body service prices
 * @brief Dollar charges per operation; bumpers are charged individually.
 * @{
 */
#define PAINT_PRICE 20
#define STICKER_PRICE 5
#define ROOF_CHANGE_PRICE 70
#define BUMPER_CHANGE_PRICE 15
/** @} */

/** Dollar balance below which an empty inventory ends the game. */
#define BROKE_CASH_LIMIT 400L

/**
 * @name Ignition adjustment
 * @brief Inclusive settings -8..4, zero optimal; random setup uses all 13.
 * @{
 */
#define TIMING_MIN (-8)
#define TIMING_MAX 4
#define TIMING_SETTING_COUNT 13
/** @} */

/** @brief Ticks added to the game clock for player actions. */
enum ActionTimeCost {
    TIME_STICKER = 0x222,             /**< One game hour. */
    TIME_PAINT = 0x888,               /**< Four game hours. */
    TIME_ROOF_CHANGE = 0x2220,        /**< Sixteen game hours. */
    TIME_BUMPER_CHANGE = 0x888,       /**< Four hours per bumper. */
    TIME_TIRE_CHANGE = 0x444,         /**< Two game hours. */
    TIME_TRANSMISSION_CHANGE = 0x666, /**< Three hours to install or remove. */
    TIME_ENGINE_CHANGE = 0x666,       /**< Three hours to install or remove. */
    TIME_ENGINE_MISFIT = 0x888,       /**< Four hours for an incompatible engine. */
    /** One and a half hours to install or remove. */
    TIME_MANIFOLD_CHANGE = 0x333,
    /** Two hours for an incompatible manifold. */
    TIME_MANIFOLD_MISFIT = 0x444,
    TIME_CARB_CHANGE = 0x199, /**< About three quarters of an hour per carb. */
    TIME_CARB_MISFIT = 0x222, /**< One hour for an incompatible carb. */
    TIME_WIRE_CHANGE = 0x6d,  /**< About twelve minutes per connection toggle. */
    /** About six minutes per engine-area bolt operation. */
    TIME_BOLT_WORK = 0x36,
    /** Two-hour cap on tuning time charged at triple elapsed ticks. */
    TIME_TUNING_MAX = 0x444,
    TIME_PART_PURCHASE = 0x222, /**< One hour per purchased part. */
    TIME_CAR_PURCHASE = 0x444,  /**< Two hours per purchased car. */
    TIME_PART_SALE = 0x444,     /**< Two hours per sold part. */
    TIME_CAR_SALE = 0x888,      /**< Four hours per sold car. */
    TIME_READ_PAPER = 0x222,    /**< One hour after browsing the paper. */
    TIME_TOWN_TRIP = 0x222,     /**< One hour per travel leg. */
    TIME_DRAG_RACE = 0x444,     /**< Two hours after a negotiated drag race. */
    /** Three and a half hours after a negotiated road race. */
    TIME_ROAD_RACE = 0x777,
    TIME_RIVAL_HOOD = 0x111 /**< Half an hour per rival-engine inspection. */
};

/** @brief Body-colour pairs used by owned cars and rivals. */
enum PaintScheme {
    PAINT_UNASSIGNED = -1, /**< No scheme assigned yet. */
    PAINT_RED = 0,         /**< Red. */
    PAINT_CYAN = 1,        /**< Cyan. */
    PAINT_GREEN = 2,       /**< Green. */
    PAINT_MAGENTA = 3,     /**< Magenta. */
    PAINT_BLUE = 4,        /**< Blue. */
    PAINT_WHITE = 5,       /**< Light gray and bright white. */
    PAINT_SCHEME_COUNT = 6 /**< Number of selectable schemes. */
};

/** Number of schemes available in advert previews and purchases. */
#define PAINT_NONWHITE_COUNT PAINT_WHITE

/** @brief Colours within one body-paint pair. */
enum PaintShade {
    PAINT_BASE = 0,       /**< Base body colour. */
    PAINT_HIGHLIGHT = 1,  /**< Bright body colour. */
    PAINT_SHADE_COUNT = 2 /**< Colours in each scheme. */
};

struct Car;
struct PartNode;

/** @brief Equipment categories in the parts catalog. */
enum PartCategory {
    PART_ENGINE = 0,       /**< Engine. */
    PART_TRANSMISSION = 1, /**< Transmission. */
    PART_CARBURETOR = 2,   /**< Carburetor. */
    PART_MANIFOLD = 3,     /**< Intake manifold. */
    PART_TIRES = 4         /**< Tires. */
};

/** @brief Packed sticker and body-modification cache for the garage display. */
struct Look {
    unsigned unusedLow : 8; /**< Unused low byte. */
    /** One-based sticker choice, or zero for none. */
    unsigned sticker : 5;
    unsigned choppedRoof : 1; /**< Cached @ref APPEARANCE_ROOF flag. */
    /** Cached @ref APPEARANCE_REAR_BUMPER flag. */
    unsigned rearBumperRemoved : 1;
    /** Cached @ref APPEARANCE_FRONT_BUMPER flag. */
    unsigned frontBumperRemoved : 1;
};

/** @brief Transient car artwork, sticker placement and last wager input. */
struct CarDisplay {
    /** Aligned car-local sticker X; screen X after presenting the car. */
    int stickerX;
    /** Car-local sticker Y; screen Y after presenting the car. */
    int stickerY;
    /** Sticker X remainder within the video alignment unit. */
    int stickerSubX;
    /** Last wager-menu result, including cancel. */
    enum InputResult wagerKey;
    /** Cached body picture ID; currently not read. */
    int bodyPic;
    enum PaintScheme paintScheme; /**< Displayed car body colours. */
    enum TireVariant tireVariant; /**< Tire artwork variant. */
    /** Occupant portrait picture; ignored when unoccupied. */
    int portraitPic;
    /** Current garage car's specification index. */
    int specIndex;
    struct Look appearance; /**< Cached garage sticker and body flags. */
    /** Transmission-dialog picture, or zero if absent. */
    int transmissionPic;
    char unused[8]; /**< Unused display-state bytes. */
};

/** Garage menu result directing the game loop to the ending screen. */
#define GARAGE_GAME_ENDED 0x29a

/** @brief How to present the current car when entering the garage menu. */
enum GarageMode {
    GARAGE_KEEP = 0, /**< Reuse the existing garage display. */
    /** Load the garage and animate the current car arriving. */
    GARAGE_DRIVE_IN = 1,
    GARAGE_REDRAW = 2, /**< Load the garage and draw a stationary car. */
    /** Animate the previous car leaving before bringing in its replacement. */
    GARAGE_SWAP = 3
};

extern int g_tirePics[][4];

/** Body-colour pairs indexed by @ref PaintScheme and @ref PaintShade. */
extern char g_paint[PAINT_SCHEME_COUNT][PAINT_SHADE_COUNT];
extern int g_nCarb[];
extern int g_nBolt[];
extern struct CarDisplay g_carDraw;
int ga_day(void);
void ga_time(int *a, int *b);
void ga_check_end(int n);
void ga_calendar(int a);
void ga_date(int x);

/**
 * @brief Apply the player's body colours on EGA and Tandy displays.
 *
 * @param scheme Body-colour pair to display.
 */
void ga_palette(enum PaintScheme scheme);
int ga_hotspot(int n);
void ga_clear_name(void);
int near ga_menu(enum GarageMode mode);

/**
 * @brief Choose and buy new paint for the current car, removing its sticker.
 */
void near ga_paint(void);
void near ga_body(unsigned mask);
void near ga_stickers(void);
void near ga_spares(enum PartCategory t, struct PartNode **list, char *buf, int start);
void near ga_install(struct PartNode *n, struct PartNode **slot);
struct PartNode *near ga_unlink(struct PartNode *n);
void near ga_tires(void);
void ga_info(void);
void near ga_transmission(void);
int ga_ready(struct Car *car, int flag);
void near ga_hood(struct Car *car);

#endif
