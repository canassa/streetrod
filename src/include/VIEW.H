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
extern int g_drawPage;                                  /* DS:5E3E */
extern int g_showPage;                                  /* DS:5E40 */
extern struct Clip g_viewRect;                          /* DS:5E42 */
extern int g_clipXMin;                                  /* DS:5E4E */
extern int g_clipYMin;                                  /* DS:5E50 */
extern int g_clipYMax;                                  /* DS:5E52 */
extern int g_clipXMax;                                  /* DS:5E54 */
extern unsigned char g_shrinkMasks[3][4];               /* DS:5E56 */
extern int g_5e62[];                                    /* DS:5E62 */
extern int g_5e86;                                      /* DS:5E86 */
extern int g_5e88[5];                                   /* DS:5E88 */
extern int g_5e92;                                      /* DS:5E92 */
extern int g_5e94[4];                                   /* DS:5E94 */
extern struct Ball g_sheetRects[];                      /* DS:5E9C */
extern struct Bitmap far *k_6e60;                       /* DS:6E60 */
extern int dz70e0;                                      /* DS:70E0 */
extern struct Bitmap far *ax72f4;                       /* DS:72F4 */
extern int a7312av;                                     /* DS:7312 */
extern int br7314;                                      /* DS:7314 */
extern struct Bitmap far *g764aw;                       /* DS:764A */
extern struct Bitmap far *ar767eu;                      /* DS:767E */
extern int g789eu;                                      /* DS:789E */
extern int f_78a0;                                      /* DS:78A0 */
extern int ac7ae8t;                                     /* DS:7AE8 */
extern int az7aeau;                                     /* DS:7AEA */
extern int e_7f2e;                                      /* DS:7F2E */
extern int g7f30t;                                      /* DS:7F30 */
extern int al7fa4z;                                     /* DS:7FA4 */
extern int by7fa6;                                      /* DS:7FA6 */
extern int dz7ff2;                                      /* DS:7FF2 */
extern int g7ff4g;                                      /* DS:7FF4 */
extern struct Bitmap far *g8184y;                       /* DS:8184 */
extern struct Bitmap far *bz8b7ex;                      /* DS:8B7E */
extern int far *am8b82y;                                /* DS:8B82 */
extern struct Bitmap far *ak8bcat;                      /* DS:8BCA */
void view_unclip_top(void);                             /* 0x28840 */
void view_clip(void);                                   /* 0x28860 */
void view_load(void);                                   /* 0x28879 */
void view_sprite(int x, int y, int first, struct List l, int dir);/* 0x2905A */
void view_road_row(int n);                              /* 0x29398 */
void view_mirror_row(void);                             /* 0x2969D */
void view_draw_dot(int x, int y);                       /* 0x29F0F */
void view_draw(void);                                   /* 0x2AFDC */
void view_start(void);                                  /* 0x2B6CA */

#endif
