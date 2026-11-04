/* RACE.H - declarations of what RACE.C defines. */

#ifndef RACE_H
#define RACE_H
#include "STRUCTS.H"

extern int g_53b6[];                                    /* DS:53B6 */
extern int g_53c8[];                                    /* DS:53C8 */
extern struct Spot g_53d4[];                            /* DS:53D4 */
extern int g_53e6;                                      /* DS:53E6 */
extern int g_54b2[][5];                                 /* DS:54B2 */
extern int g_54d0;                                      /* DS:54D0 */
extern int g_54d2;                                      /* DS:54D2 */
extern int g_54d4;                                      /* DS:54D4 */
extern int g_54d6;                                      /* DS:54D6 */
extern int g_54d8;                                      /* DS:54D8 */
extern int g_54da;                                      /* DS:54DA */
extern struct State70C0 al70c0x;                        /* DS:70C0 */
extern int by72f2;                                      /* DS:72F2 */
extern struct Bitmap far *ab7526x[13];                  /* DS:7526 */
extern int g755av;                                      /* DS:755A */
extern int g7644m;                                      /* DS:7644 */
extern char at787cu;                                    /* DS:787C */
extern int ap7d7cv;                                     /* DS:7D7C */
extern int gz7d7e;                                      /* DS:7D7E */
extern int g7d82p;                                      /* DS:7D82 */
extern int a_7f88d;                                     /* DS:7F88 */
extern int h_8264;                                      /* DS:8264 */
extern int g826af;                                      /* DS:826A */
extern int ae826cv;                                     /* DS:826C */
extern int ar826e;                                      /* DS:826E */
extern int q_8270;                                      /* DS:8270 */
extern int bp82c2;                                      /* DS:82C2 */
extern int dz82c8x;                                     /* DS:82C8 */
extern int bz8630z;                                     /* DS:8630 */
extern int s_8632;                                      /* DS:8632 */
extern int g_8648;                                      /* DS:8648 */
extern struct Bitmap far *g8acew[13];                   /* DS:8ACE */
extern int ay8bc2z;                                     /* DS:8BC2 */
extern float p_8bc4;                                    /* DS:8BC4 */
extern int cz8bd0y;                                     /* DS:8BD0 */
void F_0B8A1(void);                                     /* 0x0B8A1 */
void F_0C407(void);                                     /* 0x0C407 */
int near F_0C613(void);                                 /* 0x0C613 */
void near F_0CDAF(struct Eng *p);                       /* 0x0CDAF */
void near F_0D031(void);                                /* 0x0D031 */
int F_0D544(int rpm);                                   /* 0x0D544 */
void F_0DA25(void);                                     /* 0x0DA25 */

#endif
