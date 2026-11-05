/**
 * @file HOTSPOT.H
 * @brief Menu layouts and interactive screen regions.
 */

#ifndef HOTSPOT_H
#define HOTSPOT_H

/** @brief Rendering and hit-test role of a hotspot record. */
enum HotKind {
    HOT_UNSET = 0,                  /**< Unset record; stops hit scanning. */
    HOT_MENU = 1,                   /**< Menu header and drawing-chain root. */
    HOT_LIST_MENU = 2,              /**< List menu header and drawing root. */
    HOT_LIST_NO_FRAME = 3,          /**< List header hiding selection frame. */
    HOT_BOX = 4,                    /**< Box with optional title and bevel. */
    HOT_TEXT = 5,                   /**< Text drawn at the top left. */
    HOT_CENTERED_TEXT = 6,          /**< Text centered within the rectangle. */
    HOT_EDIT_HEADER = 7,            /**< Editable, left-aligned list header. */
    HOT_CENTERED_HEADER = 8,        /**< Centered list header. */
    HOT_END = 9,                    /**< End of a contiguous hit-test list. */
    HOT_BUTTON = 10,                /**< Drawn button returning a hit code. */
    HOT_REGION = 11,                /**< Undrawn region returning a hit code. */
    HOT_LINK_BOX = 12,              /**< Box with a hit-test skip index. */
    HOT_LIST_ROW = 13,              /**< List row with a one-based hit code. */
    HOT_DISABLED = 14,              /**< Skipped by drawing and hit testing. */
    HOT_LAST_NONCLICKABLE = HOT_END /**< Last kind ending hit scanning. */
};

/**
 * @brief One menu header, drawing element or interactive screen region.
 *
 * Drawing follows indexed links; hit testing scans consecutive records and
 * skips children of @ref HOT_LINK_BOX when the pointer lies outside its box.
 */
struct Hotspot {
    enum HotKind kind; /**< Drawing and hit-test role. */
    int styleIndex;    /**< Drawing-style bank; zero suppresses box drawing. */
    int textOffset;    /**< Text/edit-buffer byte offset in UI text storage. */
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
