/**
 * @file VIDEO.H
 * @brief Display state, drawing hooks and bitmap animation scripts.
 */

#ifndef VIDEO_H
#define VIDEO_H

/** @brief Palette registers replaced when applying player or rival paint. */
enum PaletteSlot {
    PALETTE_CAR_PRIMARY = 6,    /**< Player body base color. */
    PALETTE_CAR_SECONDARY = 7,  /**< Player body highlight. */
    PALETTE_RIVAL_PRIMARY = 12, /**< Rival paint base and diner name color. */
    PALETTE_RIVAL_SECONDARY = 3 /**< Rival body highlight. */
};

struct Bitmap;
struct Blt;
struct Sprite;

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
    struct AnimCommand *command;
    struct Obj *object;
    struct Box *box;
};

/** @brief Script operations on an object bitmap and rectangle. */
enum AnimOp {
    ANIM_NOP_1 = 1,          /**< Advance without changing the object. */
    ANIM_BLIT = 2,           /**< Copy to the back buffer. */
    ANIM_NOP_3 = 3,          /**< Advance without changing the object. */
    ANIM_BLIT_MASKED = 4,    /**< Copy opaque pixels to the back buffer. */
    ANIM_HOOK = 5,           /**< Run a scene callback; stop if requested. */
    ANIM_STOP = 6,           /**< Deactivate and cancel pending timers. */
    ANIM_JUMP = 7,           /**< Jump to the first operand command index. */
    ANIM_LOAD_IMAGE = 8,     /**< Load the first operand picture ID. */
    ANIM_SET_BOX = 9,        /**< Apply @ref BoxSet to the origins. */
    ANIM_SET_SIZE = 10,      /**< Set rectangle width and height in pixels. */
    ANIM_WAIT = 11,          /**< Resume after the first operand ticks. */
    ANIM_COPY_TO_FRONT = 12, /**< Refresh the page and show the cursor. */
    ANIM_TILE_CAR = 13       /**< Tile the clipped car into the bitmap. */
};

/** @brief Coordinate pairs replaced by animation operands. */
enum BoxSet {
    BOX_KEEP = 0,            /**< Leave both origins unchanged. */
    BOX_SET_SOURCE = 1,      /**< Set the source origin in bitmap pixels. */
    BOX_SET_DESTINATION = 2, /**< Set the destination pixel origin. */
    BOX_SET_BOTH = 3         /**< Set both origins to the same pixels. */
};

/** @brief Visible-page refresh policy when an animation yields to a timer. */
enum AnimWaitMode {
    ANIM_WAIT_ONLY = 0,         /**< Wait without refreshing the page. */
    ANIM_WAIT_COPY_TO_FRONT = 4 /**< Refresh the rectangle before waiting. */
};

/**
 * @brief One bitmap animation instruction, or a script's runtime header.
 *
 * Operands depend on the opcode. A script starts after its header, whose first
 * operand holds the active object slot, or -1 after the script stops.
 */
struct AnimCommand {
    signed char opcode; /**< Operation from @ref AnimOp; ignored in headers. */
    /** @ref BoxSet or @ref AnimWaitMode, according to the opcode; else ignored. */
    signed char mode;
    /** X, width, picture ID, command index, wait ticks, hook count or header slot. */
    int operand1;
    int operand2; /**< Y, height or wait resume index (0 means next); else ignored. */
};

struct Hooks {
    int(far *copy)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
    int(far *shiftBlit)(struct Bitmap far *, struct Bitmap far *, struct Blt *);
    int(far *maskedBlit)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
    void(far *drawText)(struct Bitmap far *, int, int, char *);
    void(far *cursorBlit)(char far *, char far *, char far *, char far *, int, int, int, int);
    void(far *copyPage)(char far *, char far *);
    void(far *recolorRegion)(struct Bitmap far *, int, int, int, int, int, int);
    void(far *blank)(void);
    void(far *show)(void);
    int(far *spriteStep)(struct Sprite *);
    void(far *reveal)(char far *, char far *);
    void(far *slideSprite)(int, int, int, int, int, struct Bitmap far *);
    void(far *drawLine)(int, int, int, int, int);
    void(far *fillSpan)(int, int, int, int);
    void(far *fillBelow)(int, int);
};

/* Display state set by vi_init; mode follows front->kind and geometry depends on the mode. */
struct Video {
    struct Bitmap far *front;
    struct Bitmap far *back;
    struct Bitmap far *hoodBg;
    int mode;
    int planes; /* 4 on EGA (-2), else 1 */
    int shift;  /* log2 pixels per byte: 3, 2, 1 */
    int bpp;    /* bits per pixel: 1, 2, 4 */
    int mask;   /* ~(pixels per byte - 1): -8, -4, -2 */
    int copyToFront;
};

extern unsigned char g_palette[17];
extern char g_blackPalette[16];
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
int vi_anim_hook(struct Obj *o, struct AnimCommand *c);
void vi_palette_reg(int reg, char color);
void vi_set_palette(char *table);

#endif
