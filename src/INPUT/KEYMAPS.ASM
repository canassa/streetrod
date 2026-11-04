        .MODEL MEDIUM
        INCLUDE PC.INC

KEY_VALUE_BASE             EQU 0CEh
KR_MUSIC                   EQU -40
KR_MESSAGE_DISMISS         EQU -26
KR_SEE_IT                  EQU -25
KR_OFFER                   EQU -24
KR_FORGET                  EQU -23
KR_NEVER_MIND              EQU -22
KR_NO                      EQU -21
KR_OK                      EQU -20
KR_SCROLL_DOWN             EQU -10
KR_SCROLL_UP               EQU -7
KR_CANCEL                  EQU -4
KR_ACCEPT                  EQU -3
KR_AD_CLOSE                EQU -3
KR_AD_NEXT                 EQU -2
KR_AD_PREVIOUS             EQU -1
KR_CLOSE_CARS              EQU -3
KR_ENGINE_DONE             EQU -1
KR_ENGINE_PARTS            EQU -2
KR_ENGINE_TIMING           EQU -3
KR_INFO_OK                 EQU -3
KR_LOAD_GAME               EQU -2
KR_NEW_GAME                EQU -1
KR_OLD_GAME                EQU -2
KR_SAVE_GAME               EQU -3
KR_SELL_CAR                EQU -2
KR_START_OK                EQU -3
KR_SWITCH_CAR              EQU -1
KR_TIMING_DONE             EQU -1
KR_TRANS_DONE              EQU -1
KR_TRANS_PARTS             EQU -2
KR_BET_50                  EQU 3
KR_BET_PINK_SLIPS          EQU 9
KR_BOBS_CHALLENGE          EQU 2
KR_BOBS_FIND_RIVAL         EQU 4
KR_BOBS_GARAGE             EQU 1
KR_BOBS_HOOD               EQU 3
KR_END_QUIT                EQU 4
KR_GAR_CALENDAR            EQU 3
KR_GAR_CARS                EQU 6
KR_GAR_CAR_INFO            EQU 16
KR_GAR_FRONT_BUMPER        EQU 12
KR_GAR_HOOD                EQU 10
KR_GAR_NEWSPAPER           EQU 1
KR_GAR_PAINT               EQU 7
KR_GAR_QUIT                EQU 4
KR_GAR_REAR_BUMPER         EQU 11
KR_GAR_ROOF                EQU 13
KR_GAR_SELL_PARTS          EQU 5
KR_GAR_STICKERS            EQU 14
KR_GAR_STREET              EQU 2
KR_GAR_TANK                EQU 15
KR_GAR_TIRES               EQU 8
KR_GAR_TRANSMISSION        EQU 9
KR_GAS_CAP                 EQU 4
KR_GAS_PUMP                EQU 2
KR_GAS_RAG                 EQU 1
KR_PAINT_GO_AHEAD          EQU 3
KR_PAINT_NEXT              EQU 2
KR_PAINT_PREVIOUS          EQU 1
KR_RACE_DRAG               EQU 1
KR_RACE_ROAD               EQU 2
KR_REPAIR_FIX              EQU 1
KR_REPAIR_JUNK             EQU 2
KR_BET_FOR_KICKS           EQU 1
KR_BET_10                  EQU 2
KR_BET_25                  EQU 7
KR_BET_100                 EQU 8
KR_ENGINE_WIRES            EQU 13
KR_EDIT_HEADER             EQU -6
KR_PAGE_UP                 EQU -8
KR_PAGE_DOWN               EQU -9
KR_STICKER_1               EQU -31
KR_STICKER_2               EQU -32
KR_STICKER_3               EQU -33
KR_STICKER_4               EQU -34
KR_STICKER_5               EQU -35
KR_STICKER_6               EQU -36
KR_STICKER_7               EQU -37
KR_STICKER_8               EQU -38

KR_TIMING_ADVANCE          EQU KR_SCROLL_DOWN
KR_TIMING_RETARD           EQU KR_SCROLL_UP
KR_BUY_YEAH                EQU KR_OK
KR_BUY_FORGET              EQU KR_NO
KR_QUIT                    EQU KR_OK
KR_MENU_FORGET             EQU KR_NEVER_MIND
KR_OVERWRITE_GO_AHEAD      EQU KR_OK
KR_OVERWRITE_FORGET        EQU KR_NO
KR_BODY_FORGET             EQU KR_NEVER_MIND
KR_STICKER_SKIP            EQU KR_NEVER_MIND
KR_SELL_PART_NO_THANKS     EQU KR_NO

KEY MACRO sc, result
        DB sc, KEY_VALUE_BASE + (result)
ENDM

ITEMS MACRO sc, first, count
        LOCAL scan, result
scan = sc
result = first
        REPT count
        KEY scan, result
scan = scan + 1
result = result + 1
        ENDM
ENDM

        .DATA

        PUBLIC  _g_keyMapData
_g_keyMapData LABEL BYTE                  ; DS:5580
K_RACE_OUTCOME LABEL BYTE
        KEY SC_N,          -1
        KEY SC_ENTER,      -1
        KEY SC_O,          -2
        DB 0

K_START_GAME LABEL BYTE
        KEY SC_O,          KR_OLD_GAME
        KEY SC_ENTER,      KR_START_OK
        DB 0

K_LOAD LABEL BYTE
        ITEMS SC_1,        1, 10
        ITEMS SC_F1,       11, 5
        KEY SC_F,          KR_CANCEL
        KEY SC_ESC,        KR_CANCEL
        KEY SC_L,          KR_ACCEPT
        KEY SC_ENTER,      KR_ACCEPT
        DB 0

K_SPARE_PARTS LABEL BYTE
        KEY SC_C,          KR_ACCEPT
        ITEMS SC_1,        1, 10
        KEY SC_ENTER,      KR_ACCEPT
        KEY SC_ESC,        KR_CANCEL
        KEY SC_F,          KR_CANCEL
        DB 0

K_SAVE LABEL BYTE
        ITEMS SC_1,        1, 10
        ITEMS SC_F1,       11, 5
        KEY SC_S,          KR_ACCEPT
        KEY SC_ENTER,      KR_ACCEPT
        KEY SC_F,          KR_CANCEL
        KEY SC_ESC,        KR_CANCEL
        DB 0

K_SAVE_ERROR LABEL BYTE
        KEY SC_ENTER,      KR_OK
        KEY SC_O,          KR_OK
        DB 0

K_NEWSPAPER LABEL BYTE
        KEY SC_C,          1
        KEY SC_U,          1
        KEY SC_P,          2
        KEY SC_A,          2
        KEY SC_G,          3
        KEY SC_ESC,        3
        DB 0

K_ADVERTS LABEL BYTE
        KEY SC_P,          KR_AD_PREVIOUS
        KEY SC_N,          KR_AD_NEXT
        KEY SC_ESC,        KR_AD_CLOSE
        KEY SC_G,          KR_AD_CLOSE
        DB 0

K_BUY LABEL BYTE
        KEY SC_ENTER,      KR_BUY_YEAH
        KEY SC_ESC,        KR_BUY_FORGET
        KEY SC_S,          KR_SEE_IT
        KEY SC_Y,          KR_BUY_YEAH
        KEY SC_F,          KR_BUY_FORGET
        DB 0

K_GARAGE LABEL BYTE
        KEY SC_N,          KR_GAR_NEWSPAPER
        KEY SC_D,          KR_GAR_STREET
        KEY SC_X,          KR_GAR_CALENDAR
        KEY SC_Q,          KR_GAR_QUIT
        KEY SC_L,          KR_GAR_SELL_PARTS
        KEY SC_C,          KR_GAR_CARS
        KEY SC_S,          KR_GAR_STICKERS
        KEY SC_T,          KR_GAR_TIRES
        KEY SC_A,          KR_GAR_TRANSMISSION
        KEY SC_H,          KR_GAR_HOOD
        KEY SC_R,          KR_GAR_ROOF
        KEY SC_P,          KR_GAR_PAINT
        KEY SC_G,          KR_GAR_TANK
        KEY SC_I,          KR_GAR_CAR_INFO
        KEY SC_B,          KR_GAR_REAR_BUMPER
        KEY SC_F,          KR_GAR_FRONT_BUMPER
        DB 0

K_YOUR_CARS LABEL BYTE
        ITEMS SC_1,        1, 10
        KEY SC_ENTER,      KR_SWITCH_CAR
        KEY SC_S,          KR_SWITCH_CAR
        KEY SC_L,          KR_SELL_CAR
        KEY SC_ESC,        KR_CLOSE_CARS
        KEY SC_F,          KR_CLOSE_CARS
        DB 0

K_SELL_CAR_OFFER LABEL BYTE
        KEY SC_ENTER,      KR_OFFER
        KEY SC_ESC,        KR_NEVER_MIND
        KEY SC_O,          KR_OFFER
        KEY SC_N,          KR_NEVER_MIND
        DB 0

K_COUNTER_OFFER LABEL BYTE
        KEY SC_ENTER,      KR_OK
        KEY SC_O,          KR_OK
        KEY SC_N,          KR_NO
        KEY SC_ESC,        KR_NO
        DB 0

K_PAINT LABEL BYTE
        KEY SC_P,          KR_PAINT_PREVIOUS
        KEY SC_N,          KR_PAINT_NEXT
        KEY SC_ENTER,      KR_PAINT_GO_AHEAD
        KEY SC_ESC,        KR_FORGET
        KEY SC_F,          KR_FORGET
        DB 0

K_NO_SAVED_GAMES LABEL BYTE
        KEY SC_ENTER,      KR_OK
        KEY SC_O,          KR_OK
        DB 0

K_SELL_PART_OFFER LABEL BYTE
        KEY SC_N,          KR_SELL_PART_NO_THANKS
        KEY SC_O,          KR_OK
        KEY SC_ENTER,      KR_OK
        KEY SC_ESC,        KR_SELL_PART_NO_THANKS
        DB 0

K_UNUSED_17 LABEL BYTE
        DB 0

K_DRAG_WAGER LABEL BYTE
        KEY SC_1,          KR_BET_FOR_KICKS
        KEY SC_2,          KR_BET_10
        KEY SC_3,          KR_BET_50
        KEY SC_ENTER,      KR_BET_50
        KEY SC_ESC,        KR_FORGET
        KEY SC_F,          KR_FORGET
        DB 0

K_ROAD_WAGER LABEL BYTE
        KEY SC_1,          KR_BET_25
        KEY SC_2,          KR_BET_100
        KEY SC_3,          KR_BET_PINK_SLIPS
        KEY SC_ENTER,      KR_BET_PINK_SLIPS
        KEY SC_ESC,        KR_FORGET
        KEY SC_F,          KR_FORGET
        DB 0

K_TRANSMISSION LABEL BYTE
        KEY SC_1,          1
        KEY SC_2,          2
        KEY SC_D,          KR_TRANS_DONE
        KEY SC_ESC,        KR_TRANS_DONE
        KEY SC_ENTER,      KR_TRANS_DONE
        KEY SC_S,          KR_TRANS_PARTS
        DB 0

K_ENGINE LABEL BYTE
        ITEMS SC_1,        1, 9
        KEY SC_T,          KR_ENGINE_TIMING
        KEY SC_P,          KR_ENGINE_PARTS
        KEY SC_D,          KR_ENGINE_DONE
        KEY SC_W,          KR_ENGINE_WIRES
        KEY SC_ESC,        KR_ENGINE_DONE
        KEY SC_S,          KR_ENGINE_PARTS
        DB 0

K_RACE_TYPE LABEL BYTE
        KEY SC_D,          KR_RACE_DRAG
        KEY SC_R,          KR_RACE_ROAD
        KEY SC_ENTER,      KR_RACE_ROAD
        KEY SC_ESC,        KR_FORGET
        KEY SC_F,          KR_FORGET
        DB 0

K_GAS_STATION LABEL BYTE
        KEY SC_R,          KR_GAS_RAG
        KEY SC_P,          KR_GAS_PUMP
        KEY SC_T,          KR_GAS_CAP
        DB 0

K_MESSAGE LABEL BYTE
        KEY SC_ENTER,      KR_MESSAGE_DISMISS
        DB 0

K_REPAIR LABEL BYTE
        KEY SC_ENTER,      KR_REPAIR_FIX
        KEY SC_F,          KR_REPAIR_FIX
        KEY SC_ESC,        KR_REPAIR_JUNK
        KEY SC_J,          KR_REPAIR_JUNK
        DB 0

K_GET_RUNNING LABEL BYTE
        KEY SC_ENTER,      KR_REPAIR_FIX
        KEY SC_F,          KR_REPAIR_FIX
        KEY SC_ESC,        KR_REPAIR_JUNK
        KEY SC_J,          KR_REPAIR_JUNK
        DB 0

K_SAVE_RESTART_QUIT LABEL BYTE
        KEY SC_N,          KR_NEW_GAME
        KEY SC_O,          KR_OLD_GAME
        KEY SC_S,          KR_SAVE_GAME
        KEY SC_ENTER,      KR_SAVE_GAME
        KEY SC_Q,          KR_QUIT
        KEY SC_ESC,        KR_MENU_FORGET
        KEY SC_F,          KR_MENU_FORGET
        DB 0

K_TIMING LABEL BYTE
        KEY SC_ESC,        KR_TIMING_DONE
        KEY SC_D,          KR_TIMING_DONE
        KEY SC_A,          KR_TIMING_ADVANCE
        KEY SC_R,          KR_TIMING_RETARD
        DB 0

K_MESSAGE_BOX LABEL BYTE
        KEY SC_O,          KR_OK
        KEY SC_ENTER,      KR_OK
        DB 0

K_OVERWRITE_SAVE LABEL BYTE
        KEY SC_ESC,        KR_OVERWRITE_FORGET
        KEY SC_F,          KR_OVERWRITE_FORGET
        KEY SC_ENTER,      KR_OVERWRITE_GO_AHEAD
        KEY SC_G,          KR_OVERWRITE_GO_AHEAD
        DB 0

K_LOAD_ERROR LABEL BYTE
        KEY SC_O,          KR_OK
        KEY SC_ENTER,      KR_OK
        DB 0

K_READY LABEL BYTE
        KEY SC_O,          KR_OK
        KEY SC_ENTER,      KR_OK
        DB 0

K_RACE_STATS LABEL BYTE
        DB 0
        DB 0                             ; unreferenced empty list

K_BODY_CHANGE LABEL BYTE
        KEY SC_ENTER,      KR_OK
        KEY SC_ESC,        KR_BODY_FORGET
        KEY SC_O,          KR_OK
        KEY SC_F,          KR_BODY_FORGET
        DB 0

K_STICKERS LABEL BYTE
        KEY SC_1,          KR_STICKER_1
        KEY SC_2,          KR_STICKER_2
        KEY SC_3,          KR_STICKER_3
        KEY SC_4,          KR_STICKER_4
        KEY SC_5,          KR_STICKER_5
        KEY SC_6,          KR_STICKER_6
        KEY SC_7,          KR_STICKER_7
        KEY SC_8,          KR_STICKER_8
        KEY SC_ESC,        KR_STICKER_SKIP
        KEY SC_S,          KR_STICKER_SKIP
        DB 0

K_CALL_RIVAL LABEL BYTE
        KEY SC_C,          KR_ACCEPT
        ITEMS SC_1,        1, 10
        KEY SC_ENTER,      KR_ACCEPT
        KEY SC_ESC,        KR_CANCEL
        KEY SC_F,          KR_CANCEL
        DB 0

K_BOBS LABEL BYTE
        KEY SC_G,          KR_BOBS_GARAGE
        KEY SC_ENTER,      KR_BOBS_CHALLENGE
        KEY SC_C,          KR_BOBS_CHALLENGE
        KEY SC_H,          KR_BOBS_HOOD
        KEY SC_O,          KR_BOBS_FIND_RIVAL
        DB 0

K_CAR_INFO LABEL BYTE
        KEY SC_ESC,        KR_INFO_OK
        KEY SC_ENTER,      KR_INFO_OK
        DB 0

K_MANUAL_COLORS LABEL BYTE
        ITEMS SC_1,        1, 10
        KEY SC_F1,         11
        KEY SC_F2,         12
        DB 0

K_UNUSED_40 LABEL BYTE
        KEY SC_N,          3
        KEY SC_L,          2
        KEY SC_Q,          4
        KEY SC_S,          1
        DB 0

K_END_MENU LABEL BYTE
        KEY SC_N,          KR_NEW_GAME
        KEY SC_L,          KR_LOAD_GAME
        KEY SC_Q,          KR_END_QUIT
        DB 0

K_MUSIC LABEL BYTE
        KEY SC_M,          KR_MUSIC
        DB 0

        PUBLIC  _g_keyMaps
_g_keyMaps DW      42                       ; DS:579F struct KeyMap *[]
        DW      OFFSET DGROUP:K_RACE_OUTCOME
        DW      OFFSET DGROUP:K_START_GAME
        DW      OFFSET DGROUP:K_LOAD
        DW      OFFSET DGROUP:K_SPARE_PARTS
        DW      OFFSET DGROUP:K_SAVE
        DW      OFFSET DGROUP:K_SAVE_ERROR
        DW      OFFSET DGROUP:K_NEWSPAPER
        DW      OFFSET DGROUP:K_ADVERTS
        DW      OFFSET DGROUP:K_BUY
        DW      OFFSET DGROUP:K_GARAGE
        DW      OFFSET DGROUP:K_YOUR_CARS
        DW      OFFSET DGROUP:K_SELL_CAR_OFFER
        DW      OFFSET DGROUP:K_COUNTER_OFFER
        DW      OFFSET DGROUP:K_PAINT
        DW      OFFSET DGROUP:K_NO_SAVED_GAMES
        DW      OFFSET DGROUP:K_SELL_PART_OFFER
        DW      OFFSET DGROUP:K_UNUSED_17
        DW      OFFSET DGROUP:K_DRAG_WAGER
        DW      OFFSET DGROUP:K_ROAD_WAGER
        DW      OFFSET DGROUP:K_TRANSMISSION
        DW      OFFSET DGROUP:K_ENGINE
        DW      OFFSET DGROUP:K_RACE_TYPE
        DW      OFFSET DGROUP:K_GAS_STATION
        DW      OFFSET DGROUP:K_MESSAGE
        DW      OFFSET DGROUP:K_REPAIR
        DW      OFFSET DGROUP:K_GET_RUNNING
        DW      OFFSET DGROUP:K_SAVE_RESTART_QUIT
        DW      OFFSET DGROUP:K_TIMING
        DW      OFFSET DGROUP:K_MESSAGE_BOX
        DW      OFFSET DGROUP:K_OVERWRITE_SAVE
        DW      OFFSET DGROUP:K_LOAD_ERROR
        DW      OFFSET DGROUP:K_READY
        DW      OFFSET DGROUP:K_RACE_STATS
        DW      OFFSET DGROUP:K_BODY_CHANGE
        DW      OFFSET DGROUP:K_STICKERS
        DW      OFFSET DGROUP:K_CALL_RIVAL
        DW      OFFSET DGROUP:K_BOBS
        DW      OFFSET DGROUP:K_CAR_INFO
        DW      OFFSET DGROUP:K_MANUAL_COLORS
        DW      OFFSET DGROUP:K_UNUSED_40
        DW      OFFSET DGROUP:K_END_MENU
        DW      OFFSET DGROUP:K_MUSIC
        EVEN

        END
