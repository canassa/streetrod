/**
 * @file VIEW.H
 * @brief Driving scene rendering and projected row cursors.
 */

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

/**
 * @brief Current and adjacent projected rows, traversed from far to near.
 *
 * Coordinates are screen pixels. The far flag pointers are signed word views
 * of the unsigned track arrays, with the same @ref RoadsideFlags format.
 */
struct RenderRows {
    int *leftX;           /**< Current left road edge. */
    int *leftOuterX;      /**< Current left outer verge. */
    int *buildingY;       /**< Current base building-top row. */
    int *rightOuterX;     /**< Current right outer verge. */
    int *prevLeftX;       /**< Adjacent near left road edge. */
    int *prevLeftOuterX;  /**< Adjacent near left outer verge. */
    int *prevBuildingY;   /**< Adjacent near base building-top row. */
    int *prevRightOuterX; /**< Adjacent near right outer verge. */
    int *rightX;          /**< Current right road edge. */
    int *y;               /**< Current road surface row. */
    int *prevRightX;      /**< Adjacent near right road edge. */
    int *prevY;           /**< Adjacent near road surface row. */
    int far *leftFlags;   /**< Current left sample; @ref RoadsideFlags. */
    int far *rightFlags;  /**< Current right sample; @ref RoadsideFlags. */
    int *lineWidth;       /**< Projected center-line width. */
};

extern struct ViewPages g_viewPages;
extern struct Clip g_viewRect;
extern struct ViewClip g_viewClip;
extern struct Bitmap far *g_lightsBm;
extern struct Bitmap far *g_windowStrutArt;
extern struct Bitmap far *g_citySkylineBmp;
extern struct Bitmap far *g_skylineCountry;
extern struct Bitmap far *g_imageFullMirror;
extern struct Bitmap far *g_rearBasePic;
/** Signed view of the current shared road sample; @ref TerrainFlags. */
extern int far *g_renderRoadFlagsWordPtr;
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
