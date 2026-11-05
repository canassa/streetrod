/**
 * @file MENU.H
 * @brief Menu stacks, selection and scrolling lists.
 */

#ifndef MENU_H
#define MENU_H

#include "HOTSPOT.H"

struct Bitmap;

/** @brief Active scrolling list and its borrowed menu hotspots. */
struct ListDialog {
    int rowBaseAddress;     /**< Near address of the first contiguous row hotspot. */
    int rows;               /**< Number of visible row hotspots. */
    int *list;              /**< Borrowed item list, with its count in element zero. */
    int selection;          /**< One-based selected item in the entire list. */
    struct Hotspot *header; /**< Borrowed header hotspot, or zero for none. */
};
#include "KEYMAPS.H"

/**
 * @brief Screen region in which the cached cursor hover result remains valid.
 *
 * Holds either a hit hotspot's bounds or the surrounding free area on a miss.
 */
struct HoverBounds {
    int x1; /**< Included left pixel column. */
    int x2; /**< Excluded right pixel column. */
    int y1; /**< Included top pixel row. */
    int y2; /**< Excluded bottom pixel row. */
};

extern struct ListDialog g_menuList;

extern int g_cgaPicMap[];
extern int g_menuGrpHistoryList[];
extern char g_hideSelectFrameVal;
extern char g_headerEditableState;
extern struct Hotspot *g_hotsCurMenu;
extern int g_textOfsMenu;
extern int g_hotMenuGroupState;
void mn_draw_hot(struct Hotspot *p);
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
