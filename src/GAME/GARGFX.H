/* GARGFX.H - declarations of what GARGFX.C defines. */

#ifndef GARGFX_H
#define GARGFX_H
#include "STRUCTS.H"

extern struct PartSelection g_slotSelec;

extern char g_partAnimate;
extern int g_tboltY[];
extern int g_enginePos[][2];
extern int g_engineBoltPos[];
extern int g_wiresPos[];
extern int g_manifoldBoltPos[];
extern int g_carbPos[][2];
extern int g_carbBoltOff[];
extern char g_partSlotMap[];
extern char g_carbPosBase[];
extern struct Bitmap far *g_boltOverlay;
extern struct Hot g_boltHot;
extern struct Blt g_cpyBoltPatchRecord;
extern int g_partsPics[];
extern int g_engineBoltPicIds[9];
void gx_trans_bolt(int a, int b);
void gx_trans_drop(int a);
void gx_trans_pic(int n, int flag);
void gx_trans_clear(int n, int flag);
void gx_trans_draw(int n, char far *s);
void gx_part_pos(int *px, int *py);
void gx_bolt_pos(int *px, int *py);
void gx_draw_bolt(int n);
void gx_drop_bolt(void);
void gx_draw_part(int n, int flag);
void gx_erase_part(int n, int flag);
void gx_draw_wires(void);
void gx_draw_parts(struct Car *car);
void gx_draw_bolts(int *a, int *b, int flag);
void gx_build_hots(int *xs, int *ys);
void gx_sort_hots(int first, int n);

#endif
