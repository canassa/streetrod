/**
 * @file HOTSPOT.H
 * @brief Menu drawing records and selectable regions.
 */

#ifndef HOTSPOT_H
#define HOTSPOT_H

/** @brief Menu drawing operations and hit-test record kinds. */
enum HotKind {
    HOT_UNSET = 0,                  /**< Zero record; draws nothing. */
    HOT_MENU = 1,                   /**< Header selecting a menu group. */
    HOT_LIST_MENU = 2,              /**< List group with a selection frame. */
    HOT_LIST_NO_FRAME = 3,          /**< List without a selection frame. */
    HOT_BOX = 4,                    /**< Panel with caption and divider. */
    HOT_TEXT = 5,                   /**< Label drawn at the rectangle origin. */
    HOT_CENTERED_TEXT = 6,          /**< Label centered within the rectangle. */
    HOT_EDIT_HEADER = 7,            /**< Editable left-aligned list header. */
    HOT_CENTERED_HEADER = 8,        /**< Noneditable centered list header. */
    HOT_END = 9,                    /**< End of a selectable record run. */
    HOT_BUTTON = 10,                /**< Beveled hit target with text. */
    HOT_REGION = 11,                /**< Hit target without drawing. */
    HOT_LINK_BOX = 12,              /**< ID links to another hit-test run. */
    HOT_LIST_ROW = 13,              /**< Row counted toward list capacity. */
    HOT_DISABLED = 14,              /**< Record skipped during hit testing. */
    HOT_LAST_NONCLICKABLE = HOT_END /**< Larger kinds enter hit testing. */
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
extern char g_unused596[2];
extern char g_unused696[6];
extern char g_unused1239[64];
extern char g_unused1449[2];
extern char g_unused34[2];

#endif
