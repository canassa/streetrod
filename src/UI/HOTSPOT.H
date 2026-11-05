/**
 * @file HOTSPOT.H
 * @brief Menu layouts and interactive screen regions.
 */

#ifndef HOTSPOT_H
#define HOTSPOT_H

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

/** @brief Screen headers and template indexes in @ref g_hots. */
enum HotspotIndex {
    HOTS_RACE_OUTCOME = 0,        /**< @ref KM_RACE_OUTCOME header. */
    HOTS_START_GAME = 2,          /**< @ref KM_START_GAME header. */
    HOTS_LOAD = 6,                /**< @ref KM_LOAD header. */
    HOTS_SPARE_PARTS = 26,        /**< @ref KM_SPARE_PARTS header. */
    HOTS_NONE = 41,               /**< Default entry, not a header. */
    HOTS_SAVE = 46,               /**< @ref KM_SAVE header. */
    HOTS_SAVE_ERROR = 67,         /**< @ref KM_SAVE_ERROR header. */
    HOTS_NEWSPAPER = 72,          /**< @ref KM_NEWSPAPER header. */
    HOTS_ADVERTS = 77,            /**< @ref KM_ADVERTS header. */
    HOTS_BUY = 108,               /**< @ref KM_BUY header. */
    HOTS_GARAGE = 115,            /**< @ref KM_GARAGE header. */
    HOTS_YOUR_CARS = 135,         /**< @ref KM_YOUR_CARS header. */
    HOTS_SELL_CAR_OFFER = 156,    /**< @ref KM_SELL_CAR_OFFER header. */
    HOTS_COUNTER_OFFER = 163,     /**< @ref KM_COUNTER_OFFER header. */
    HOTS_PAINT = 166,             /**< @ref KM_PAINT header. */
    HOTS_NO_SAVED_GAMES = 173,    /**< @ref KM_NO_SAVED_GAMES header. */
    HOTS_SELL_PART_OFFER = 178,   /**< @ref KM_SELL_PART_OFFER header. */
    HOTS_UNUSED_17 = 183,         /**< @ref KM_UNUSED_17 header. */
    HOTS_DRAG_WAGER = 185,        /**< @ref KM_DRAG_WAGER header. */
    HOTS_ROAD_WAGER = 191,        /**< @ref KM_ROAD_WAGER header. */
    HOTS_TRANSMISSION = 197,      /**< @ref KM_TRANSMISSION header. */
    HOTS_ENGINE = 203,            /**< @ref KM_ENGINE header. */
    HOTS_ENGINE_TEMPLATE = 218,   /**< Engine control templates. */
    HOTS_RACE_TYPE = 221,         /**< @ref KM_RACE_TYPE header. */
    HOTS_GAS_STATION = 226,       /**< @ref KM_GAS_STATION header. */
    HOTS_MESSAGE = 234,           /**< @ref KM_MESSAGE header. */
    HOTS_REPAIR = 237,            /**< @ref KM_REPAIR header. */
    HOTS_GET_RUNNING = 244,       /**< @ref KM_GET_RUNNING header. */
    HOTS_SAVE_RESTART_QUIT = 250, /**< @ref KM_SAVE_RESTART_QUIT header. */
    HOTS_TIMING = 258,            /**< @ref KM_TIMING header. */
    HOTS_MESSAGE_BOX = 263,       /**< @ref KM_MESSAGE_BOX header. */
    HOTS_OVERWRITE_SAVE = 268,    /**< @ref KM_OVERWRITE_SAVE header. */
    HOTS_LOAD_ERROR = 273,        /**< @ref KM_LOAD_ERROR header. */
    HOTS_READY = 278,             /**< @ref KM_READY header. */
    HOTS_RACE_STATS = 283,        /**< @ref KM_RACE_STATS header. */
    HOTS_BODY_CHANGE = 287,       /**< @ref KM_BODY_CHANGE header. */
    HOTS_STICKERS = 292,          /**< @ref KM_STICKERS header. */
    HOTS_CALL_RIVAL = 303,        /**< @ref KM_CALL_RIVAL header. */
    HOTS_BOBS = 322,              /**< @ref KM_BOBS header. */
    HOTS_MANUAL_COLORS = 329,     /**< @ref KM_MANUAL_COLORS header. */
    HOTS_UNUSED_40 = 342,         /**< @ref KM_UNUSED_40 header. */
    HOTS_END_MENU = 344,          /**< @ref KM_END_MENU header. */
    HOTS_CAR_INFO = 349,          /**< @ref KM_CAR_INFO header. */
    HOTS_COUNT = 365              /**< Number of records in @ref g_hots. */
};

/** @brief Screen records, mutable regions and engine control templates. */
extern struct Hotspot g_hots[HOTS_COUNT];
extern char g_unused596[2];
extern char g_unused696[6];
extern char g_unused1239[64];
extern char g_unused1449[2];
extern char g_unused34[2];

#endif
