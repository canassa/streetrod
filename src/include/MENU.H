/* MENU.H - declarations of what MENU.C defines. */

#ifndef MENU_H
#define MENU_H
#include "STRUCTS.H"

extern int g_hotChain;                                  /* DS:07A8 */
extern char g_wearText[];                               /* DS:07AA */
extern char g_wearPad[4];                               /* DS:07BE */
extern int g_cgaPicMap[];                                    /* DS:07C6 */
extern void (far *g_drvRecolorRegion)(struct Bitmap far *, int, int, int, int, int, int);/* DS:78BA */
extern int bq824e;                                      /* DS:824E */
extern int ai8b86y[];                                   /* DS:8B86 */
extern char av8b98;                                     /* DS:8B98 */
extern int a_8b9ap;                                     /* DS:8B9A */
extern int a_8b9ca;                                     /* DS:8B9C */
extern int *az8b9ez;                                    /* DS:8B9E */
extern int x_8ba0;                                      /* DS:8BA0 */
extern struct Hot *az8ba2t;                             /* DS:8BA2 */
extern char g_headerEditableState;                                    /* DS:8BA4 */
extern struct Hot *av8ba6v;                             /* DS:8BA6 */
extern int ai8bacx;                                     /* DS:8BAC */
extern int g_hotMenuGroupState;                                      /* DS:8EFE */
void menu_draw_hot(struct Hot *p);                      /* 0x139DE */
void menu_push(int id);                                 /* 0x13DE6 */
void menu_pop(int keep);                                /* 0x13F21 */
void menu_reset(void);                                  /* 0x13F7F */
void menu_bbox(int id, int *x, int *y, int *w, int *h); /* 0x13F9A */
struct Bitmap far *menu_grab(int id, int *px, int *py, int mode);/* 0x14090 */
int menu_hit(int x, int y);                             /* 0x1412D */
int menu_modal(int id);                                 /* 0x14241 */
int menu_wait(int a);                                   /* 0x14294 */
int F_142CF(int a, int b, int *c, int *list, char *buf, char **d);/* 0x142CF */
int menu_list(int a, int b, int *c, int *list, char *buf, char **d);/* 0x1438E */
int F_14C8B(void);                                      /* 0x14C8B */

#endif
