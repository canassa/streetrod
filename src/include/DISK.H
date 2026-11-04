/* DISK.H - declarations of what DISK.C defines. */

#ifndef DISK_H
#define DISK_H
#include "STRUCTS.H"

extern char *g_4ea2;                                    /* DS:4EA2 */
extern char *g_4ea4[13];                                /* DS:4EA4 */
extern unsigned g_4ebe[13];                             /* DS:4EBE */
extern int g_4ed8;                                      /* DS:4ED8 */
extern int g_4eda;                                      /* DS:4EDA */
extern char *g_4edc;                                    /* DS:4EDC */
extern char *g_4ede;                                    /* DS:4EDE */
extern char *g_4ee0;                                    /* DS:4EE0 */
extern char *g_4ee2;                                    /* DS:4EE2 */
extern char *g_4ee4;                                    /* DS:4EE4 */
extern char *g_4ee6;                                    /* DS:4EE6 */
extern char *g_4ee8;                                    /* DS:4EE8 */
extern char *g_4eea;                                    /* DS:4EEA */
extern char *g_4eec;                                    /* DS:4EEC */
extern unsigned char g_4eee[16];                        /* DS:4EEE */
extern unsigned char g_4efe[16];                        /* DS:4EFE */
extern int g_4f0e;                                      /* DS:4F0E */
extern char g_4f3a[];                                   /* DS:4F3A */
extern struct Panel ai70e4t[];                          /* DS:70E4 */
extern struct Spr12 g7682n[];                           /* DS:7682 */
extern struct Pos6 a784als[];                           /* DS:784A */
extern int a_7d16q;                                     /* DS:7D16 */
extern unsigned g7d68m[][3];                            /* DS:7D68 */
extern struct Spec g7d86n[];                            /* DS:7D86 */
extern struct Racer av7ff8[];                           /* DS:7FF8 */
extern struct Spr6 ap818c[];                            /* DS:818C */
extern unsigned ag8baey[][3];                           /* DS:8BAE */
extern int h_8bc8;                                      /* DS:8BC8 */
extern int s_8dd2;                                      /* DS:8DD2 */
extern char *aw8dd4x;                                   /* DS:8DD4 */
extern char *au8dd6y;                                   /* DS:8DD6 */
extern int h_8dd8[][3];                                 /* DS:8DD8 */
extern struct Model w_8df0[];                           /* DS:8DF0 */
int F_15396(int handle, long pos);                      /* 0x15396 */
int F_1552B(int n);                                     /* 0x1552B */
void F_158DE(int n);                                    /* 0x158DE */
struct Bitmap far *F_1592E(int id, struct Bitmap far *bm);/* 0x1592E */
void F_15B7F(int id, int *w, int *h, int *x);           /* 0x15B7F */
struct Bitmap far *F_15BBE(int id, int mode);           /* 0x15BBE */
int F_15C07(int *list);                                 /* 0x15C07 */
int F_15C4A(int *list, int n);                          /* 0x15C4A */
void F_15C95(int *list);                                /* 0x15C95 */
void F_15CC3(int a, int mode);                          /* 0x15CC3 */
void F_15EF9(void);                                     /* 0x15EF9 */
int F_15FBD(void);                                      /* 0x15FBD */
void F_16089(void);                                     /* 0x16089 */
void F_16152(void);                                     /* 0x16152 */

#endif
