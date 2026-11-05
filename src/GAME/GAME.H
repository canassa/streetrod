/**
 * @file GAME.H
 * @brief The garage game loop, the player's cars and parts.
 */

#ifndef GAME_H
#define GAME_H

/**
 * @name Assembly storage
 * @brief Rack and attachment counts for an engine, manifold and three carbs.
 * @{
 */
#define ENGINE_RACK_SLOTS 5
#define ENGINE_BOLT_COUNT 9
#define TRANSMISSION_BOLT_COUNT 2
/** @} */

/** @brief Assembly-bolt tightness, from missing to fully tightened. */
enum BoltTightness {
    BOLT_ABSENT = 0,       /**< Bolt removed. */
    BOLT_INSERTED = 1,     /**< Inserted, without tightening turns. */
    BOLT_PARTLY_TIGHT = 2, /**< One tightening turn. */
    BOLT_TIGHT = 3,        /**< Fully tightened. */
    BOLT_TIGHTNESS_COUNT   /**< Number of bolt states and picture slots. */
};

/** Wear-adjustment factor used twice in engine/transmission resale. */
#define PART_RESALE_FACTOR 0.9

/** Wear sentinel for parts without a condition percentage. */
#define PART_WEAR_UNTRACKED (-128)
/** Menu-label byte suppressing the condition percentage. */
#define PART_WEAR_LABEL_NONE 0x80
/** Dollar increment used when rounding sale offers down. */
#define SALE_PRICE_STEP 5
/** Dollars added when the demo cannot afford a purchase. */
#define DEMO_PURCHASE_CASH 1999
/**
 * @name Intake resale offers
 * @brief Offer is 70..89 percent of carburetor or manifold value.
 * @{
 */
#define PART_SALE_RANDOM_PCT 20
#define PART_SALE_BASE_PCT 70
/** @} */
/**
 * @name Car sale negotiation
 * @brief Initial offer is 75..94 percent of value; counters scale that offer.
 * @{
 */
#define CAR_SALE_RANDOM_PCT 20
#define CAR_SALE_BASE_PCT 75.0
/** Asking prices above this multiple of car value are rejected. */
#define CAR_SALE_MAX_FACTOR 1.15
/** Exclusive random percentage-point bound added to each counter base. */
#define CAR_COUNTER_RANDOM_PCT 5
/** First counter base, as a percentage of the initial offer. */
#define CAR_COUNTER_FIRST_PCT 85.0
/** Second counter base, as a percentage of the initial offer. */
#define CAR_COUNTER_SECOND_PCT 91.0
/** Final offer must reach this fraction of the requested price. */
#define CAR_SALE_ACCEPT_FACTOR 0.9
/** @} */

/**
 * @name Game-ending reasons
 * @brief Bits select the ending and whether to record a victory score.
 * @{
 */
/** The 91-day season expired. */
#define END_TIME_UP 0x01
/** The King lost the final road race. */
#define END_KING_BEATEN 0x02
/** The post-race jail visit ended the game. */
#define END_JAIL 0x04
/** No cars or spares remain and cash cannot fund a restart. */
#define END_BROKE 0x10
/** @} */

/** @brief Body overlays drawn on a car, as bits. */
enum BodyMod {
    BODY_FRONT_BUMPER = 1, /**< Front-bumper overlay. */
    BODY_REAR_BUMPER = 2,  /**< Rear-bumper overlay. */
    BODY_ROOF = 4,         /**< Chopped-roof overlay. */
    BODY_LARGE_ENGINE = 8  /**< Large-engine overlay. */
};

/**
 * @name Packed car appearance
 * @brief Packed sticker and body-modification fields.
 * @{
 */
/** Sticker bits 8..12; roof and bumper flags occupy bits 13..15. */
#define APPEARANCE_STICKER_SHIFT 8
/** Sticker selection field in the packed car appearance. */
#define APPEARANCE_STICKER_MASK 0x1f00
/** AND mask clearing the sticker field. */
#define APPEARANCE_NO_STICKER 0xe0ff
#define APPEARANCE_ROOF_SHIFT 13
#define APPEARANCE_ROOF_MASK 1
/** The car roof has been chopped. */
#define APPEARANCE_ROOF 0x2000
#define APPEARANCE_REAR_BUMPER_SHIFT 14
#define APPEARANCE_REAR_BUMPER_MASK 1
/** The rear bumper has been removed. */
#define APPEARANCE_REAR_BUMPER 0x4000
#define APPEARANCE_FRONT_BUMPER_SHIFT 15
#define APPEARANCE_FRONT_BUMPER_MASK 1
/** The front bumper has been removed. */
#define APPEARANCE_FRONT_BUMPER 0x8000
/** AND mask clearing the chopped-roof bit. */
#define APPEARANCE_KEEP_ROOF 0xdfff
/** AND mask clearing the rear-bumper removal bit. */
#define APPEARANCE_KEEP_REAR_BUMPER 0xbfff
/** AND mask clearing the front-bumper removal bit. */
#define APPEARANCE_KEEP_FRONT_BUMPER 0x7fff
/** AND mask clearing the unused low byte. */
#define APPEARANCE_KEEP_HIGH_BYTE 0xff00
#define STICKER_INDEX_MASK 0x1f
/** @} */

/** Internal menu result when excess inventory prevents saving. */
#define GAME_MENU_BLOCKED 0x309
/**
 * @name Inventory restrictions
 * @brief Excess stock blocks purchases, travel and saving.
 * @{
 */
#define MAX_OWNED_CARS 15
#define MAX_OWNED_PARTS 130
/** @} */
/** Number of entries scanned when building the parts purchase list. */
#define PART_CATALOG_COUNT 0x2c
/**
 * @name Catalog category bases
 * @brief Zero-based catalog offsets added to equipment variants.
 * @{
 */
#define PART_TRANSMISSION_FIRST 9
#define PART_CARBURETOR_FIRST 0x16
#define PART_MANIFOLD_FIRST 0x19
#define PART_TIRES_FIRST 0x28
/** @} */
/** Stored fuel units per gallon. */
#define FUEL_UNITS_PER_GALLON 10
/** Initial fuel in tenths of a gallon: four gallons. */
#define NEW_CAR_FUEL 0x28
/** Minimum fuel to leave for Bob's, in tenths of a gallon. */
#define MIN_BOBS_FUEL 9
/** Fuel at or above this many tenths of a gallon blocks a pump visit. */
#define REFUEL_VISIT_LIMIT 0x8c
/** Dollar value substituted for every V6 engine in part valuation. */
#define V6_PART_VALUE 100
/** Minimum catalog base price, in dollars, for a purchase-list entry. */
#define MIN_AD_PART_PRICE 0x14

#include "POOLS.H"

struct CarPoolNode;

/** @brief Tire equipment variants. */
enum TireVariant {
    TIRES_STANDARD = 0,     /**< Standard tires. */
    TIRES_BRAND_NAME = 1,   /**< Brand-name tires. */
    TIRES_RACING_SLICKS = 2 /**< Racing slicks. */
};

/** @brief Intake layouts by carburetor count and size. */
enum ManifoldVariant {
    MANIFOLD_TWO_BARREL = 0,  /**< One 2-barrel carb. */
    MANIFOLD_FOUR_BARREL = 1, /**< One 4-barrel carb. */
    MANIFOLD_TRI_POWER = 2,   /**< Three 2-barrel carbs (Tri-Power). */
    MANIFOLD_DUAL_QUAD = 3,   /**< Two 4-barrel carbs (Dual-Quad). */
    MANIFOLD_RACING = 4       /**< Two 4-barrel carbs (racing). */
};

/** @brief Carburetor equipment variants. */
enum CarburetorVariant {
    CARB_TWO_BARREL = 0,  /**< Two-barrel carburetor. */
    CARB_FOUR_BARREL = 1, /**< Four-barrel carburetor. */
    CARB_RACING = 2       /**< Racing four-barrel carburetor. */
};

/** @brief Transmission equipment variants. */
enum TransmissionVariant {
    TRANS_AUTOMATIC = 0,   /**< Automatic. */
    TRANS_THREE_SPEED = 1, /**< Three-speed manual. */
    TRANS_FOUR_SPEED = 2,  /**< Four-speed manual. */
    TRANS_RACING = 3       /**< Racing four-speed manual. */
};

/** @brief Engine equipment variants. */
enum EngineVariant {
    ENGINE_V6 = 0,       /**< V6. */
    ENGINE_V8_SMALL = 1, /**< Small V8. */
    ENGINE_V8_LARGE = 2  /**< Large V8. */
};

/** @brief Ignition-wire presence and connection state. */
enum WireState {
    WIRES_DISCONNECTED = -1, /**< Wires disconnected. */
    WIRES_ABSENT = 0,        /**< No engine wires. */
    WIRES_CONNECTED = 1      /**< Wires connected for running. */
};

/** @brief Installed engine slots, independent of catalog categories. */
enum EngineRackSlot {
    RACK_NONE = -1,       /**< No selected installed component. */
    RACK_ENGINE = 0,      /**< Engine. */
    RACK_MANIFOLD = 1,    /**< Intake manifold. */
    RACK_CARB_FIRST = 2,  /**< First carburetor. */
    RACK_CARB_SECOND = 3, /**< Second carburetor. */
    RACK_CARB_THIRD = 4   /**< Third carburetor. */
};

/** @brief An engine, manifold and up to three installed carburetors. */
struct Rack {
    /** Installed pointers by @ref EngineRackSlot; null if absent. */
    struct PartNode *parts[ENGINE_RACK_SLOTS];
};

/**
 * @brief A car's installed equipment, assembly state and running condition.
 *
 * Owned cars use pooled storage and are saved with their installed parts.
 * Rival previews and stock performance calculations also use temporary cars.
 */
struct Car {
    int value;                     /**< Accounted car value, in dollars. */
    int specIndex;                 /**< Index into the car specification tables. */
    char paintScheme;              /**< Body-colour pair; @ref PaintScheme. */
    unsigned char stockRank;       /**< Cached model rank; higher is better. */
    struct PartNode *transmission; /**< Installed transmission, or null. */
    struct Rack rack;              /**< Engine, manifold and up to three carbs. */
    struct PartNode *tires;        /**< Installed tire set, or null. */
    /** Transmission attachment states; @ref BoltTightness. */
    char transmissionBolts[TRANSMISSION_BOLT_COUNT];
    /** Engine bolt, two manifold bolts, then carb pairs; @ref BoltTightness. */
    char engineBolts[ENGINE_BOLT_COUNT];
    char wireState;     /**< Ignition-wire connection; @ref WireState. */
    signed char timing; /**< Ignition timing setting; zero is optimal. */
    char unused;        /**< Unused saved byte. */
    /** Fuel remaining; @ref FUEL_UNITS_PER_GALLON units per gallon. */
    int fuel;
    /**
     * Sticker field @ref APPEARANCE_STICKER_MASK and modification flags
     * @ref APPEARANCE_ROOF, @ref APPEARANCE_REAR_BUMPER,
     * @ref APPEARANCE_FRONT_BUMPER; low byte unused.
     */
    unsigned appearance;
    struct Car *next; /**< Other-car link; may be stale on current car. */
};

extern struct Car *g_old;
extern struct Car *g_new;
extern int g_carPulledIn;
extern int g_picsFreed;
void gm_pause(int a);
void gm_message(int n);
struct PartNode *near gm_new_part(void);
int gm_part_value(struct PartNode *n);
struct CarPoolNode *near gm_new_car(void);
struct PartNode *near gm_add_part(int idx, int a);
struct Car *near gm_make_car(int idx, int value, struct Car *old);
int near gm_take_part(struct PartNode *n, int keep);
int near gm_scrap_car(struct Car *c, int keep);
int gm_car_look(struct Car *c);
void gm_play_anim(int h);
void gm_popup(int a, int b, int c);
void near gm_run(void);

#endif
