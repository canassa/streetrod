/* TIRES.H - declarations of what TIRES.C defines. */

#ifndef TIRES_H
#define TIRES_H

#include "CARPIC.H"
struct Sprite;

struct TireWork {
    struct Bitmap far *liftBuf;
    int wheelX2;
    struct Bitmap far *liftPics[3];
    int underlayY;
    struct Rect overlayBounds;
    struct Bitmap far *wheelBitmap;
    struct Bitmap liftHeader;
    struct Bitmap far *rollCanvas;
    struct Bitmap far *jackColumn;
    struct Bitmap far *backdrop;
    struct Bitmap far *column;
    struct Bitmap far *groundStrip;
    struct Bitmap far *underlay;
    struct Sprite *sprite;
    int phase;
    struct Bitmap rollHeader;
    struct Bitmap jackHeader;
    int carX;
    int rollShift;
    struct Bitmap wheelHeader;
    int wheelX1;
    struct Bitmap far *vehicleBottom;
    struct Bitmap far *groundOverlay;
    struct Bitmap far *basePic;
    struct Bitmap far *frames[4];
    struct Bitmap far *copyBitmap;
    struct Bitmap far *liftCar;
    struct Bitmap far *liftBackdrop;
};

extern int g_spriteBgY;
void ti_change(int *a, int *b);

#endif
