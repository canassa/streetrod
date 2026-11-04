/* GARAGE.H - declarations of what GARAGE.C defines. */

#ifndef GARAGE_H
#define GARAGE_H

struct Car;
struct Node;

enum PartCategory {
    PART_ENGINE = 0,
    PART_TRANSMISSION = 1,
    PART_CARBURETOR = 2,
    PART_MANIFOLD = 3,
    PART_TIRES = 4
};

struct Look {
    unsigned lo : 8;
    unsigned sticker : 5;
    unsigned roof : 1;
    unsigned front : 1;
    unsigned rear : 1;
};

struct CarDisplay {
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
    char unused[8];
};

#define GARAGE_GAME_ENDED 0x29a

enum GarageMode {
    GARAGE_KEEP = 0,
    GARAGE_DRIVE_IN = 1,
    GARAGE_REDRAW = 2,
    GARAGE_SWAP = 3
};

extern int g_tirePics[][4];
extern int g_colX;
extern int g_rowY[];
extern int g_wOff[];
extern char *g_mon[];
extern int g_hIds[];
extern char g_paint[][2];
extern int g_pad[];
extern char *g_iTxt[];
extern int g_nCarb[];
extern int g_nBolt[];
extern enum PartCategory g_nextCat[];
extern int g_cPos[][6];
extern int g_boltSlots[10];
extern int g_boltSubslots[10];
extern struct CarDisplay g_carDraw;
int ga_day(void);
void ga_time(int *a, int *b);
void ga_check_end(int n);
void ga_calendar(int a);
void ga_date(int x);
void ga_palette(int a);
int ga_hotspot(int n);
void ga_clear_name(void);
int near ga_menu(enum GarageMode mode);
void near ga_paint(void);
void near ga_body(unsigned mask);
void near ga_stickers(void);
void near ga_spares(enum PartCategory t, struct Node **list, char *buf, int start);
void near ga_install(struct Node *n, struct Node **slot);
struct Node *near ga_unlink(struct Node *n);
void near ga_tires(void);
void ga_info(void);
void near ga_transmission(void);
int ga_ready(struct Car *car, int flag);
void near ga_hood(struct Car *car);

#endif
