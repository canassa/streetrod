/* VIDEO.H - declarations of what VIDEO.C defines. */

#ifndef VIDEO_H
#define VIDEO_H
#include "STRUCTS.H"

struct Animation {
    struct Cmd *command;
    struct Obj *object;
    struct Box *box;
    int blinkCount;
};

enum AnimOp {
    ANIM_NOP_1 = 1,
    ANIM_BLIT = 2,
    ANIM_NOP_3 = 3,
    ANIM_BLIT_MASKED = 4,
    ANIM_HOOK = 5,
    ANIM_STOP = 6,
    ANIM_JUMP = 7,
    ANIM_LOAD_IMAGE = 8,
    ANIM_SET_BOX = 9,
    ANIM_SET_SIZE = 10,
    ANIM_WAIT = 11,
    ANIM_COPY_TO_FRONT = 12,
    ANIM_TILE_CAR = 13
};

enum BoxSet {
    BOX_KEEP = 0,
    BOX_SET_SOURCE = 1,
    BOX_SET_DESTINATION = 2,
    BOX_SET_BOTH = 3
};

enum AnimWaitMode {
    ANIM_WAIT_ONLY = 0,
    ANIM_WAIT_COPY_TO_FRONT = 4
};

extern struct Style g_style16;                          /* DS:03A0 */
extern struct Style g_style4;                           /* DS:03F0 */
extern unsigned char g_palette[];                       /* DS:0440 */
extern int g_palTail;                                      /* DS:0450 */
extern char g_blackPalette[16];                         /* DS:0452 */
extern unsigned char g_altPalette[16];                  /* DS:0462 */
extern int g_altPalTail;                                      /* DS:0472 */
extern struct Bitmap far *g_pageBmps[];                 /* DS:7316 */
extern struct Obj bm7568[];                             /* DS:7568 */
extern struct Style *bustyle;                           /* DS:787E */
extern struct Colors x_colors;                          /* DS:8242 */
extern char g_colScreenBorder;                                     /* DS:8248 */
extern struct Bitmap far *g_autoSprImagePtr;                       /* DS:825A */
extern int ac825et;                                     /* DS:825E */
extern int aw8260y;                                     /* DS:8260 */
void F_10B7C(void);                                     /* 0x10B7C */
void video_bobs_pal(void);                                     /* 0x10B8C */
void video_name_pal(void);                                     /* 0x10B9C */
void video_swap_pal(void);                              /* 0x10BAC */
void video_init(struct Bitmap far *front, struct Bitmap far *back);/* 0x10BD9 */
void video_anim_stop(int n);                            /* 0x10D6C */
int video_anim_start(int n);                            /* 0x11149 */
void video_anim_resume(int n);                          /* 0x111C1 */
int video_anim_active(int n);                           /* 0x111F2 */
int video_anim_hook(struct Obj *o, struct Cmd *c);      /* 0x11213 */
void F_112CB(int reg, char color);                      /* 0x112CB */
void video_set_palette(char *table);                    /* 0x11324 */

#endif
