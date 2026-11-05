/**
 * @file CARPIC.H
 * @brief Car assembly pictures and arrival animations.
 */

#ifndef CARPIC_H
#define CARPIC_H

#include "BITMAP.H"
#include "GAME.H"
#include "STRUCTS.H"

/** @brief A composited EGA car strip packed into scratch video memory. */
struct Band {
    char far *pixels; /**< Strip pixels, packed at the car bitmap's pitch. */
    int sourceY;      /**< First row of the strip within the car frame. */
    int height;       /**< Strip height in pixels. */
};

/**
 * Height in pixels of the car-animation background band; scratch bands follow
 * it.
 */
#define BACKDROP_ROWS 70

/** @brief Car movement or stationary compositing requested by the scene. */
enum CarAnimKind {
    /** Forward arrival using the current saved background. */
    CAR_DRIVE_IN = 1,
    CAR_DRIVE_OUT = 2, /**< Forward departure to the right. */
    /** Composite a stationary car and copy the back page to the front. */
    CAR_SHOW_BACK_PAGE = 3,
    /** Capture the scene backdrop before animating arrival. */
    CAR_PULL_IN = 4,
    CAR_BACK_OUT = 6,    /**< Reverse departure using the last car rectangle. */
    CAR_SHOW_SCREEN = 7, /**< Draw a stationary car directly over the screen. */
    /** Redraw a stationary car using the saved backdrop. */
    CAR_SHOW_BACKDROP = 8
};

extern int g_delayScale;
extern int g_dashBuffered;
extern int g_autoTrans;
extern int g_wheelPicIds[];
extern int g_driveRebuild;
extern struct AnimCommand g_animScripts[];
extern int g_animScriptStart[];
extern int g_carMoving;
extern int g_backdropX;

/** @brief Progress of a rival car passing through without parking. */
enum PassByPhase {
    PASS_BY_NONE = 0,     /**< Ordinary arrival, parking or departure. */
    PASS_BY_ARRIVING = 1, /**< Arrive, then continue directly into departure. */
    PASS_BY_DEPARTING = 2 /**< Depart using the arrival frames and backdrop. */
};

/** Current pass-through animation phase. */
extern enum PassByPhase g_carPassPhase;
extern struct Bitmap g_bandRecordsSprArray[];
extern struct Bitmap far *g_lastPic;
extern struct Band g_carSpriteStrips[4];
extern struct Bitmap g_carBackdropHdrBmp;
extern int g_carVisibleHtValue;
void cp_backdrop(struct Bitmap far *src, struct Bitmap far **pa, int mode, struct Bitmap far **pb, int y1, int y0,
                 struct Rect **pr1, struct Rect **pr2);

/**
 * @brief Build car pictures with body changes, a sticker and optional occupant.
 *
 * Shifted frames, tires and shadows share the car's arena lifetime.
 *
 * @param frames Receives the low-stack bitmap pointers.
 * @param specIndex Car specification index.
 * @param bodyMods Body modifications to display.
 * @param sticker One-based sticker choice, or zero for none.
 * @param showPortrait TRUE includes the occupant portrait.
 * @param tirePics Tire picture per frame, or zero for no tires.
 * @param showShadow TRUE underlays the car shadow.
 * @param frameCount Number of output frames; one or four in current callers.
 */
void cp_build(struct Bitmap far **frames, int specIndex, enum BodyMod bodyMods, int sticker, BOOL showPortrait,
              int *tirePics, BOOL showShadow, int frameCount);

/**
 * @brief Draw a parked car or animate its arrival or departure.
 *
 * A requested pass-through immediately follows arrival with departure.
 *
 * @param animation Movement or stationary presentation.
 * @param specIndex Car specification index.
 * @param bodyMods Body modifications to display.
 * @param sticker One-based sticker choice, or zero for none.
 * @param showPortrait TRUE includes the occupant portrait.
 * @param carBottom Bottom screen row of the car.
 * @param useGasMask TRUE preserves the gas station foreground over the car.
 * @param tirePics Tire picture per frame, or zero for no tires.
 */
void cp_show_car(enum CarAnimKind animation, int specIndex, enum BodyMod bodyMods, int sticker, BOOL showPortrait,
                 int carBottom, BOOL useGasMask, int *tirePics);
void cp_patch_bg(struct Bitmap far *bm, int a, int b, int c, int d, int e, int f, int g, int h);

#endif
