/* VIDEO.H - declarations of what VIDEO.C defines. */

#ifndef VIDEO_H
#define VIDEO_H

struct Bitmap;
struct Cmd;

struct Box {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
    char unused[0x1c - 12];
};

struct Obj {
    int id;
    int scriptIndex;
    int pendingTimers;
    int unused1;
    struct Bitmap far *img;
    void far *unused2;
    struct Box box;
};

struct Colors {
    char text;
    char textBg;
    char fill;
    char line;
    char frame;
};

struct Style {
    struct Colors pal[7];
    struct Colors c23;
    struct Colors c28;
    struct Colors c2d;
    struct Colors c32;
    struct Colors colors;
    char unused[5];
    struct Colors hilite;
    struct Colors popup;
    struct Colors bank; /* fifth palette bank, part of the 80-byte record */
};

struct AnimRun {
    struct Cmd *command;
    struct Obj *object;
    struct Box *box;
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

extern struct Style g_style16;
extern struct Style g_style4;
extern unsigned char g_palette[17];
extern char g_blackPalette[16];
extern unsigned char g_altPalette[17];
extern struct Bitmap far *g_pageBmps[];
extern struct Obj g_scriptObjsData[];
extern struct Style *bustyle;
extern struct Colors x_colors;
extern char g_colScreenBorder;
extern struct Bitmap far *g_autoSprImagePtr;
extern int g_picXDraw;
extern int g_sprDrawY;
void vi_garage_pal(void);
void vi_bobs_pal(void);
void vi_name_pal(void);
void vi_swap_pal(void);
void vi_init(struct Bitmap far *front, struct Bitmap far *back);
void vi_anim_stop(int n);
int vi_anim_start(int n);
void vi_anim_resume(int n);
int vi_anim_active(int n);
int vi_anim_hook(struct Obj *o, struct Cmd *c);
void vi_palette_reg(int reg, char color);
void vi_set_palette(char *table);

#endif
