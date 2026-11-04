/* GARGFX.H - declarations of what GARGFX.C defines. */

#ifndef GARGFX_H
#define GARGFX_H
#include "STRUCTS.H"

extern char g_partAnimate;                              /* DS:06FC */
extern int g_tboltY[];                                  /* DS:06FE */
extern int g_enginePos[][2];                            /* DS:0702 */
extern int g_engineBoltPos[];                           /* DS:070E */
extern int g_wiresPos[];                                /* DS:071A */
extern int g_manifoldBoltPos[];                         /* DS:0732 */
extern int g_carbPos[][2];                              /* DS:0742 */
extern int g_carbBoltOff[];                             /* DS:076A */
extern char g_077e[];                                   /* DS:077E */
extern char g_078c[];                                   /* DS:078C */
extern struct Bitmap far *g_0792;                       /* DS:0792 */
extern struct Hot g_boltHot;                            /* DS:0796 */
extern struct Blt u_70ac;                               /* DS:70AC */
extern int (far *am78a6t)(struct Bitmap far *, struct Bitmap far *, struct Blt *);/* DS:78A6 */
extern void (far *bl78ce)(int, int, int, int, int, struct Bitmap far *);/* DS:78CE */
extern int o_8290[];                                    /* DS:8290 */
extern int af829ax[];                                   /* DS:829A */
extern int ad829cy[];                                   /* DS:829C */
extern int ap82acz;                                     /* DS:82AC */
extern int hz82ae;                                      /* DS:82AE */
extern int g82b2t;                                      /* DS:82B2 */
void F_123F4(int a, int b);                             /* 0x123F4 */
void F_12525(int a);                                    /* 0x12525 */
void F_1264E(int n, int flag);                          /* 0x1264E */
void F_12784(int n, int flag);                          /* 0x12784 */
void F_127DB(int n, char far *s);                       /* 0x127DB */
void gargfx_part_pos(int *px, int *py);                 /* 0x12909 */
void gargfx_bolt_pos(int *px, int *py);                 /* 0x12964 */
void gargfx_draw_bolt(int n);                           /* 0x12A17 */
void gargfx_drop_bolt(void);                            /* 0x12B7C */
void gargfx_draw_part(int n, int flag);                 /* 0x12DD8 */
void gargfx_erase_part(int n, int flag);                /* 0x12F99 */
void gargfx_draw_wires(void);                           /* 0x1309C */
void gargfx_draw_parts(struct Car *car);                /* 0x131B5 */
void gargfx_draw_bolts(int *a, int *b, int flag);       /* 0x132AD */
void gargfx_build_hots(int *xs, int *ys);               /* 0x1344A */
void F_135E5(int first, int n);                         /* 0x135E5 */

#endif
