/* MENU.H - declarations of what MENU.C defines. */

#ifndef MENU_H
#define MENU_H
#include "STRUCTS.H"
#include "KEYMAPS.H"

extern struct ListDialog g_menuList;

extern int g_hotChain;
extern char g_wearText[];
extern char g_wearPad[4];
extern int g_cgaPicMap[];
extern int g_menuGrpHistoryList[];
extern char g_hideSelectFrameVal;
extern char g_headerEditableState;
extern struct Hot *g_hotsCurMenu;
extern int g_textOfsMenu;
extern int g_hotMenuGroupState;
void mn_draw_hot(struct Hot *p);
void mn_push(enum ScreenId id);
void mn_pop(int keep);
void mn_reset(void);
void mn_bbox(enum ScreenId id, int *x, int *y, int *w, int *h);
struct Bitmap far *mn_grab(enum ScreenId id, int *px, int *py, int mode);
int mn_hit(int x, int y);
int mn_modal(enum ScreenId id);
int mn_wait(enum ScreenId a);
int mn_modal_list(enum ScreenId a, int b, int *c, int *list, char *buf, char **d);
int mn_list(enum ScreenId a, int b, int *c, int *list, char *buf, char **d);
int mn_sel(void);

#endif
