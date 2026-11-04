/* TIRES.H - declarations of what TIRES.C defines. */

#ifndef TIRES_H
#define TIRES_H
#include "STRUCTS.H"

struct TireWork {
    struct Bitmap far *savedVehicle;
    int wheelX2;
    struct Bitmap far *liftPics[3];
    int underlayY;
    struct Rect overlayBounds;
    struct Bitmap far *wheelBitmap;
    struct Bitmap liftBuffer;
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
    int wheelX;
    struct Bitmap far *vehicleBottom;
    struct Bitmap far *groundOverlay;
    struct Bitmap far *basePic;
    struct Bitmap far *frames[4];
    struct Bitmap far *copyBitmap;
    struct Bitmap far *liftCar;
    struct Bitmap far *liftBackdrop;
};

extern int g_spriteBgY;                                 /* DS:5AA8 */
extern int g_liftPicIds[3];                             /* DS:5AAA */
extern struct Bitmap far *g_wheelBuf;                       /* DS:5AB0 */
extern struct Bitmap far *g_rollBuf;                       /* DS:5AB4 */
extern struct Bitmap far *g_jackBuf;                       /* DS:5AB8 */
void F_1B213(int *a, int *b);                           /* 0x1B213 */

#endif
