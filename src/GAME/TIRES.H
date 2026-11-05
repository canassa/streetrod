/**
 * @file TIRES.H
 * @brief Garage tire-change animation.
 */

#ifndef TIRES_H
#define TIRES_H

#include "CARPIC.H"
struct Sprite;

/**
 * @brief Working images and scratch views for one garage tire change.
 *
 * Arena pointers last only for their animation stage; bitmap headers also
 * provide views over shared video or work memory.
 */
struct TireWork {
    /** Car-lift sprite work bitmap; unused by Tandy. */
    struct Bitmap far *liftBuf;
    /** Second tire X relative to the car left. */
    int wheelX2;
    struct Bitmap far *jackFrames[3]; /**< Jack up, column and down images. */
    /** Shadow top row within a wheel frame. */
    int underlayY;
    /** Inclusive foreground bounds for the current sprite. */
    struct Rect overlayBounds;
    struct Bitmap far *wheelBitmap; /**< Temporary loaded tire artwork. */
    /** Embedded EGA car-lift scratch view. */
    struct Bitmap liftHeader;
    /** Ground-level backdrop for rolling wheels. */
    struct Bitmap far *rollCanvas;
    /** Saved vertical background behind the jack. */
    struct Bitmap far *jackColumn;
    /** Active first- or second-wheel background column. */
    struct Bitmap far *backdrop;
    /** Tire artwork composited into captured wheel columns. */
    struct Bitmap far *rollingTirePic;
    /** Temporary garage strip containing car shadows. */
    struct Bitmap far *groundStrip;
    /** Shadow artwork placed beneath wheel frames. */
    struct Bitmap far *tireShadow;
    /** Borrowed shared sprite state for the current motion. */
    struct Sprite *sprite;
    /** Old-wheel pixel shift, then new-wheel frame index. */
    int phase;
    struct Bitmap rollHeader; /**< Embedded rolling scratch view. */
    struct Bitmap jackHeader; /**< Embedded jack scratch view. */
    int carX;                 /**< Car left in screen pixels. */
    /** Pixel alignment remainder for the current frame. */
    int rollShift;
    /** Embedded vertical wheel-motion scratch view. */
    struct Bitmap wheelHeader;
    /** First tire X relative to the car left. */
    int wheelX1;
    /** Saved bottom rows of the bare car. */
    struct Bitmap far *vehicleBottom;
    /** Temporary aligned car-shadow image. */
    struct Bitmap far *groundOverlay;
    struct Bitmap far *shadowPic; /**< Temporary car-shadow resource. */
    /** Four shifted wheel frames; first later holds fitted art. */
    struct Bitmap far *frames[4];
    /** Clean wheel copy or mutable single-frame sprite. */
    struct Bitmap far *copyBitmap;
    /** Car image fitted with old or new tires. */
    struct Bitmap far *liftCar;
    /** Masked garage backdrop including the current jack frame. */
    struct Bitmap far *liftBackdrop;
};

extern int g_spriteBgY;
void ti_change(int *a, int *b);

#endif
