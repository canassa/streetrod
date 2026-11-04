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
void dash_speedo_pts(int i, int *x1, int *y1, int *x2, int *y2);/* 0x0F4B7 */
void dash_speedo_init(int i);                           /* 0x0F552 */
void dash_tach_pts(int i, int *x1, int *y1, int *x2, int *y2);/* 0x0F62E */
void dash_tach_init(int v);                             /* 0x0F6C9 */
int dash_move_needle(int cur, int old, int flag);       /* 0x0F7A5 */
void dash_draw_clock(void);                             /* 0x0F972 */
void far dash_pal_blank(void);                          /* 0x0FEAD */
void far dash_pal_show(void);                           /* 0x0FEBF */
void dash_wait_key(int n);                              /* 0x0FED1 */
void dash_intro(void);                                  /* 0x0FF70 */
void dash_time_up(void);                                /* 0x100E2 */
void dash_set_view(int n);                              /* 0x1013D */
void dash_load_wheel(void);                             /* 0x102D3 */
void dash_steer_marker(int a);                          /* 0x10414 */
void dash_draw_wheel(int a, int b, struct Bitmap far *dst);/* 0x104B0 */
void dash_init_shifter(int a, int b);                   /* 0x10677 */
void dash_draw_shifter(struct Bitmap far *dst, struct Eng *e);/* 0x10872 */
void dash_fmemset(char far *p, int c, unsigned n);      /* 0x109C0 */
int dash_clip_blt(struct Bitmap far *dst, struct Bitmap far *src, struct Blt *in, struct Blt *out);/* 0x10A00 */
void F_10AFD(int a);                                    /* 0x10AFD */
int dash_text_width(char *s);                           /* 0x10B48 */

#endif
