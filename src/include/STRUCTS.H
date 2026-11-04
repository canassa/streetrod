/* STRUCTS.H - shared struct types. Hand-written source: one definition per type. */
#ifndef STRUCTS_H
#define STRUCTS_H

struct Mouse {
    int dx;
    int dy;
    int accX;
    int accY;
    int shiftX;
    int shiftY;
    int left;
    int right;
    char present;
};

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
    signed char color;
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
    int width;
    int height;
    char pad4[2];
    unsigned char bitmapFlags;
    unsigned char pal[16];
    char pad17;
    long dataOffset;
    int packedSize;
    unsigned char vramPlane;
    char pad1f;
    union FarPtr ptr;
    long f24;
    long f28;
    int f2c;
    unsigned char f2e;
    char pad2f;
};

struct Node {
    int value;
    int wear;
    int idx;
    struct Node *next;
};

struct Rack {
    struct Node *parts[5];
};

struct Car {
    int value;
    int specIndex;
    char paintScheme;
    unsigned char rank;
    struct Node *transmission;
    struct Rack rack;
    struct Node *tires;
    char transmissionBolts[2];
    char engineBolts[9];
    char wireState;
    signed char timing;
    char pad21;
    int fuel;
    unsigned appearance;    /* bits 8-12 sticker (F_06B06), 13 roof chopped, 14 front and 15 rear bumper stripped */
    struct Car *next;
};

struct Node2 {
    int a;
    int b;
    char pad[0x22];
    struct Node2 *next;
};

struct Item {
    int value;
    char variant;
    char fitMask;
    char category;
    char pad3;
    int nameOffset;
};

union Word {
    int w;
    unsigned char b[2];
};

struct Tone {
    int phase;
    int waveform;
    int depth;
    int length;
    int delay;
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
    int sx;
    int sy;
    int dx;
    int dy;
    char pad[0x1c - 12];
};

struct Obj {
    int id;
    int scriptIndex;
    int pendingTimers;
    int f6;
    struct Bitmap far *img;
    void far *fc;
    struct Box box;
};

/* blit request handed to the b_78a2.. hooks (20 bytes). The first six ints are the same as
   struct Clip; dash_clip_blt reads only those. mx, my: origin in the mask bitmap (tandyblt_compose). */
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

/* clipped rectangle (12 bytes): size, source origin, destination origin. dash_clip_blt takes struct Blt *
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

/* one animation frame: 12 bytes, the ring links nxt[0..2] (sprite_setup builds g_frmTable[]) */
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
    struct Rect rect;
    int pageIndex;
    struct Page *link;
};

struct Dev {
    union {
        int w;
        unsigned char b[2];
    } divisor;
    int ticksLeft;
    int repeatCount;
    char active;
    char dirty;
    int far *song;
    int far *phraseStart;
    int far *cur;
    int *repeatTable;
    int *repeatCursor;
    int *toneTable;
    int *toneCursor;
};

struct Hot {
    int kind;
    int palIndex;
    int textOffset;
    int f6;
    int x1;
    int y1;
    int x2;
    int y2;
    int next;
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
    char shiftPenalty;
    signed char f2;
    char f3;
    int overlayPic;
    int nameOffset;
    int specIndex;
    signed char paintScheme;
    char status;
    signed char betRaces;
    char betWins;
    char bodyMods;
    signed char sticker;
    char lostCarOnce;
    char pad11;
};

struct Spec {
    int price;
    unsigned char rank;
    signed char next;
    unsigned parts;
    int nameOffset;
    int bodyPic;
};

/* one car model (10 bytes, w_8df0[], indexed by Car.specIndex): kind picks its sprites (view_load); roofPic, frontBumperPic, rearBumperPic
   are nonzero when the roof can be chopped and the front and rear bumpers stripped (F_0711A) */
struct Model {
    signed char kind;
    char pad1;
    int roofPic;
    int f4;
    int frontBumperPic;
    int rearBumperPic;
};

struct Link {
    int nxt;
    int prv;
    long deadline;
    int priority;
    int action;
    int argument;
    char pad[4];
};

struct Pair {
    int a;
    int b;
};


struct Dlg {
    int specIndex;
    int bodyMods;
    struct Bitmap far *bm1;
    struct Bitmap far *bm2;
    int f0c;
    int fillsLeft;
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
    char rows[8];
};


struct Spot {
    int id;
    int x;
    int y;
};

/* one range of the engine curve: 8 bytes */
struct Range {
    int minRpm;
    int maxRpm;
    int accelStep;
    int decelStep;
};

/* one gear: three ranges */
struct Gear {
    struct Range bands[3];
};

/* drive train state; g_driveEng and g_oppEngState are two instances, 0xc2 bytes each */
struct Eng {
    int speed;
    int steeringInput;
    int steering;
    char pad6[4];
    int tireFailureIndex;
    int tireSlip;
    int rpm;
    int minRpm;
    int maxRpm;
    int accelStep;
    int decelStep;
    int gearIndex;
    int shifterDirty;
    int gearCount;
    int shiftRpm;
    int automatic;
    int autoShiftRpm;
    int autoSelector;
    int f26;
    int f28;
    int f2a;
    char pad2c[8];
    int f34[6];
    int rpmPerSpeed;
    int rpmBand;
    struct Gear gear[5];
    int stopFlags;
    int fbe;
    int elapsedTicks;
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

/* the free area around the pointer found by menu_hit (x1, x2: left and right limits; y1, y2: top
   and bottom) or, on a hit, the box of the hot spot */
struct Lims {
    int x1;
    int x2;
    int y1;
    int y2;
};

/* what g_shifter points at (dash_draw_shifter, dash_init_shifter): number of points, bitmap ids of the two sprites, bitmap
   height at +6 and a table of (x, y) signed-byte pairs at +8 */
struct Track {
    int n;
    int id1;
    int id2;
    int h;
    signed char *pt;
};

/* 16-byte blit packet of dash_draw_shifter (struct Blt without the last two words) */
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

/* 12-byte table entry (g_layoutAutoSprArray[]), read as bytes by carpic_build */
struct Spr12 {
    unsigned char wheelX1, wheelX2, b2, b3, roofX, b5, b6, frontBumperY, rearBumperX, rearBumperY, stickerX, stickerY;
};

/* 6-byte table entry (g_pointsModelService[]) */
struct Spr6 {
    unsigned char fuelCapX, fuelCapY, rearGlassX, rearGlassY, frontGlassX, frontGlassY;
};

/* 6-byte table entry (a784als[]): bitmap id, then the (dx, dy) offset it is drawn at */
struct Pos6 {
    int id;
    int dx;
    int dy;
};

/* one step of an animation table (g_animSteps): frame number, then the delay in ticks before the next */
struct Step {
    unsigned char frame;
    unsigned char delay;
};

struct Look {
    unsigned lo : 8;
    unsigned sticker : 5;
    unsigned roof : 1;
    unsigned front : 1;
    unsigned rear : 1;
};

/* one candidate of build_rival_pool: rating difference to the current car, an "already taken" flag, kind difference */
struct Cand {
    int dr;
    int taken;
    int dk;
};

/* DS:822A, the display state set by video_init: the visible and drawing bitmaps, a scratch bitmap,
   the video mode (front->flags), and per-mode geometry. Members, not plain globals: /Ol never
   hoists r_vid.mode into a register (0x004F4, 0x10AFD). */
struct Video {
    struct Bitmap far *front;
    struct Bitmap far *back;
    struct Bitmap far *hoodBg;
    int mode;
    int planes;     /* 4 on EGA (-2), else 1 */
    int shift;      /* log2 pixels per byte: 3, 2, 1 */
    int bpp;        /* bits per pixel: 1, 2, 4 */
    int mask;       /* ~(pixels per byte - 1): -8, -4, -2 */
    int copyToFront;
};

/* DS:04FE, the pages of a list menu (ads_open fills it, ads_run pages through it): start[k] is the
   first entry of page k, n the page count, cur the current page, list the menu (count, then items). */
struct Pages {
    int start[12];
    int n;
    int cur;
    int *list;
};

/* mouse cursor state at DS:7aec (cursor_init sets it up, cursor_ctl/cursor_draw move it). The four
   rectangle members compare as nested members (cursor_draw), which plain globals don't reproduce. */
struct Cursor {
    struct Bitmap far *save;    /* background under the cursor */
    struct Clip clip;           /* last drawn strip; memset as 12 bytes */
    char far *shape;
    char far *mask;
    unsigned shapeOffset;               /* shape offset, cursor_shape_off(n) */
    unsigned shapeSize;              /* bytes per shape */
    int shapeIndex;                      /* shape number */
    char state;                 /* 0 hidden, 1 shown, <0 hide pending */
    struct Rect drawnRect;              /* last drawn extent, x2 < 0 when none */
};

/* one cell of the intro picture tables g_endCellsA / g_endCellsB (6 bytes): source origin in the sheet,
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
    int rightEdge;
    int roadIndex;
    int roadFraction;
    int distanceStep;
    int lateralX;
    int cameraY;
    int m7d3a;
    int m7d3c;
    int m7d3e;
};

/* DS:7E8A..7E99: one communal; the code reads these as struct members */
struct State7E8A {
    int roadIndex;
    int roadFraction;
    int distanceStep;
    int lateralX;
    char m7e92[8];
};

/* DS:70C0..70DF: one communal; the code reads these as struct members */
struct State70C0 {
    int leftEdge;
    int m70c2[6];
    int m70ce;
    int m70d0;
    int m70d2[6];
    int m70de;
};

/* DS:7EAA..7EC2: one COMMON block in the original (folded from 11 placeholder
   globals: the compiler budget of the original translation units requires fewer external symbols) */
struct State7EAA {
    long money;
    int m7eae;
    struct Car *currentCar;
    struct Car *otherCars;
    struct Node2 *freeCars;
    int carCount;
    struct Node *spareParts;
    struct Node *freeParts;
    int partCount;
    int adIndex;
    int raceType;
};

/* DS:8272..8290: one COMMON block in the original (folded from 11 placeholder
   globals: the compiler budget of the original translation units requires fewer external symbols) */
struct State8272 {
    int stickerX;
    int stickerY;
    int stickerSubX;
    int betType;
    int bodyPic;
    int paintScheme;
    int tireVariant;
    int overlayPic;
    int specIndex;
    struct Look appearance;
    int transmissionPic;
    char pad8288[8];
};

struct Hooks {
    int (far *copy)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
    int (far *shiftBlit)(struct Bitmap far *, struct Bitmap far *, struct Blt *);
    int (far *maskedBlit)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);
    void (far *drawText)(struct Bitmap far *, int, int, char *);
    void (far *cursorBlit)(char far *, char far *, char far *, char far *, int, int, int, int);
    void (far *copyPage)(char far *, char far *);
    void (far *recolorRegion)(struct Bitmap far *, int, int, int, int, int, int);
    void (far *blank)(void);
    void (far *show)(void);
    int (far *spriteStep)(struct Sprite *);
    void (far *reveal)(char far *, char far *);
    void (far *slideSprite)(int, int, int, int, int, struct Bitmap far *);
    void (far *drawLine)(int, int, int, int, int);
    void (far *fillSpan)(int, int, int, int);
    void (far *fillBelow)(int, int);
};


struct TextEnd {
    int x;
    int y;
};

struct StatusText {
    int messageX;
    int y;
    int messageWidth;
    int shown;
    int valueX;
};

struct BodyPoints {
    int hasRearGlass;
    int frontGlassX;
    int frontGlassY;
    int rearGlassX;
    int rearGlassY;
    int gasCapX;
    int gasCapY;
};


struct MirrorEdges {
    int buildingY[6];
    int prevBuildingY;
    int lastBuildingY;
    int rightX[6];
    int prevRightX;
    int lastRightX;
};

struct BuildingRows {
    int y[13];
    int prevY;
    int lastY;
};

struct ListDialog {
    int lineBase;
    int rows;
    int *list;
    int selection;
    struct Hot *header;
};

struct PartSelection {
    int wirePic;
    int slot;
    char unused[2];
    int subslot;
};

struct InputState {
    int any;
    int key;
    int unused4;
    int unused6;
    int unused8;
    int unusedA;
};

enum BetKind { BET_NONE, BET_10, BET_50, BET_25, BET_100, BET_CAR };

struct Wager {
    enum BetKind kind;
    int amount;
};
#endif
