        .MODEL MEDIUM
        INCLUDE PC.INC

KV_ITEM_BASE               EQU 0CEh
KV_MUSIC                   EQU 0A6h
KV_MESSAGE_DISMISS         EQU 0B4h
KV_SEE_IT                  EQU 0B5h
KV_OFFER                   EQU 0B6h
KV_FORGET                  EQU 0B7h
KV_NEVER_MIND              EQU 0B8h
KV_NO                      EQU 0B9h
KV_OK                      EQU 0BAh
KV_SCROLL_DOWN             EQU 0C4h
KV_SCROLL_UP               EQU 0C7h
KV_CANCEL                  EQU 0CAh
KV_ACCEPT                  EQU 0CBh
KV_RETURN_MINUS_2          EQU 0CCh
KV_RETURN_MINUS_1          EQU 0CDh

KV_AD_CLOSE                EQU KV_ACCEPT
KV_AD_NEXT                 EQU KV_RETURN_MINUS_2
KV_AD_PREVIOUS             EQU KV_RETURN_MINUS_1
KV_CLOSE_CARS              EQU KV_ACCEPT
KV_ENGINE_DONE             EQU KV_RETURN_MINUS_1
KV_ENGINE_PARTS            EQU KV_RETURN_MINUS_2
KV_ENGINE_TIMING           EQU KV_ACCEPT
KV_INFO_OK                 EQU KV_ACCEPT
KV_LOAD_GAME               EQU KV_RETURN_MINUS_2
KV_NEW_GAME                EQU KV_RETURN_MINUS_1
KV_OLD_GAME                EQU KV_RETURN_MINUS_2
KV_SAVE_GAME               EQU KV_ACCEPT
KV_SELL_CAR                EQU KV_RETURN_MINUS_2
KV_START_OK                EQU KV_ACCEPT
KV_SWITCH_CAR              EQU KV_RETURN_MINUS_1
KV_TIMING_DONE             EQU KV_RETURN_MINUS_1
KV_TRANS_DONE              EQU KV_RETURN_MINUS_1
KV_TRANS_PARTS             EQU KV_RETURN_MINUS_2

KV_BET_50                  EQU 0D1h
KV_BET_PINK_SLIPS          EQU 0D7h
KV_BOBS_CHALLENGE          EQU 0D0h
KV_BOBS_FIND_RIVAL         EQU 0D2h
KV_BOBS_GARAGE             EQU 0CFh
KV_BOBS_HOOD               EQU 0D1h
KV_END_QUIT                EQU 0D2h
KV_GAR_CALENDAR            EQU 0D1h
KV_GAR_CARS                EQU 0D4h
KV_GAR_CAR_INFO            EQU 0DEh
KV_GAR_FRONT_BUMPER        EQU 0DAh
KV_GAR_HOOD                EQU 0D8h
KV_GAR_NEWSPAPER           EQU 0CFh
KV_GAR_PAINT               EQU 0D5h
KV_GAR_QUIT                EQU 0D2h
KV_GAR_REAR_BUMPER         EQU 0D9h
KV_GAR_ROOF                EQU 0DBh
KV_GAR_SELL_PARTS          EQU 0D3h
KV_GAR_STICKERS            EQU 0DCh
KV_GAR_STREET              EQU 0D0h
KV_GAR_TANK                EQU 0DDh
KV_GAR_TIRES               EQU 0D6h
KV_GAR_TRANSMISSION        EQU 0D7h
KV_GAS_CAP                 EQU 0D2h
KV_GAS_PUMP                EQU 0D0h
KV_GAS_RAG                 EQU 0CFh
KV_PAINT_GO_AHEAD          EQU 0D1h
KV_PAINT_NEXT              EQU 0D0h
KV_PAINT_PREVIOUS          EQU 0CFh
KV_RACE_DRAG               EQU 0CFh
KV_RACE_ROAD               EQU 0D0h
KV_REPAIR_FIX              EQU 0CFh
KV_REPAIR_JUNK             EQU 0D0h
KV_TIMING_ADVANCE          EQU 0C4h
KV_TIMING_RETARD           EQU 0C7h

KV_BET_FOR_KICKS           EQU 0CFh

KV_BET_10                  EQU 0D0h

KV_BET_25                  EQU 0D5h

KV_BET_100                 EQU 0D6h

KEY MACRO sc, value
        DB sc, value
ENDM

ITEM MACRO sc, number
        DB sc, KV_ITEM_BASE + number
ENDM

ITEMS MACRO sc, first, count
        LOCAL scan, number
scan = sc
number = first
        REPT count
        ITEM scan, number
scan = scan + 1
number = number + 1
        ENDM
ENDM

STICKER MACRO sc, number
        DB sc, 0B0h - number
ENDM

        .DATA

        PUBLIC  _g_keyMapData
_g_keyMapData LABEL BYTE                  ; DS:5580
K_RACE LABEL BYTE
        KEY SC_N,          KV_RETURN_MINUS_1
        KEY SC_ENTER,      KV_RETURN_MINUS_1
        KEY SC_O,          KV_RETURN_MINUS_2
        DB 0

K_START_GAME LABEL BYTE
        KEY SC_O,          KV_OLD_GAME
        KEY SC_ENTER,      KV_START_OK
        DB 0

K_LOAD LABEL BYTE
        ITEMS SC_1,        1, 10
        ITEMS SC_F1,       11, 5
        KEY SC_F,          KV_CANCEL
        KEY SC_ESC,        KV_CANCEL
        KEY SC_L,          KV_ACCEPT
        KEY SC_ENTER,      KV_ACCEPT
        DB 0

K_SPARE_PARTS LABEL BYTE
        KEY SC_C,          KV_ACCEPT
        ITEMS SC_1,        1, 10
        KEY SC_ENTER,      KV_ACCEPT
        KEY SC_ESC,        KV_CANCEL
        KEY SC_F,          KV_CANCEL
        DB 0

K_SAVE LABEL BYTE
        ITEMS SC_1,        1, 10
        ITEMS SC_F1,       11, 5
        KEY SC_S,          KV_ACCEPT
        KEY SC_ENTER,      KV_ACCEPT
        KEY SC_F,          KV_CANCEL
        KEY SC_ESC,        KV_CANCEL
        DB 0

K_SAVE_ERROR LABEL BYTE
        KEY SC_ENTER,      KV_OK
        KEY SC_O,          KV_OK
        DB 0

K_NEWSPAPER LABEL BYTE
        ITEM SC_C,         1
        ITEM SC_U,         1
        ITEM SC_P,         2
        ITEM SC_A,         2
        ITEM SC_G,         3
        ITEM SC_ESC,       3
        DB 0

K_ADVERTS LABEL BYTE
        KEY SC_P,          KV_AD_PREVIOUS
        KEY SC_N,          KV_AD_NEXT
        KEY SC_ESC,        KV_AD_CLOSE
        KEY SC_G,          KV_AD_CLOSE
        DB 0

K_BUY LABEL BYTE
        KEY SC_ENTER,      KV_OK
        KEY SC_ESC,        KV_NO
        KEY SC_S,          KV_SEE_IT
        KEY SC_Y,          KV_OK
        KEY SC_F,          KV_NO
        DB 0

K_GARAGE LABEL BYTE
        KEY SC_N,          KV_GAR_NEWSPAPER
        KEY SC_D,          KV_GAR_STREET
        KEY SC_X,          KV_GAR_CALENDAR
        KEY SC_Q,          KV_GAR_QUIT
        KEY SC_L,          KV_GAR_SELL_PARTS
        KEY SC_C,          KV_GAR_CARS
        KEY SC_S,          KV_GAR_STICKERS
        KEY SC_T,          KV_GAR_TIRES
        KEY SC_A,          KV_GAR_TRANSMISSION
        KEY SC_H,          KV_GAR_HOOD
        KEY SC_R,          KV_GAR_ROOF
        KEY SC_P,          KV_GAR_PAINT
        KEY SC_G,          KV_GAR_TANK
        KEY SC_I,          KV_GAR_CAR_INFO
        KEY SC_B,          KV_GAR_REAR_BUMPER
        KEY SC_F,          KV_GAR_FRONT_BUMPER
        DB 0

K_YOUR_CARS LABEL BYTE
        ITEMS SC_1,        1, 10
        KEY SC_ENTER,      KV_SWITCH_CAR
        KEY SC_S,          KV_SWITCH_CAR
        KEY SC_L,          KV_SELL_CAR
        KEY SC_ESC,        KV_CLOSE_CARS
        KEY SC_F,          KV_CLOSE_CARS
        DB 0

K_SELL_CAR_OFFER LABEL BYTE
        KEY SC_ENTER,      KV_OFFER
        KEY SC_ESC,        KV_NEVER_MIND
        KEY SC_O,          KV_OFFER
        KEY SC_N,          KV_NEVER_MIND
        DB 0

K_COUNTER_OFFER LABEL BYTE
        KEY SC_ENTER,      KV_OK
        KEY SC_O,          KV_OK
        KEY SC_N,          KV_NO
        KEY SC_ESC,        KV_NO
        DB 0

K_PAINT LABEL BYTE
        KEY SC_P,          KV_PAINT_PREVIOUS
        KEY SC_N,          KV_PAINT_NEXT
        KEY SC_ENTER,      KV_PAINT_GO_AHEAD
        KEY SC_ESC,        KV_FORGET
        KEY SC_F,          KV_FORGET
        DB 0

K_NO_SAVED_GAMES LABEL BYTE
        KEY SC_ENTER,      KV_OK
        KEY SC_O,          KV_OK
        DB 0

K_SELL_PART_OFFER LABEL BYTE
        KEY SC_N,          KV_NO
        KEY SC_O,          KV_OK
        KEY SC_ENTER,      KV_OK
        KEY SC_ESC,        KV_NO
        DB 0

K_5688 LABEL BYTE
        DB 0

K_DRAG_WAGER LABEL BYTE
        KEY SC_1,          KV_BET_FOR_KICKS
        KEY SC_2,          KV_BET_10
        KEY SC_3,          KV_BET_50
        KEY SC_ENTER,      KV_BET_50
        KEY SC_ESC,        KV_FORGET
        KEY SC_F,          KV_FORGET
        DB 0

K_ROAD_WAGER LABEL BYTE
        KEY SC_1,          KV_BET_25
        KEY SC_2,          KV_BET_100
        KEY SC_3,          KV_BET_PINK_SLIPS
        KEY SC_ENTER,      KV_BET_PINK_SLIPS
        KEY SC_ESC,        KV_FORGET
        KEY SC_F,          KV_FORGET
        DB 0

K_TRANSMISSION LABEL BYTE
        ITEM SC_1,         1
        ITEM SC_2,         2
        KEY SC_D,          KV_TRANS_DONE
        KEY SC_ESC,        KV_TRANS_DONE
        KEY SC_ENTER,      KV_TRANS_DONE
        KEY SC_S,          KV_TRANS_PARTS
        DB 0

K_ENGINE LABEL BYTE
        ITEMS SC_1,        1, 9
        KEY SC_T,          KV_ENGINE_TIMING
        KEY SC_P,          KV_ENGINE_PARTS
        KEY SC_D,          KV_ENGINE_DONE
        ITEM SC_W,         13
        KEY SC_ESC,        KV_ENGINE_DONE
        KEY SC_S,          KV_ENGINE_PARTS
        DB 0

K_RACE_TYPE LABEL BYTE
        KEY SC_D,          KV_RACE_DRAG
        KEY SC_R,          KV_RACE_ROAD
        KEY SC_ENTER,      KV_RACE_ROAD
        KEY SC_ESC,        KV_FORGET
        KEY SC_F,          KV_FORGET
        DB 0

K_GAS_STATION LABEL BYTE
        KEY SC_R,          KV_GAS_RAG
        KEY SC_P,          KV_GAS_PUMP
        KEY SC_T,          KV_GAS_CAP
        DB 0

K_MESSAGE LABEL BYTE
        KEY SC_ENTER,      KV_MESSAGE_DISMISS
        DB 0

K_REPAIR LABEL BYTE
        KEY SC_ENTER,      KV_REPAIR_FIX
        KEY SC_F,          KV_REPAIR_FIX
        KEY SC_ESC,        KV_REPAIR_JUNK
        KEY SC_J,          KV_REPAIR_JUNK
        DB 0

K_GET_RUNNING LABEL BYTE
        KEY SC_ENTER,      KV_REPAIR_FIX
        KEY SC_F,          KV_REPAIR_FIX
        KEY SC_ESC,        KV_REPAIR_JUNK
        KEY SC_J,          KV_REPAIR_JUNK
        DB 0

K_SAVE_RESTART_QUIT LABEL BYTE
        KEY SC_N,          KV_NEW_GAME
        KEY SC_O,          KV_OLD_GAME
        KEY SC_S,          KV_SAVE_GAME
        KEY SC_ENTER,      KV_SAVE_GAME
        KEY SC_Q,          KV_OK
        KEY SC_ESC,        KV_NEVER_MIND
        KEY SC_F,          KV_NEVER_MIND
        DB 0

K_TIMING LABEL BYTE
        KEY SC_ESC,        KV_TIMING_DONE
        KEY SC_D,          KV_TIMING_DONE
        KEY SC_A,          KV_TIMING_ADVANCE
        KEY SC_R,          KV_TIMING_RETARD
        DB 0

K_MESSAGE_BOX LABEL BYTE
        KEY SC_O,          KV_OK
        KEY SC_ENTER,      KV_OK
        DB 0

K_OVERWRITE_SAVE LABEL BYTE
        KEY SC_ESC,        KV_NO
        KEY SC_F,          KV_NO
        KEY SC_ENTER,      KV_OK
        KEY SC_G,          KV_OK
        DB 0

K_LOAD_ERROR LABEL BYTE
        KEY SC_O,          KV_OK
        KEY SC_ENTER,      KV_OK
        DB 0

K_READY LABEL BYTE
        KEY SC_O,          KV_OK
        KEY SC_ENTER,      KV_OK
        DB 0

K_RACE_STATS LABEL BYTE
        DB 0
        DB 0                             ; unreferenced empty list

K_BODY_CHANGE LABEL BYTE
        KEY SC_ENTER,      KV_OK
        KEY SC_ESC,        KV_NEVER_MIND
        KEY SC_O,          KV_OK
        KEY SC_F,          KV_NEVER_MIND
        DB 0

K_STICKERS LABEL BYTE
        STICKER SC_1,      1
        STICKER SC_2,      2
        STICKER SC_3,      3
        STICKER SC_4,      4
        STICKER SC_5,      5
        STICKER SC_6,      6
        STICKER SC_7,      7
        STICKER SC_8,      8
        KEY SC_ESC,        KV_NEVER_MIND
        KEY SC_S,          KV_NEVER_MIND
        DB 0

K_CALL_RIVAL LABEL BYTE
        KEY SC_C,          KV_ACCEPT
        ITEMS SC_1,        1, 10
        KEY SC_ENTER,      KV_ACCEPT
        KEY SC_ESC,        KV_CANCEL
        KEY SC_F,          KV_CANCEL
        DB 0

K_BOBS LABEL BYTE
        KEY SC_G,          KV_BOBS_GARAGE
        KEY SC_ENTER,      KV_BOBS_CHALLENGE
        KEY SC_C,          KV_BOBS_CHALLENGE
        KEY SC_H,          KV_BOBS_HOOD
        KEY SC_O,          KV_BOBS_FIND_RIVAL
        DB 0

K_CAR_INFO LABEL BYTE
        KEY SC_ESC,        KV_INFO_OK
        KEY SC_ENTER,      KV_INFO_OK
        DB 0

K_MANUAL_COLORS LABEL BYTE
        ITEMS SC_1,        1, 10
        ITEM SC_F1,        11
        ITEM SC_F2,        12
        DB 0

K_578c LABEL BYTE
        ITEM SC_N,         3
        ITEM SC_L,         2
        ITEM SC_Q,         4
        ITEM SC_S,         1
        DB 0

K_END_MENU LABEL BYTE
        KEY SC_N,          KV_NEW_GAME
        KEY SC_L,          KV_LOAD_GAME
        KEY SC_Q,          KV_END_QUIT
        DB 0

K_MUSIC LABEL BYTE
        KEY SC_M,          KV_MUSIC
        DB 0

        PUBLIC  _g_keyMaps
_g_keyMaps DW      42                       ; DS:579F struct KeyMap *[]
        DW      OFFSET DGROUP:K_RACE
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
        DW      OFFSET DGROUP:K_5688
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
        DW      OFFSET DGROUP:K_578c
        DW      OFFSET DGROUP:K_END_MENU
        DW      OFFSET DGROUP:K_MUSIC
        EVEN

        END
