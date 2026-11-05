/**
 * @file GARAGE.H
 * @brief Garage services and car display state.
 */

#ifndef GARAGE_H
#define GARAGE_H

#define TIME_TRANSMISSION_MISFIT 0x888

#define GAME_DAY_TICKS 6552
#define GAME_HOUR_TICKS 546
#define GAME_LAST_DAY 91
#define GAME_LAST_TICK 0x91908L
#define PAINT_PRICE 20
#define STICKER_PRICE 5
#define ROOF_CHANGE_PRICE 70
#define BUMPER_CHANGE_PRICE 15
#define BROKE_CASH_LIMIT 400L
#define TIMING_MIN (-8)
#define TIMING_MAX 4
#define TIMING_SETTING_COUNT 13

enum ActionTimeCost {
    TIME_STICKER = 0x222,
    TIME_PAINT = 0x888,
    TIME_ROOF_CHANGE = 0x2220,
    TIME_BUMPER_CHANGE = 0x888,
    TIME_TIRE_CHANGE = 0x444,
    TIME_TRANSMISSION_CHANGE = 0x666,
    TIME_ENGINE_CHANGE = 0x666,
    TIME_ENGINE_MISFIT = 0x888,
    TIME_MANIFOLD_CHANGE = 0x333,
    TIME_MANIFOLD_MISFIT = 0x444,
    TIME_CARB_CHANGE = 0x199,
    TIME_CARB_MISFIT = 0x222,
    TIME_WIRE_CHANGE = 0x6d,
    TIME_BOLT_WORK = 0x36,
    TIME_TUNING_MAX = 0x444,
    TIME_PART_PURCHASE = 0x222,
    TIME_CAR_PURCHASE = 0x444,
    TIME_PART_SALE = 0x444,
    TIME_CAR_SALE = 0x888,
    TIME_READ_PAPER = 0x222,
    TIME_TOWN_TRIP = 0x222,
    TIME_DRAG_RACE = 0x444,
    TIME_ROAD_RACE = 0x777,
    TIME_RIVAL_HOOD = 0x111
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
struct Node;

enum PartCategory {
    PART_ENGINE = 0,
    PART_TRANSMISSION = 1,
    PART_CARBURETOR = 2,
    PART_MANIFOLD = 3,
    PART_TIRES = 4
};

struct Look {
    unsigned lo : 8;
    unsigned sticker : 5;
    unsigned roof : 1;
    unsigned front : 1;
    unsigned rear : 1;
};

struct CarDisplay {
    int stickerX;
    int stickerY;
    int stickerSubX;
    int betType;
    int bodyPic;
    enum PaintScheme paintScheme; /**< Displayed car body colours. */
    int tireVariant;
    int overlayPic;
    int specIndex;
    struct Look appearance;
    int transmissionPic;
    char unused[8];
};

#define GARAGE_GAME_ENDED 0x29a

enum GarageMode {
    GARAGE_KEEP = 0,
    GARAGE_DRIVE_IN = 1,
    GARAGE_REDRAW = 2,
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
void near ga_spares(enum PartCategory t, struct Node **list, char *buf, int start);
void near ga_install(struct Node *n, struct Node **slot);
struct Node *near ga_unlink(struct Node *n);
void near ga_tires(void);
void ga_info(void);
void near ga_transmission(void);
int ga_ready(struct Car *car, int flag);
void near ga_hood(struct Car *car);

#endif
