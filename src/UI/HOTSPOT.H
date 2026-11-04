/* HOTSPOT.H - declarations of what HOTSPOT.C defines. */

#ifndef HOTSPOT_H
#define HOTSPOT_H

enum HotKind {
    HOT_UNSET = 0,
    HOT_MENU = 1,
    HOT_LIST_MENU = 2,
    HOT_LIST_NO_FRAME = 3,
    HOT_BOX = 4,
    HOT_TEXT = 5,
    HOT_CENTERED_TEXT = 6,
    HOT_EDIT_HEADER = 7,
    HOT_CENTERED_HEADER = 8,
    HOT_END = 9,
    HOT_BUTTON = 10,
    HOT_REGION = 11,
    HOT_LINK_BOX = 12,
    HOT_LIST_ROW = 13,
    HOT_DISABLED = 14,
    HOT_LAST_NONCLICKABLE = HOT_END
};

struct Hot {
    enum HotKind kind;
    int palIndex;
    int textOffset;
    int id;
    int x1;
    int y1;
    int x2;
    int y2;
    int next;
};

extern struct Hot g_hots[];
extern struct Hot g_hotPageRows[];
extern struct Hot g_hotBuy[];
extern struct Hot g_gar[];
extern struct Hot g_hotCalendar[];
extern struct Hot g_transBolts[];
extern struct Hot g_engineHots[];
extern struct Hot g_engineHotTemplate[];
extern struct Hot g_hotGasStation[];
extern struct Hot g_hotGasExit[];
extern struct Hot g_hotRepair[];
extern struct Hot g_hotReadyBox[];
extern struct Hot g_bodyAsk[];
extern struct Hot g_hotBobs[];
extern char fz79a8[2];
extern char ah7aa4w[6];
extern char bz7ee2[64];
extern char g7ff6f[2];
extern char a8ef4fv[2];

#endif
