/* M_00D58.H - declarations of what M_00D58.C defines. */

#ifndef M_00D58_H
#define M_00D58_H
#include "STRUCTS.H"

extern int g_0566;                                      /* DS:0566 */
extern int g_0568;                                      /* DS:0568 */
extern int g_056a;                                      /* DS:056A */
extern long g_056c;                                     /* DS:056C */
extern char g_0570;                                     /* DS:0570 */
extern char g_0571;                                     /* DS:0571 */
extern struct Link g_0572[];                            /* DS:0572 */
extern int g_05ba;                                      /* DS:05BA */
extern int g_05bc[8];                                   /* DS:05BC */
extern long g_05cc;                                     /* DS:05CC */
extern int g_05d0;                                      /* DS:05D0 */
extern int g_05d2;                                      /* DS:05D2 */
extern int g_05d4;                                      /* DS:05D4 */
extern int g_05d6;                                      /* DS:05D6 */
extern int g_05d8;                                      /* DS:05D8 */
extern int g_05da;                                      /* DS:05DA */
extern int g_05dc;                                      /* DS:05DC */
extern int g_05de;                                      /* DS:05DE */
extern int g_05e0;                                      /* DS:05E0 */
extern int g_05e2;                                      /* DS:05E2 */
extern int g_05e4;                                      /* DS:05E4 */
extern int g_05e6;                                      /* DS:05E6 */
extern int g_05e8;                                      /* DS:05E8 */
extern int g_05ea;                                      /* DS:05EA */
extern int g_05ec;                                      /* DS:05EC */
extern int g_05ee;                                      /* DS:05EE */
extern int g_05f0;                                      /* DS:05F0 */
extern char g_05f2;                                     /* DS:05F2 */
extern char g_05f3;                                     /* DS:05F3 */
extern int g_05f4[2];                                   /* DS:05F4 */
extern long g_tick;                                     /* DS:05F8 */
extern long g_05fc;                                     /* DS:05FC */
extern long g_0600;                                     /* DS:0600 */
extern long g_0604;                                     /* DS:0604 */
extern char g_0608;                                     /* DS:0608 */
extern char g_0609;                                     /* DS:0609 */
extern int g_060a;                                      /* DS:060A */
extern char *g_060c;                                    /* DS:060C */
extern char *g_060e;                                    /* DS:060E */
extern int g_0610;                                      /* DS:0610 */
extern int g_0612;                                      /* DS:0612 */
extern char g_064d;                                     /* DS:064D */
extern char g_064e;                                     /* DS:064E */
extern struct KeyMap g_0650[];                          /* DS:0650 */
extern unsigned char g_0692[];                          /* DS:0692 */
extern int g_06a2;                                      /* DS:06A2 */
extern int g_06a4;                                      /* DS:06A4 */
extern char g_06a6;                                     /* DS:06A6 */
extern char g_06a7;                                     /* DS:06A7 */
extern int g_06a8;                                      /* DS:06A8 */
extern int g_06aa;                                      /* DS:06AA */
extern int g_06ac;                                      /* DS:06AC */
extern int g_06ae;                                      /* DS:06AE */
extern int g_06b0;                                      /* DS:06B0 */
extern int g_06b2;                                      /* DS:06B2 */
extern int g_06b4[];                                    /* DS:06B4 */
extern char g_06e0;                                     /* DS:06E0 */
extern int g_06e2;                                      /* DS:06E2 */
extern int g_06e4;                                      /* DS:06E4 */
extern int g_06e6;                                      /* DS:06E6 */
extern char g_06e8;                                     /* DS:06E8 */
extern char g_06e9;                                     /* DS:06E9 */
extern char g_06ea;                                     /* DS:06EA */
extern int t_72ec;                                      /* DS:72EC */
extern struct Eng e_78e0;                               /* DS:78E0 */
extern struct Dev bs79aa;                               /* DS:79AA */
extern struct Dev ao79c6t;                              /* DS:79C6 */
extern void (interrupt far *w_7aaa)(void);              /* DS:7AAA */
extern struct State7D2E ao7d2ev;                        /* DS:7D2E */
extern struct State7E8A ar7e8av;                        /* DS:7E8A */
extern char g7f32s[];                                   /* DS:7F32 */
extern int ao8228u;                                     /* DS:8228 */
extern long b_82bc;                                     /* DS:82BC */
extern struct Tone *bn82c4;                             /* DS:82C4 */
extern int z_8b7c;                                      /* DS:8B7C */
extern int g8b8eb;                                      /* DS:8B8E */
extern struct Lims ao8b90v;                             /* DS:8B90 */
extern struct Hot *at8ba8v;                             /* DS:8BA8 */
extern int az8baa;                                      /* DS:8BAA */
extern int cz8bc0y;                                     /* DS:8BC0 */
extern int g8ef6q;                                      /* DS:8EF6 */
extern int g_8ef8;                                      /* DS:8EF8 */
extern int aj8efax;                                     /* DS:8EFA */
extern int bw8efc;                                      /* DS:8EFC */
void F_00D78(void);                                     /* 0x00D78 */
void F_00D99(void);                                     /* 0x00D99 */
void F_00DD8(void);                                     /* 0x00DD8 */
void F_00E24(void);                                     /* 0x00E24 */
void F_00F0B(int n);                                    /* 0x00F0B */
void F_00FB7(void);                                     /* 0x00FB7 */
void F_00FEA(void);                                     /* 0x00FEA */
void F_0105B(unsigned deverr, unsigned char errcode);   /* 0x0105B */
void F_01111(void);                                     /* 0x01111 */
void F_012DE(int n);                                    /* 0x012DE */
int F_01391(int keep);                                  /* 0x01391 */
int F_01417(int n);                                     /* 0x01417 */
int F_01843(int delay, int pri);                        /* 0x01843 */
void F_0198A(int fc, int delay);                        /* 0x0198A */
void F_019BC(int n);                                    /* 0x019BC */
void F_01A05(int n, int cnt);                           /* 0x01A05 */
void F_01A7C(void);                                     /* 0x01A7C */
void F_01AAD(void);                                     /* 0x01AAD */
void F_01B27(void);                                     /* 0x01B27 */
int F_01B91(int x);                                     /* 0x01B91 */
void F_01BAE(void);                                     /* 0x01BAE */
void interrupt far F_01FE2(void);                       /* 0x01FE2 */
void F_02374(void);                                     /* 0x02374 */
void interrupt far F_0238C(void);                       /* 0x0238C */

#endif
