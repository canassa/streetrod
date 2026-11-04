/* VIEW.H - declarations of what VIEW.C defines. */

#ifndef VIEW_H
#define VIEW_H

#include "SCENERY.H"
struct Bitmap;

/* source rectangle of one size of ball in the sprite sheet */
struct Ball {
    int sx;
    int w;
    int h;
};

/* clipped rectangle (12 bytes): size, source origin, destination origin. da_clip_blt takes struct Blt *
   but reads and writes only these first 12 bytes, so callers with a Clip local pass it cast. */
struct Clip {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
};

/* Draw and show page indices. */
struct ViewPages {
    int draw;
    int show;
};

/* Clipping bounds shared by VIEW and the span drawing routines. */
struct ViewClip {
    int xMin;
    int yMin;
    int yMax;
    int xMax;
};

struct RenderRows {
    int *leftX;
    int *leftOuterX;
    int *buildingY;
    int *rightOuterX;
    int *prevLeftX;
    int *prevLeftOuterX;
    int *prevBuildingY;
    int *prevRightOuterX;
    int *rightX;
    int *y;
    int *prevRightX;
    int *prevY;
    int far *leftFlags;
    int far *rightFlags;
    int *lineWidth;
};

extern int g_opponentPics[][4];
extern int g_unused20;
extern struct ViewPages g_viewPages;
extern struct Clip g_viewRect;
extern struct ViewClip g_viewClip;
extern unsigned char g_shrinkMasks[3][4];
extern int g_sceneBitmapIds[4];
extern int g_roadLineWidths[21];
extern int g_unused21[4];
extern struct Ball g_sheetRects[];
extern struct Bitmap far *g_lightsBm;
extern struct Bitmap far *g_windowStrutArt;
extern struct Bitmap far *g_citySkylineBmp;
extern struct Bitmap far *g_skylineCountry;
extern struct Bitmap far *g_imageFullMirror;
extern struct Bitmap far *g_rearBasePic;
extern int far *g_viewTerrainBitsData;
extern struct Bitmap far *g_windowBorderImage;
void vw_unclip_top(void);
void vw_clip(void);
void vw_load(void);
void vw_sprite(int x, int y, int first, struct List l, int dir);
void vw_road_row(int n);
void vw_mirror_row(void);
void vw_draw_dot(int x, int y);
void vw_draw(void);
void vw_start(void);

#endif
