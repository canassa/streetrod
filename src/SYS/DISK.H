/* DISK.H - declarations of what DISK.C defines. */

#ifndef DISK_H
#define DISK_H
#include "STRUCTS.H"

extern char *g_dataFileName;                            /* DS:4EA2 */
extern char *g_dataBlocks[13];                          /* DS:4EA4 */
extern unsigned g_dataSizes[13];                        /* DS:4EBE */
extern int g_lib1Handle;                                /* DS:4ED8 */
extern int g_lib2Handle;                                /* DS:4EDA */
extern char *g_lib1Name;                                /* DS:4EDC */
extern char *g_lib1cName;                               /* DS:4EDE */
extern char *g_lib2Name;                                /* DS:4EE0 */
extern char *g_lib2cName;                               /* DS:4EE2 */
extern char *g_msgInsertDisk;                           /* DS:4EE4 */
extern char *g_msgCopyFiles;                            /* DS:4EE6 */
extern char *g_msgBadFormat;                            /* DS:4EE8 */
extern char *g_msgWriteError;                           /* DS:4EEA */
extern char *g_msgNotFound;                             /* DS:4EEC */
extern unsigned char g_rleCounts[16];                   /* DS:4EEE */
extern unsigned char g_rleValues[16];                   /* DS:4EFE */
extern int g_picNewHeader;                              /* DS:4F0E */
extern char g_hallFileName[];                           /* DS:4F3A */
extern struct Panel ai70e4t[];                          /* DS:70E4 */
extern struct Spr12 g_layoutAutoSprArray[];                           /* DS:7682 */
extern struct Pos6 a784als[];                           /* DS:784A */
extern int g_usedCarAdFirst;                                     /* DS:7D16 */
extern unsigned g_carRatingsTable[][3];                            /* DS:7D68 */
extern struct Spec g7d86n[];                            /* DS:7D86 */
extern struct Spr6 g_pointsModelService[];                            /* DS:818C */
extern unsigned g_rivalStakeToleranceTable[][3];                           /* DS:8BAE */
extern int g_lib1Count;                                      /* DS:8BC8 */
extern int g_countLibBmB;                                      /* DS:8DD2 */
extern char *g_library1Path;                                   /* DS:8DD4 */
extern char *g_picLibBPath;                                   /* DS:8DD6 */
extern int g_acceptProbability[][3];                                 /* DS:8DD8 */
extern struct Model w_8df0[];                           /* DS:8DF0 */
int disk_seek(int handle, long pos);                    /* 0x15396 */
int disk_read_index(int n);                             /* 0x1552B */
void disk_free_pic(int n);                              /* 0x158DE */
struct Bitmap far *disk_unpack_pic(int id, struct Bitmap far *bm);/* 0x1592E */
void disk_pic_size(int id, int *w, int *h, int *x);     /* 0x15B7F */
struct Bitmap far *disk_bm(int id, int mode);           /* 0x15BBE */
int disk_cache_list(int *list);                         /* 0x15C07 */
int disk_load_list(int *list, int n);                   /* 0x15C4A */
void disk_free_list(int *list);                         /* 0x15C95 */
void disk_open_libs(int a, int mode);                   /* 0x15CC3 */
void disk_load_data(void);                              /* 0x15EF9 */
int disk_data_drive(void);                              /* 0x15FBD */
void disk_load_hall(void);                              /* 0x16089 */
void disk_save_hall(void);                              /* 0x16152 */

#endif
