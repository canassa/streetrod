/**
 * @file VIDEO.H
 * @brief Display state, drawing hooks and bitmap animation scripts.
 */

#ifndef VIDEO_H
#define VIDEO_H

#include "BITMAP.H"
#include "STRUCTS.H"

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

/** @brief Persistent pixel-copy rectangle for a bitmap animation object. */
struct Box {
    int w;                  /**< Width in pixels. */
    int h;                  /**< Height in rows. */
    int sx;                 /**< Source left pixel column. */
    int sy;                 /**< Source top row. */
    int dx;                 /**< Destination left pixel column. */
    int dy;                 /**< Destination top row. */
    char unused[0x1c - 12]; /**< Reserved bytes after the copy rectangle. */
};

/** Number of concurrently allocated bitmap-animation objects. */
#define ANIMATION_OBJECT_COUNT 5

/** Script identity or resume index marking an inactive animation object. */
#define ANIMATION_INACTIVE (-1)

/** @brief Runtime bitmap, rectangle and resume state of one animation script. */
struct Obj {
    int scriptId;             /**< Script-table index, or @ref ANIMATION_INACTIVE. */
    int resumeIndex;          /**< Command-array index at which execution resumes. */
    int pendingTimers;        /**< Number of outstanding animation timer entries. */
    int unused1;              /**< Reserved word. */
    struct Bitmap far *image; /**< Borrowed arena bitmap used by the script. */
    void far *unused2;        /**< Unused pointer, cleared when the script stops. */
    struct Box box;           /**< Rectangle retained across commands and waits. */
};

/** @brief Border treatment selected by a menu drawing scheme. */
enum BevelStyle {
    BEVEL_NONE = 0,   /**< Suppress the menu bevel. */
    BEVEL_SINGLE = 1, /**< Fill border with one highlight line. */
    BEVEL_DOUBLE = 2, /**< Fill border with two highlight lines. */
    BEVEL_SOLID = 3   /**< Solid border in the line color, without highlights. */
};

/**
 * @brief Palette indices and bevel style for text, fills and lines.
 *
 * Color indices refer to the active palette, not fixed RGB colors.
 */
struct Colors {
    char text;       /**< Text foreground palette index. */
    char textBg;     /**< Text background palette index. */
    char fill;       /**< Rectangle fill palette index. */
    char line;       /**< Line and bevel highlight palette index. */
    char bevelStyle; /**< Border selector from @ref BevelStyle. */
};

/** @brief Drawing-color schemes shared by menus and scene text. */
enum StyleIndex {
    STYLE_NONE = 0,  /**< No box drawing; all-zero drawing colors. */
    STYLE_PANEL = 1, /**< Parts, cars and general menu panels. */
    /** Inverse panel colors; not selected by current menus. */
    STYLE_INVERSE = 2,
    STYLE_BUTTON = 3,         /**< Ordinary menu button. */
    STYLE_DEFAULT_BUTTON = 4, /**< Emphasized default menu button. */
    STYLE_TEXT = 5,           /**< General dialog text and edit cursor. */
    STYLE_SAVE_NAME = 6,      /**< Save-game name editing. */
    STYLE_MESSAGE = 7,        /**< Message panel and its text. */
    STYLE_MESSAGE_BUTTON = 8, /**< Message acknowledgement button. */
    STYLE_LIST_COPY = 9,      /**< Saved copy of the active list colors. */
    STYLE_SCROLL_BUTTON = 10, /**< List scroll buttons. */
    STYLE_STATUS = 11,        /**< Status and offer text outside menu panels. */
    STYLE_SAVE_LIST = 12,     /**< Saved-game list rows. */
    /** Newspaper rows, highlighted prices and car labels. */
    STYLE_NEWSPAPER = 13,
    STYLE_POPUP = 14,      /**< Speech bubbles and offer panels. */
    STYLE_RIVAL_LIST = 15, /**< Met-rival list and its controls. */
    STYLE_COUNT = 16       /**< Number of drawing-color schemes. */
};

/** @brief Complete drawing-color set for a display mode. */
struct Style {
    struct Colors schemes[STYLE_COUNT]; /**< Colors indexed by @ref StyleIndex. */
};

/** @brief Borrowed interpreter cursor for the currently executing object. */
struct AnimRun {
    struct AnimCommand *command; /**< Current instruction. */
    struct Obj *object;          /**< Active animation object. */
    struct Box *box;             /**< Rectangle within that same object. */
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

/**
 * @brief Mode-selected far entry points for drawing and display updates.
 *
 * Rectangle operations receive pixel coordinates. Copy callbacks take source
 * before destination; the reveal callback takes destination before source.
 * Drawing callbacks use the current shared drawing target where no bitmap is
 * passed. Return values of the three bitmap-copy callbacks are ignored.
 */
struct Hooks {
    /** Copy a rectangle and selected pixel/mask planes. */
    int(far *copy)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
    /** Copy a rectangle with partial-byte pixel alignment. */
    int(far *shiftBlit)(struct Bitmap far *, struct Bitmap far *, struct Blt *);
    /** Combine a rectangle with its transparency mask. */
    int(far *maskedBlit)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
    /** Draw a string and update @ref TextEnd. */
    void(far *drawText)(struct Bitmap far *, int, int, char *);
    /** Save/draw cursor pixels; X and width are in bytes. */
    void(far *cursorBlit)(char far *, char far *, char far *, char far *, int, int, int, int);
    /** Copy an entire display page, source to destination. */
    void(far *copyPage)(char far *, char far *);
    /** Replace one palette index within a pixel rectangle. */
    void(far *recolorRegion)(struct Bitmap far *, int, int, int, int, int, int);
    /** Blank the display or its palette. */
    void(far *blank)(void);
    /** Enable the display or restore its palette. */
    void(far *show)(void);
    /** Draw/advance a sprite; nonzero while active. */
    int(far *spriteStep)(struct Sprite *);
    /** Reveal source rows over destination in timed stages. */
    void(far *reveal)(char far *, char far *);
    /** Animate a bitmap vertically and refresh its strip. */
    void(far *slideSprite)(int, int, int, int, int, struct Bitmap far *);
    /** Draw between pixel endpoints using the shared target. */
    void(far *drawLine)(int, int, int, int, int);
    /** Fill left/right pixel bounds at Y in a given color. */
    void(far *fillSpan)(int, int, int, int);
    /** Fill from a row through the lower clip row. */
    void(far *fillBelow)(int, int);
};

/**
 * @brief Display pages, linear pixel geometry and automatic-refresh policy.
 *
 * Hercules retains its display mode while its visible bitmap uses CGA storage.
 */
struct Video {
    struct Bitmap far *front;  /**< Visible display page. */
    struct Bitmap far *back;   /**< Current drawing page. */
    struct Bitmap far *hoodBg; /**< Lazy engine-bay background for part drawing. */
    enum BitmapKind mode;      /**< Active display selector. */
    int planes;                /**< Number of stored color planes in a linear bitmap. */
    int shift;                 /**< Log2 pixels per stored byte in one plane. */
    int planeBitsPerPixel;     /**< Bits per pixel within one stored plane. */
    int xByteMask;             /**< Signed mask aligning X to a whole pixel-data byte. */
    BOOL copyToFront;          /**< Refresh visible rectangles after back-page drawing. */
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
