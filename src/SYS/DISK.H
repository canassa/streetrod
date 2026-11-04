/* DISK.H - declarations of what DISK.C defines. */

#ifndef DISK_H
#define DISK_H

struct Bitmap;

struct Spec {
    int price;
    unsigned char rank;
    signed char next;
    unsigned parts;
    int nameOffset;
    int bodyPic;
};

/* one car model (10 bytes, g_models[], indexed by Car.specIndex): kind picks its sprites (vw_load); roofPic, frontBumperPic, rearBumperPic
   are nonzero when the roof can be chopped and the front and rear bumpers stripped (ga_body) */
struct Model {
    signed char kind;
    char unused;
    int roofPic;
    int engineOverlayPic;
    int frontBumperPic;
    int rearBumperPic;
};

struct Offs {
    unsigned char x, y, w, h;
};

struct Panel {
    struct Offs r[5];
};

/* 12-byte table entry (g_layoutAutoSprArray[]), read as bytes by cp_build */
struct Spr12 {
    unsigned char wheelX1, wheelX2, b2, b3, roofX, b5, b6, frontBumperY, rearBumperX, rearBumperY, stickerX, stickerY;
};

/* 6-byte table entry (g_pointsModelService[]) */
struct Spr6 {
    unsigned char fuelCapX, fuelCapY, rearGlassX, rearGlassY, frontGlassX, frontGlassY;
};

/* 6-byte table entry (g_imageDecalLayout[]): bitmap id, then the (dx, dy) offset it is drawn at */
struct Pos6 {
    int id;
    int dx;
    int dy;
};
#include "MAIN.H"

struct PicLibSelection {
    int *count;
    int *handle;
    char *path;
    int *otherHandle;
};

extern char *g_dataFileName;
extern char *g_dataBlocks[13];
extern unsigned g_dataSizes[13];
extern int g_lib1Handle;
extern int g_lib2Handle;
extern char *g_lib1Name;
extern char *g_lib1cName;
extern char *g_lib2Name;
extern char *g_lib2cName;
extern char *g_msgInsertDisk;
extern char *g_msgCopyFiles;
extern char *g_msgBadFormat;
extern char *g_msgWriteError;
extern char *g_msgNotFound;
extern unsigned char g_rleCounts[16];
extern unsigned char g_rleValues[16];
extern int g_picNewHeader;
extern char g_hallFileName[];
extern struct Panel g_bodyRects[];
extern struct Spr12 g_layoutAutoSprArray[];
extern struct Pos6 g_imageDecalLayout[];
extern int g_usedCarAdFirst;
extern unsigned g_carRatingsTable[][3];
extern struct Spec g_carData[];
extern struct Spr6 g_pointsModelService[];
extern unsigned g_rivalStakeToleranceTable[][3];
extern int g_lib1Count;
extern int g_countLibBmB;
extern char *g_library1Path;
extern char *g_picLibBPath;
extern int g_acceptProbability[][3];
extern struct Model g_models[];
int di_seek(int handle, long pos);
int di_read_index(int n);
void di_free_pic(int n);
struct Bitmap far *di_unpack_pic(int id, struct Bitmap far *bm);
void di_pic_size(int id, int *w, int *h, int *x);
struct Bitmap far *di_bm(int id, int mode);
int di_cache_list(int *list);
int di_load_list(int *list, int n);
void di_free_list(int *list);
void di_open_libs(int a, enum VideoChoice mode);
void di_load_data(void);
int di_data_drive(void);
void di_load_hall(void);
void di_save_hall(void);

#endif
