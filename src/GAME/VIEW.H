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

extern int g_opponentPics[][4];                         /* DS:5E14 */
extern int g_5e3c;                                      /* DS:5E3C */
extern struct ViewPages g_viewPages; /* DS:5E3E */
extern struct Clip g_viewRect;                          /* DS:5E42 */
extern struct ViewClip g_viewClip; /* DS:5E4E */
extern unsigned char g_shrinkMasks[3][4];               /* DS:5E56 */
extern int g_sceneBitmapIds[4];                         /* DS:5E62 */
extern int g_roadLineWidths[21];                        /* DS:5E6A */
extern int g_5e94[4];                                   /* DS:5E94 */
extern struct Ball g_sheetRects[];                      /* DS:5E9C */
extern struct Bitmap far *k_6e60;                       /* DS:6E60 */
extern struct Bitmap far *ax72f4;                       /* DS:72F4 */
extern struct Bitmap far *g764aw;                       /* DS:764A */
extern struct Bitmap far *ar767eu;                      /* DS:767E */
extern struct Bitmap far *g8184y;                       /* DS:8184 */
extern struct Bitmap far *bz8b7ex;                      /* DS:8B7E */
extern int far *am8b82y;                                /* DS:8B82 */
extern struct Bitmap far *ak8bcat;                      /* DS:8BCA */
void vw_unclip_top(void);                             /* 0x28840 */
void vw_clip(void);                                   /* 0x28860 */
void vw_load(void);                                   /* 0x28879 */
void vw_sprite(int x, int y, int first, struct List l, int dir);/* 0x2905A */
void vw_road_row(int n);                              /* 0x29398 */
void vw_mirror_row(void);                             /* 0x2969D */
void vw_draw_dot(int x, int y);                       /* 0x29F0F */
void vw_draw(void);                                   /* 0x2AFDC */
void vw_start(void);                                  /* 0x2B6CA */

#endif
