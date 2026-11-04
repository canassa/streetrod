#ifndef KEYMAPS_H
#define KEYMAPS_H
#include "STRUCTS.H"

enum KeyMapId {
    KM_NONE = 0,
    KM_RACE = 1,
    KM_START_GAME = 2,
    KM_LOAD = 3,
    KM_SPARE_PARTS = 4,
    KM_SAVE = 5,
    KM_SAVE_ERROR = 6,
    KM_NEWSPAPER = 7,
    KM_ADVERTS = 8,
    KM_BUY = 9,
    KM_GARAGE = 10,
    KM_YOUR_CARS = 11,
    KM_SELL_CAR_OFFER = 12,
    KM_COUNTER_OFFER = 13,
    KM_PAINT = 14,
    KM_NO_SAVED_GAMES = 15,
    KM_SELL_PART_OFFER = 16,
    KM_DRAG_WAGER = 18,
    KM_ROAD_WAGER = 19,
    KM_TRANSMISSION = 20,
    KM_ENGINE = 21,
    KM_RACE_TYPE = 22,
    KM_GAS_STATION = 23,
    KM_MESSAGE = 24,
    KM_REPAIR = 25,
    KM_GET_RUNNING = 26,
    KM_SAVE_RESTART_QUIT = 27,
    KM_TIMING = 28,
    KM_MESSAGE_BOX = 29,
    KM_OVERWRITE_SAVE = 30,
    KM_LOAD_ERROR = 31,
    KM_READY = 32,
    KM_RACE_STATS = 33,
    KM_BODY_CHANGE = 34,
    KM_STICKERS = 35,
    KM_CALL_RIVAL = 36,
    KM_BOBS = 37,
    KM_CAR_INFO = 38,
    KM_MANUAL_COLORS = 39,
    KM_END_MENU = 41,
    KM_MUSIC = 42
};

enum KeyValue {
    KV_REPAIR_FIX = 0xcf,
    KV_REPAIR_JUNK = 0xd0,
    KV_MUSIC = 0xa6,
    KV_MESSAGE_DISMISS = 0xb4,
    KV_SEE_IT = 0xb5,
    KV_OFFER = 0xb6,
    KV_FORGET = 0xb7,
    KV_NEVER_MIND = 0xb8,
    KV_NO = 0xb9,
    KV_OK = 0xba,
    KV_SCROLL_DOWN = 0xc4,
    KV_SCROLL_UP = 0xc7,
    KV_CANCEL = 0xca,
    KV_ACCEPT = 0xcb,
    KV_RETURN_MINUS_2 = 0xcc,
    KV_RETURN_MINUS_1 = 0xcd
};

#define KV_ITEM(n) (0xce + (n))

extern unsigned char g_keyMapData[543];                       /* DS:5580 */
extern struct KeyMap *g_keyMaps[];                      /* DS:579F */

#endif
