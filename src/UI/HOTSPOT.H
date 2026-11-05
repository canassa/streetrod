/**
 * @file HOTSPOT.H
 * @brief Menu layouts and interactive screen regions.
 */

#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "VIDEO.H"

/** @brief Menu drawing operations and hit-test record kinds. */
enum HotKind {
    HOT_UNSET = 0,           /**< Zero record; draws nothing. */
    HOT_MENU = 1,            /**< Header selecting a menu group. */
    HOT_LIST_MENU = 2,       /**< List group with a selection frame. */
    HOT_LIST_NO_FRAME = 3,   /**< List without a selection frame. */
    HOT_BOX = 4,             /**< Panel with caption and divider. */
    HOT_TEXT = 5,            /**< Label drawn at the rectangle origin. */
    HOT_CENTERED_TEXT = 6,   /**< Label centered within the rectangle. */
    HOT_EDIT_HEADER = 7,     /**< Editable left-aligned list header. */
    HOT_CENTERED_HEADER = 8, /**< Noneditable centered list header. */
    HOT_END = 9,             /**< End of a selectable record run. */
    HOT_BUTTON = 10,         /**< Beveled hit target with text. */
    HOT_REGION = 11,         /**< Hit target without drawing. */
    /** @ref Hotspot::hitCode links to another hit-test run. */
    HOT_LINK_BOX = 12,
    HOT_LIST_ROW = 13,              /**< Row counted toward list capacity. */
    HOT_DISABLED = 14,              /**< Record skipped during hit testing. */
    HOT_LAST_NONCLICKABLE = HOT_END /**< Larger kinds enter hit testing. */
};

/**
 * @brief One menu header, drawing element or interactive screen region.
 *
 * Drawing follows indexed links; hit testing scans consecutive records and
 * skips children of @ref HOT_LINK_BOX when the pointer lies outside its box.
 */
struct Hotspot {
    enum HotKind kind; /**< Drawing and hit-test role. */
    /** Drawing scheme; @ref STYLE_NONE suppresses box drawing. */
    enum StyleIndex styleIndex;
    int textOffset; /**< Text/edit-buffer byte offset in UI text storage. */
    /** Hit result, or a @ref g_hots skip index for @ref HOT_LINK_BOX. */
    int hitCode;
    int x1;       /**< Left screen-pixel coordinate. */
    int y1;       /**< Top screen row; hits extend three rows above it. */
    int x2;       /**< Right screen-pixel limit, exclusive for hits. */
    int y2;       /**< Bottom screen-row limit, exclusive for hits. */
    int drawNext; /**< Next drawing index in @ref g_hots, or zero. */
};

/** @brief Base of the shared hotspot index space. */
extern struct Hotspot g_hots[];
extern struct Hotspot g_hotPageRows[];
extern struct Hotspot g_hotBuy[];
extern struct Hotspot g_gar[];
extern struct Hotspot g_hotCalendar[];
extern struct Hotspot g_transBolts[];
extern struct Hotspot g_engineHots[];
extern struct Hotspot g_engineHotTemplate[];
extern struct Hotspot g_hotGasStation[];
extern struct Hotspot g_hotGasExit[];
extern struct Hotspot g_hotRepair[];
extern struct Hotspot g_hotReadyBox[];
extern struct Hotspot g_bodyAsk[];
extern struct Hotspot g_hotBobs[];
extern char g_unused596[2];
extern char g_unused696[6];
extern char g_unused1239[64];
extern char g_unused1449[2];
extern char g_unused34[2];

#endif
