/* DASH.H - declarations of what DASH.C defines. */

#ifndef DASH_H
#define DASH_H
#include "STRUCTS.H"

extern int g_02d2;                                      /* DS:02D2 */
extern char g_02d4[];                                   /* DS:02D4 */
extern struct Bitmap far *g_02e6;                       /* DS:02E6 */
extern struct Bitmap far *g_02ea;                       /* DS:02EA */
extern signed char g_02ee[24][2];                       /* DS:02EE */
extern unsigned char g_031e[][2];                       /* DS:031E */
extern int g_0350;                                      /* DS:0350 */
extern int g_0352[3];                                   /* DS:0352 */
extern signed char g_0358[11][2];                       /* DS:0358 */
extern struct Track g_036e[];                           /* DS:036E */
extern struct Pair g_038c[];                            /* DS:038C */
extern struct Bitmap far *bm7678;                       /* DS:7678 */
extern unsigned at767ct;                                /* DS:767C */
extern unsigned av787at;                                /* DS:787A */
extern int (far *b_78a2)(struct Bitmap far *, struct Bitmap far *, struct Blt *, int);/* DS:78A2 */
extern void (far *bl78be)(void);                        /* DS:78BE */
extern void (far *m_present)(void);                     /* DS:78C2 */
extern void (far *s_blit)(char far *, char far *);      /* DS:78CA */
extern void (far *a_78d2)(int, int, int, int, int);     /* DS:78D2 */
extern char gz8249;                                     /* DS:8249 */
extern struct Bitmap far *g82b4s;                       /* DS:82B4 */
extern char bw8aca;                                     /* DS:8ACA */
void F_0F4B7(int i, int *x1, int *y1, int *x2, int *y2);/* 0x0F4B7 */
void F_0F552(int i);                                    /* 0x0F552 */
void F_0F62E(int i, int *x1, int *y1, int *x2, int *y2);/* 0x0F62E */
void F_0F6C9(int v);                                    /* 0x0F6C9 */
int F_0F7A5(int cur, int old, int flag);                /* 0x0F7A5 */
void F_0F972(void);                                     /* 0x0F972 */
void far F_0FEAD(void);                                 /* 0x0FEAD */
void far F_0FEBF(void);                                 /* 0x0FEBF */
void F_0FED1(int n);                                    /* 0x0FED1 */
void F_0FF70(void);                                     /* 0x0FF70 */
void F_100E2(void);                                     /* 0x100E2 */
void F_1013D(int n);                                    /* 0x1013D */
void F_102D3(void);                                     /* 0x102D3 */
void F_10414(int a);                                    /* 0x10414 */
void F_104B0(int a, int b, struct Bitmap far *dst);     /* 0x104B0 */
void F_10677(int a, int b);                             /* 0x10677 */
void F_10872(struct Bitmap far *dst, struct Eng *e);    /* 0x10872 */
void F_109C0(char far *p, int c, unsigned n);           /* 0x109C0 */
int F_10A00(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *in, struct Blt *out);/* 0x10A00 */
void F_10AFD(int a);                                    /* 0x10AFD */
int F_10B48(char *s);                                   /* 0x10B48 */

#endif
