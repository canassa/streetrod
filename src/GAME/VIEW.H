/* VIEW.H - declarations of what VIEW.C defines. */

#ifndef VIEW_H
#define VIEW_H
#include "STRUCTS.H"

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
extern int g_5e3c;
extern struct ViewPages g_viewPages;
extern struct Clip g_viewRect;
extern struct ViewClip g_viewClip;
extern unsigned char g_shrinkMasks[3][4];
extern int g_sceneBitmapIds[4];
extern int g_roadLineWidths[21];
extern int g_5e94[4];
extern struct Ball g_sheetRects[];
extern struct Bitmap far *k_6e60;
extern struct Bitmap far *ax72f4;
extern struct Bitmap far *g764aw;
extern struct Bitmap far *ar767eu;
extern struct Bitmap far *g8184y;
extern struct Bitmap far *bz8b7ex;
extern int far *am8b82y;
extern struct Bitmap far *ak8bcat;
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
