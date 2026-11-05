/**
 * @file GAME.H
 * @brief The garage game loop, the player's cars and parts.
 */

#ifndef GAME_H
#define GAME_H

#define ENGINE_RACK_SLOTS 5
#define ENGINE_BOLT_COUNT 9
#define TRANSMISSION_BOLT_COUNT 2

enum BoltTightness {
    BOLT_ABSENT = 0,
    BOLT_INSERTED = 1,
    BOLT_TIGHT = 3
};

#define PART_RESALE_FACTOR 0.9

#define PART_WEAR_UNTRACKED (-128)
#define PART_WEAR_LABEL_NONE 0x80
#define SALE_PRICE_STEP 5
#define DEMO_PURCHASE_CASH 1999
#define PART_SALE_RANDOM_PCT 20
#define PART_SALE_BASE_PCT 70
#define CAR_SALE_RANDOM_PCT 20
#define CAR_SALE_BASE_PCT 75.0
#define CAR_SALE_MAX_FACTOR 1.15
#define CAR_COUNTER_RANDOM_PCT 5
#define CAR_COUNTER_FIRST_PCT 85.0
#define CAR_COUNTER_SECOND_PCT 91.0
#define CAR_SALE_ACCEPT_FACTOR 0.9

#define END_TIME_UP 0x01
#define END_KING_BEATEN 0x02
#define END_JAIL 0x04
#define END_BROKE 0x10

/** @brief Body overlays drawn on a car, as bits. */
enum BodyMod {
    BODY_REAR_BUMPER = 1,  /**< Rear-bumper overlay. */
    BODY_FRONT_BUMPER = 2, /**< Front-bumper overlay. */
    BODY_ROOF = 4,         /**< Chopped-roof overlay. */
    BODY_LARGE_ENGINE = 8  /**< Large-engine overlay. */
};

#define APPEARANCE_STICKER_MASK 0x1f00
#define APPEARANCE_NO_STICKER 0xe0ff
#define APPEARANCE_ROOF 0x2000
#define APPEARANCE_FRONT_BUMPER 0x4000
#define APPEARANCE_REAR_BUMPER 0x8000
#define APPEARANCE_KEEP_ROOF 0xdfff
#define APPEARANCE_KEEP_FRONT_BUMPER 0xbfff
#define APPEARANCE_KEEP_REAR_BUMPER 0x7fff
#define APPEARANCE_KEEP_HIGH_BYTE 0xff00
#define STICKER_INDEX_MASK 0x1f

#define GAME_MENU_BLOCKED 0x309
#define MAX_OWNED_CARS 15
#define MAX_OWNED_PARTS 130
#define PART_CATALOG_COUNT 0x2c
#define PART_TRANSMISSION_FIRST 9
#define PART_CARBURETOR_FIRST 0x16
#define PART_MANIFOLD_FIRST 0x19
#define PART_TIRES_FIRST 0x28
#define FUEL_UNITS_PER_GALLON 10
#define NEW_CAR_FUEL 0x28
#define MIN_BOBS_FUEL 9
#define REFUEL_VISIT_LIMIT 0x8c
#define V6_PART_VALUE 100
#define MIN_AD_PART_PRICE 0x14

#include "POOLS.H"

struct Node2;

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

enum WireState {
    WIRES_DISCONNECTED = -1,
    WIRES_ABSENT = 0,
    WIRES_CONNECTED = 1
};

struct Rack {
    struct Node *parts[5];
};

struct Car {
    int value;
    int specIndex;
    char paintScheme; /**< Body-colour pair; @ref PaintScheme. */
    unsigned char rank;
    struct Node *transmission;
    struct Rack rack;
    struct Node *tires;
    char transmissionBolts[2];
    char engineBolts[9];
    char wireState;
    signed char timing;
    char unused;
    int fuel;
    unsigned appearance; /* bits 8-12 sticker (ga_menu), 13 roof chopped, 14 front and 15 rear bumper stripped */
    struct Car *next;
};

extern struct Car *g_old;
extern struct Car *g_new;
extern int g_carPulledIn;
extern int g_picsFreed;
void gm_pause(int a);
void gm_message(int n);
struct Node *near gm_new_part(void);
int gm_part_value(struct Node *n);
struct Node2 *near gm_new_car(void);
struct Node *near gm_add_part(int idx, int a);
struct Car *near gm_make_car(int idx, int value, struct Car *old);
int near gm_take_part(struct Node *n, int keep);
int near gm_scrap_car(struct Car *c, int keep);
int gm_car_look(struct Car *c);
void gm_play_anim(int h);
void gm_popup(int a, int b, int c);
void near gm_run(void);

#endif
