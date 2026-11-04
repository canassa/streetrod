/* GARGFX.H - declarations of what GARGFX.C defines. */

#ifndef GARGFX_H
#define GARGFX_H
#include "STRUCTS.H"

extern struct PartSelection g_slotSelec; /* DS:82AC */

extern char g_partAnimate;                              /* DS:06FC */
extern int g_tboltY[];                                  /* DS:06FE */
extern int g_enginePos[][2];                            /* DS:0702 */
extern int g_engineBoltPos[];                           /* DS:070E */
extern int g_wiresPos[];                                /* DS:071A */
extern int g_manifoldBoltPos[];                         /* DS:0732 */
extern int g_carbPos[][2];                              /* DS:0742 */
extern int g_carbBoltOff[];                             /* DS:076A */
extern char g_partSlotMap[];                                   /* DS:077E */
extern char g_carbPosBase[];                                   /* DS:078C */
extern struct Bitmap far *g_boltOverlay;                       /* DS:0792 */
extern struct Hot g_boltHot;                            /* DS:0796 */
extern struct Blt g_cpyBoltPatchRecord;                               /* DS:70AC */
extern int g_partsPics[];                                    /* DS:8290 */
extern int g_engineBoltPicIds[9];                                   /* DS:829A */
void gx_trans_bolt(int a, int b);                             /* 0x123F4 */
void gx_trans_drop(int a);                                    /* 0x12525 */
void gx_trans_pic(int n, int flag);                          /* 0x1264E */
void gx_trans_clear(int n, int flag);                          /* 0x12784 */
void gx_trans_draw(int n, char far *s);                       /* 0x127DB */
void gx_part_pos(int *px, int *py);                 /* 0x12909 */
void gx_bolt_pos(int *px, int *py);                 /* 0x12964 */
void gx_draw_bolt(int n);                           /* 0x12A17 */
void gx_drop_bolt(void);                            /* 0x12B7C */
void gx_draw_part(int n, int flag);                 /* 0x12DD8 */
void gx_erase_part(int n, int flag);                /* 0x12F99 */
void gx_draw_wires(void);                           /* 0x1309C */
void gx_draw_parts(struct Car *car);                /* 0x131B5 */
void gx_draw_bolts(int *a, int *b, int flag);       /* 0x132AD */
void gx_build_hots(int *xs, int *ys);               /* 0x1344A */
void gx_sort_hots(int first, int n);                         /* 0x135E5 */

#endif
