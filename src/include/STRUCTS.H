/* STRUCTS.H - shared struct types. Hand-written source: one definition per type. */
#ifndef STRUCTS_H
#define STRUCTS_H

struct ManualEntry {
    signed char page;
    signed char color;
};

struct Entry {
    char name[16];
    char score[7];
};

/* source rectangle of one size of ball in the sprite sheet */
struct Ball {
    int sx;
    int w;
    int h;
};

struct Part {
    signed char dx;
    signed char dy;
    signed char w;
    signed char h;
};

struct Run {
    int n;
    struct Part far *parts;
};

struct List {
    int last;
    struct Run *items;
};

union FarPtr {
    char far *p;
    struct {
        unsigned off;
        unsigned seg;
    } w;
};

struct Slot {
    int f0;
    int f2;
    char pad4[2];
    unsigned char f6;
    unsigned char pal[16];
    char pad17;
    long f18;
    int f1c;
    unsigned char f1e;
    char pad1f;
    union FarPtr ptr;
    long f24;
    long f28;
    int f2c;
    unsigned char f2e;
    char pad2f;
};

struct Node {
    int a;
    int b;
    int idx;
    struct Node *next;
};

struct Rack {
    struct Node *s[5];
};

struct Car {
    int weight;
    int f2;
    char f4;
    unsigned char f5;
    struct Node *slot6;
    struct Rack rack;
    struct Node *slot12;
    char b14[2];
    char b16[9];
    char f1f;
    signed char f20;
    char pad21;
    int f22;
    unsigned f24;    /* bits 8-12 paint shade (F_06B06), 13 roof chopped, 14 front and 15 rear bumper stripped */
    struct Car *f26;
};

struct Node2 {
    int a;
    int b;
    char pad[0x22];
    struct Node2 *next;
};

struct Item {
    int f0;
    char f2;
    char f3;
    char f4;
    char pad3;
    int f6;
};

union Word {
    int w;
    unsigned char b[2];
};

struct Tone {
    int f0;
    int f1;
    int f2;
    int f3;
    int f4;
};

struct Bitmap {
    int width;
    int height;
    unsigned planesize;
    char pad[0x1e];
    union FarPtr bits;
    union FarPtr alt;
    int pitch;
    signed char flags;
};

struct Cmd {
    signed char op;
    signed char sub;
    int a;
    int b;
};

struct Box {
    int w;
    int h;
    int x1;
    int y1;
    int x2;
    int y2;
    char pad[0x1c - 12];
};

struct Obj {
    int id;
    int f2;
    int f4;
    int f6;
    struct Bitmap far *img;
    void far *fc;
    struct Box box;
};

/* blit request handed to the b_78a2.. hooks (20 bytes). The first six ints are the same as
   struct Clip; F_10A00 reads only those. mx, my: origin in the mask bitmap (tandyblt_compose). */
struct Blt {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
    int fc;
    int fe;
    int mx;
    int my;
};

/* clipped rectangle (12 bytes): size, source origin, destination origin. F_10A00 takes struct Blt *
   but reads and writes only these first 12 bytes, so callers with a Clip local pass it cast. */
struct Clip {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
};

struct Colors {
    char c[5];
};

struct Style {
    struct Colors pal[7];
    struct Colors c23;
    struct Colors c28;
    struct Colors c2d;
    struct Colors c32;
    struct Colors colors;
    char pad2[5];
    struct Colors hilite;
    struct Colors popup;
    struct Colors bank;         /* fifth palette bank: DS:03EB / 043B, part of the 80-byte record */
};

struct Mask {
    char far *data;
    int pad;
    int h;
};

struct Rect {
    int x1;
    int y1;
    int x2;
    int y2;
};

struct Seq {
    int count;
    signed char dx;
    signed char dy;
};

/* one animation frame: 12 bytes, the ring links nxt[0..2] (F_195F3 builds g77bal[]) */
struct Frame {
    struct Bitmap far *pos;
    struct Bitmap *data;
    struct Frame *nxt[3];
};

struct Sprite {
    struct Frame *frame;
    struct Bitmap far *img;
    struct Bitmap far *save;
    struct Rect *rc[2];
    int x;
    int y;
    struct Seq *seq;
    struct Page *page;
};

/* one saved band of a screen page (8 bytes): where it sits in the scratch bitmap, its first row and height */
struct Band {
    char far *p;
    int y;
    int h;
};

/* one display page of the sprite engine (16 bytes): its bitmap, the clipped rectangle, the link to the next page */
struct Page {
    struct Bitmap far *bm;
    struct Rect r;
    int idx;
    struct Page *link;
};

struct Dev {
    union {
        int w;
        unsigned char b[2];
    } f0;
    int f2;
    int f4;
    char off;
    char dirty;
    int far *l8;
    int far *lc;
    int far *cur;
    int *f14;
    int *f16;
    int *f18;
    int *f1a;
};

struct Hot {
    int f0;
    int f2;
    int f4;
    int f6;
    int x1;
    int y1;
    int x2;
    int y2;
    int f10;
};

struct Win {
    int bg;
    int lines;
    int x;
    int y;
    int w;
    int h;
    int tx;
    int ty;
    int pad;
};

struct Racer {
    signed char f0;
    char f1;
    signed char f2;
    char f3;
    int f4;
    int f6;
    int f8;
    signed char fa;
    char fb;
    signed char fc;
    char fd;
    char fe;
    signed char ff;
    char f10;
    char pad11;
};

struct Spec {
    int f0;
    unsigned char f2;
    signed char next;
    unsigned f4;
    int f6;
    int f8;
};

/* one car model (10 bytes, w_8df0[], indexed by Car.f2): kind picks its sprites (F_28879); f2, f6, f8
   are nonzero when the roof can be chopped and the front and rear bumpers stripped (F_0711A) */
struct Model {
    signed char kind;
    char pad1;
    int f2;
    int f4;
    int f6;
    int f8;
};

struct Link {
    int nxt;
    int prv;
    long key;
    int f8;
    int fa;
    int fc;
    char pad[4];
};

struct Pair {
    int a;
    int b;
};


struct Dlg {
    int f0;
    int f2;
    struct Bitmap far *bm1;
    struct Bitmap far *bm2;
    int f0c;
    int f0e;
    int f10;
    int f12;
    int f14;
    int f16;
    int f18;
    int f1a;
};
struct KeyMap {
    char key;
    char val;
};



struct Glyph {
    unsigned char w;
    char pad[8];
};


struct Spot {
    int id;
    int x;
    int y;
};

/* one range of the engine curve: 8 bytes */
struct Range {
    int f0;
    int f2;
    int f4;
    int f6;
};

/* one gear: three ranges */
struct Gear {
    struct Range r[3];
};

/* drive train state; e_78e0 and g79e2g are two instances, 0xc2 bytes each */
struct Eng {
    int f0;
    int f2;
    int f4;
    char pad6[4];
    int fa;
    int fc;
    int fe;
    int f10;
    int f12;
    int f14;
    int f16;
    int f18;
    int f1a;
    int f1c;
    int f1e;
    int f20;
    int f22;
    int f24;
    int f26;
    int f28;
    int f2a;
    char pad2c[8];
    int f34[6];
    int f40;
    int f42;
    struct Gear gear[5];
    int fbc;
    int fbe;
    int fc0;
};

struct Sheet {
    int id;
    int cols;
    int n;
};

struct Btn {
    struct Bitmap far * near *src;
    int sx;
    int sy;
    int w;
    int h;
    int x;
    int y;
};

/* the free area around the pointer found by F_1412D (x1, x2: left and right limits; y1, y2: top
   and bottom) or, on a hit, the box of the hot spot */
struct Lims {
    int x1;
    int x2;
    int y1;
    int y2;
};

/* what g_6c34 points at (F_10872, F_10677): number of points, bitmap ids of the two sprites, bitmap
   height at +6 and a table of (x, y) signed-byte pairs at +8 */
struct Track {
    int n;
    int id1;
    int id2;
    int h;
    signed char *pt;
};

/* 16-byte blit packet of F_10872 (struct Blt without the last two words) */
struct Blt16 {
    int w;
    int h;
    int sx;
    int sy;
    int dx;
    int dy;
    int fc;
    int fe;
};

struct Offs {
    unsigned char x, y, w, h;
};

struct Panel {
    struct Offs r[5];
};

/* 12-byte table entry (g7682n[]), read as bytes by F_17BE6 */
struct Spr12 {
    unsigned char b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11;
};

/* 6-byte table entry (ap818c[]) */
struct Spr6 {
    unsigned char b0, b1, b2, b3, b4, b5;
};

/* 6-byte table entry (a784als[]): bitmap id, then the (dx, dy) offset it is drawn at */
struct Pos6 {
    int id;
    int dx;
    int dy;
};

/* one step of an animation table (g_521e): frame number, then the delay in ticks before the next */
struct Step {
    unsigned char frame;
    unsigned char delay;
};

struct Look {
    unsigned lo : 8;
    unsigned shade : 5;
    unsigned roof : 1;
    unsigned front : 1;
    unsigned rear : 1;
};

/* one candidate of F_09646: rating difference to the current car, an "already taken" flag, kind difference */
struct Cand {
    int dr;
    int taken;
    int dk;
};

/* DS:822A, the display state set by F_10BD9: the visible and drawing bitmaps, a scratch bitmap,
   the video mode (front->flags), and per-mode geometry. Members, not plain globals: /Ol never
   hoists r_vid.mode into a register (0x004F4, 0x10AFD). */
struct Video {
    struct Bitmap far *front;
    struct Bitmap far *back;
    struct Bitmap far *f8;
    int mode;
    int planes;     /* 4 on EGA (-2), else 1 */
    int shift;      /* log2 pixels per byte: 3, 2, 1 */
    int bpp;        /* bits per pixel: 1, 2, 4 */
    int mask;       /* ~(pixels per byte - 1): -8, -4, -2 */
    int f16;
};

/* DS:04FE, the pages of a list menu (ads_open fills it, ads_run pages through it): start[k] is the
   first entry of page k, n the page count, cur the current page, list the menu (count, then items). */
struct Pages {
    int start[12];
    int n;
    int cur;
    int *list;
};

/* mouse cursor state at DS:7aec (F_11EA8 sets it up, F_120FE/F_1220B move it). The four
   rectangle members compare as nested members (F_1220B), which plain globals don't reproduce. */
struct Cursor {
    struct Bitmap far *save;    /* background under the cursor */
    struct Clip clip;           /* last drawn strip; memset as 12 bytes */
    char far *shape;
    char far *mask;
    unsigned off;               /* shape offset, F_11E9C(n) */
    unsigned size;              /* bytes per shape */
    int n;                      /* shape number */
    char state;                 /* 0 hidden, 1 shown, <0 hide pending */
    struct Rect r;              /* last drawn extent, x2 < 0 when none */
};

/* one cell of the intro picture tables g_526a / g_52e2 (6 bytes): source origin in the sheet,
   size, and the on-screen position (x is biased by 0x50) */
struct Cell {
    unsigned char sx;
    unsigned char sy;
    unsigned char w;
    unsigned char h;
    unsigned char x;
    unsigned char y;
};

/* one glyph of the CGA text font (cga_draw_text; 91 glyphs for ' '..'z' from 0ED2:0004): a width in half pixels and 8 rows of bits */
struct CgaGlyph {
    unsigned w;
    unsigned rows[8];
};

/* DS:7D2E..7D3F: one communal; the code reads these as struct members */
struct State7D2E {
    int m7d2e;
    int m7d30;
    int m7d32;
    int m7d34;
    int m7d36;
    int m7d38;
    int m7d3a;
    int m7d3c;
    int m7d3e;
};

/* DS:7E8A..7E99: one communal; the code reads these as struct members */
struct State7E8A {
    int m7e8a;
    int m7e8c;
    int m7e8e;
    int m7e90;
    char m7e92[8];
};

/* DS:70C0..70DF: one communal; the code reads these as struct members */
struct State70C0 {
    int m70c0;
    int m70c2[6];
    int m70ce;
    int m70d0;
    int m70d2[6];
    int m70de;
};

/* DS:7EAA..7EC2: one COMMON block in the original (folded from 11 placeholder
   globals: the compiler budget of the original translation units requires fewer external symbols) */
struct State7EAA {
    long m7eaa;
    int m7eae;
    struct Car *m7eb0;
    struct Car *m7eb2;
    struct Node2 *m7eb4;
    int m7eb6;
    struct Node *m7eb8;
    struct Node *m7eba;
    int m7ebc;
    int m7ebe;
    int m7ec0;
};

/* DS:8272..8290: one COMMON block in the original (folded from 11 placeholder
   globals: the compiler budget of the original translation units requires fewer external symbols) */
struct State8272 {
    int m8272;
    int m8274;
    int m8276;
    int m8278;
    int m827a;
    int m827c;
    int m827e;
    int m8280;
    int m8282;
    struct Look m8284;
    int m8286;
    char pad8288[8];
};

#endif
