/* MENU.H - declarations of what MENU.C defines. */

#ifndef MENU_H
#define MENU_H
#include "STRUCTS.H"

extern struct ListDialog g_menuList; /* DS:8B9A */

extern int g_hotChain;                                  /* DS:07A8 */
extern char g_wearText[];                               /* DS:07AA */
extern char g_wearPad[4];                               /* DS:07BE */
extern int g_cgaPicMap[];                                    /* DS:07C6 */
extern int g_menuGrpHistoryList[];                                   /* DS:8B86 */
extern char g_hideSelectFrameVal;                                     /* DS:8B98 */
extern char g_headerEditableState;                                    /* DS:8BA4 */
extern struct Hot *g_hotsCurMenu;                             /* DS:8BA6 */
extern int ai8bacx;                                     /* DS:8BAC */
extern int g_hotMenuGroupState;                                      /* DS:8EFE */
void mn_draw_hot(struct Hot *p);                      /* 0x139DE */
void mn_push(int id);                                 /* 0x13DE6 */
void mn_pop(int keep);                                /* 0x13F21 */
void mn_reset(void);                                  /* 0x13F7F */
void mn_bbox(int id, int *x, int *y, int *w, int *h); /* 0x13F9A */
struct Bitmap far *mn_grab(int id, int *px, int *py, int mode);/* 0x14090 */
int mn_hit(int x, int y);                             /* 0x1412D */
int mn_modal(int id);                                 /* 0x14241 */
int mn_wait(int a);                                   /* 0x14294 */
int mn_modal_list(int a, int b, int *c, int *list, char *buf, char **d);/* 0x142CF */
int mn_list(int a, int b, int *c, int *list, char *buf, char **d);/* 0x1438E */
int mn_sel(void);                                      /* 0x14C8B */

#endif
