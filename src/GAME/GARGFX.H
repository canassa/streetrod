/**
 * @file GARGFX.H
 * @brief Garage equipment drawing and selection hotspots.
 */

#ifndef GARGFX_H
#define GARGFX_H

#include "ADS.H"
#include "GAME.H"
#include "HOTSPOT.H"
struct Bitmap;
struct Car;

/** @brief Engine-bay artwork positions, independent of installed rack slots. */
enum EngineDrawSlot {
    ENGINE_DRAW_ENGINE = 0,         /**< Engine body. */
    ENGINE_DRAW_MANIFOLD = 1,       /**< Manifold. */
    ENGINE_DRAW_TWO_V6 = 4,         /**< Single two-barrel carb on a V6. */
    ENGINE_DRAW_TWO_V8 = 5,         /**< Single two-barrel carb on a V8. */
    ENGINE_DRAW_FOUR = 6,           /**< Single four-barrel carb. */
    ENGINE_DRAW_TRI_FIRST = 7,      /**< First Tri-Power carb. */
    ENGINE_DRAW_TRI_SECOND = 8,     /**< Second Tri-Power carb. */
    ENGINE_DRAW_TRI_THIRD = 9,      /**< Third Tri-Power carb. */
    ENGINE_DRAW_DUAL_FIRST = 10,    /**< First Dual-Quad carb. */
    ENGINE_DRAW_DUAL_SECOND = 11,   /**< Second Dual-Quad carb. */
    ENGINE_DRAW_RACING_FIRST = 12,  /**< First racing-manifold carb. */
    ENGINE_DRAW_RACING_SECOND = 13, /**< Second racing-manifold carb. */
    /** Coordinate-map extent, including gaps. */
    ENGINE_DRAW_SLOT_COUNT = 14
};

/** @brief Bolt position within an engine-bay component. */
enum PartBoltSide {
    PART_BOLT_FIRST = 0, /**< Engine bolt or first manifold/carb bolt. */
    PART_BOLT_SECOND = 1 /**< Second manifold/carb bolt. */
};

/** @brief Current engine-bay component, bolt position and wire artwork. */
struct PartSelection {
    /** Wire picture ID, or zero when there are no ignition wires. */
    int wirePic;
    enum EngineDrawSlot drawSlot; /**< Component's drawing location. */
    int unused;                   /**< Unused selection word. */
    enum PartBoltSide boltSide;   /**< Bolt within the selected component. */
};

extern struct PartSelection g_slotSelec;

extern struct Bitmap far *g_boltOverlay;
extern struct Blt g_cpyBoltPatchRecord;
extern int g_partsPics[ENGINE_RACK_SLOTS];
extern int g_engineBoltPicIds[9];
void gx_trans_bolt(enum BoltTightness a, int b);
void gx_trans_drop(int a);
void gx_trans_pic(int n, int flag);
void gx_trans_clear(int n, int flag);
void gx_trans_draw(int n, char far *s);
void gx_part_pos(int *px, int *py);
void gx_bolt_pos(int *px, int *py);
void gx_draw_bolt(int n);
void gx_drop_bolt(void);
void gx_draw_part(int n, int flag);
void gx_erase_part(int n, int flag);
void gx_draw_wires(void);
void gx_draw_parts(struct Car *car);
void gx_draw_bolts(enum EngineDrawSlot *a, enum PartBoltSide *b, int flag);
void gx_build_hots(enum EngineDrawSlot *xs, enum PartBoltSide *ys);
void gx_sort_hots(int first, int n);

#endif
