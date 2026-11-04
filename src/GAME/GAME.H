/* GAME.H - declarations of what GAME.C defines. */

#ifndef GAME_H
#define GAME_H

#include "POOLS.H"

struct Node2;

enum TireVariant {
    TIRES_STANDARD = 0,
    TIRES_BRAND_NAME = 1,
    TIRES_RACING_SLICKS = 2
};

enum ManifoldVariant {
    MANIFOLD_TWO_BARREL = 0,
    MANIFOLD_FOUR_BARREL = 1,
    MANIFOLD_DUAL_QUAD = 2,
    MANIFOLD_TRI_POWER = 3,
    MANIFOLD_RACING = 4
};

enum CarburetorVariant {
    CARB_TWO_BARREL = 0,
    CARB_FOUR_BARREL = 1,
    CARB_RACING = 2
};

enum TransmissionVariant {
    TRANS_AUTOMATIC = 0,
    TRANS_THREE_SPEED = 1,
    TRANS_FOUR_SPEED = 2,
    TRANS_RACING = 3
};

enum EngineVariant {
    ENGINE_V6 = 0,
    ENGINE_V8_SMALL = 1,
    ENGINE_V8_LARGE = 2
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
    char paintScheme;
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
extern int g_paperPics[];
extern int g_paperPick;
extern int g_paperDay;
extern char *g_offerText;
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
