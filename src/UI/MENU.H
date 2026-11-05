/* MENU.H - declarations of what MENU.C defines. */

#ifndef MENU_H
#define MENU_H

#include "HOTSPOT.H"

struct Bitmap;

struct ListDialog {
    int lineBase;
    int rows;
    int *list;
    int selection;
    struct Hot *header;
};
#include "KEYMAPS.H"

/* the free area around the pointer found by mn_hit (x1, x2: left and right limits; y1, y2: top
   and bottom) or, on a hit, the box of the hot spot */
struct Lims {
    int x1;
    int x2;
    int y1;
    int y2;
};

extern struct ListDialog g_menuList;

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
