        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA

        PUBLIC  _g_joyX
_g_joyX DW      0                        ; DS:4718 int
        PUBLIC  _g_joyY
_g_joyY DW      0                        ; DS:471A int
        PUBLIC  _g_joyButtons
_g_joyButtons DB      0                        ; DS:471C char
        PUBLIC  _g_joystick
_g_joystick DB      0                        ; DS:471D char
        PUBLIC  _g_joyCenterX
_g_joyCenterX DW      0                        ; DS:471E int
        PUBLIC  _g_joyCenterY
_g_joyCenterY DW      0                        ; DS:4720 int
        PUBLIC  _g_joyMinX
_g_joyMinX DW      0                        ; DS:4722 int
        PUBLIC  _g_joyMaxX
_g_joyMaxX DW      0                        ; DS:4724 int
        PUBLIC  _g_joyMinY
_g_joyMinY DW      0                        ; DS:4726 int
        PUBLIC  _g_joyMaxY
_g_joyMaxY DW      0                        ; DS:4728 int
        PUBLIC  _g_cursorX
_g_cursorX DW      9Eh                      ; DS:472A int
        PUBLIC  _g_cursorY
_g_cursorY DW      80h                      ; DS:472C int
        PUBLIC  _g_keyDx
_g_keyDx DB      0                        ; DS:472E char
        PUBLIC  _g_keyDy
_g_keyDy DB      0                        ; DS:472F char
        PUBLIC  _g_lastDx
_g_lastDx DB      0                        ; DS:4730 char
        PUBLIC  _g_lastDy
_g_lastDy DB      0                        ; DS:4731 char
        PUBLIC  _g_buttons
_g_buttons DB      0                        ; DS:4732 char
        PUBLIC  _g_cVis
_g_cVis DB      0                        ; DS:4733 signed char
        PUBLIC  _g_4734
_g_4734 DW      1                        ; DS:4734 int
        PUBLIC  _g_click
_g_click DB      0                        ; DS:4736 char
        PUBLIC  _g_click2
_g_click2 DB      0                        ; DS:4737 char
        PUBLIC  _g_clickX
_g_clickX DW      0                        ; DS:4738 int
        PUBLIC  _g_clickY
_g_clickY DW      0                        ; DS:473A int
        PUBLIC  _g_click2X
_g_click2X DW      0                        ; DS:473C int
        PUBLIC  _g_click2Y
_g_click2Y DW      0                        ; DS:473E int
        PUBLIC  _g_inputLock
_g_inputLock DW      0                        ; DS:4740 int
        PUBLIC  _g_4742
_g_4742 DW      0                        ; DS:4742 int
        PUBLIC  _g_4744
_g_4744 DW      0                        ; DS:4744 int
        PUBLIC  _g_savedCursorX
_g_savedCursorX DW      0                        ; DS:4746 int
        PUBLIC  _g_savedCursorY
_g_savedCursorY DW      0                        ; DS:4748 int
        PUBLIC  _g_474a
_g_474a DW      0FFFFh                   ; DS:474A int
        PUBLIC  _g_474c
_g_474c DW      0, 1, 0FFFFh             ; DS:474C int[]
; DS:4752 int[]
        PUBLIC  _g_4752
_g_4752 DW      0, 1, 101h, 100h, 1FFh, 1, 0FFh, 0FF01h, 0FF00h, 0FFFFh
        PUBLIC  _g_scanMap
_g_scanMap DB      0, 1Bh, '1234567890-', 0  ; DS:4766 char[]
        DB      0, 0, 'QWERTYUIOP', 0
        DB      0, 0Dh, 1Dh, 'ASDFGHJKL', 0
        DB      0, 0FCh, 0, 0F8h, 'ZXCVBNM', 0
        DB      5 DUP (0), 1Fh, 13 DUP (0), 0FEh, 0FCh, 0FAh, 0F2h, 0F8h
        DB      0, 0F6h, 0F6h, 0F4h, 0F2h, 0F0h, 1Fh, 0, 0, 1Fh
        DB      16 DUP (0)
        PUBLIC  _g_demoKey
_g_demoKey DB      0                        ; DS:47CC char
        PUBLIC  _g_fireLatch
_g_fireLatch DB      0                        ; DS:47CD char
        PUBLIC  _g_soundOn
_g_soundOn DB      1                        ; DS:47CE char
        PUBLIC  _g_speakerOn
_g_speakerOn DB      0                        ; DS:47CF char
        PUBLIC  _g_lastKey
_g_lastKey DB      0                        ; DS:47D0 char
        PUBLIC  _g_47d1
_g_47d1 DB      1Dh, 1Eh, 1Fh            ; DS:47D1 char[3]
        PUBLIC  _g_oldInt9Seg
_g_oldInt9Seg DW      0                        ; DS:47D4 int
        PUBLIC  _g_oldInt9Off
_g_oldInt9Off DW      0                        ; DS:47D6 int
        PUBLIC  _g_dirHeld
_g_dirHeld DW      8 DUP (0)                ; DS:47D8 int[8]
        PUBLIC  _g_joySavedSp
_g_joySavedSp DW      0                        ; DS:47E8 int
        PUBLIC  _g_modeBitsA
_g_modeBitsA DB      0                        ; DS:47EA char
        PUBLIC  _g_biosModeA
_g_biosModeA DB      0                        ; DS:47EB char
        PUBLIC  _g_modeBitsB
_g_modeBitsB DB      0                        ; DS:47EC char
        PUBLIC  _g_biosModeB
_g_biosModeB DB      0                        ; DS:47ED char
        PUBLIC  _g_firePressed
_g_firePressed DW      0                        ; DS:47EE int
        PUBLIC  _g_47f0
_g_47f0 DB      0                        ; DS:47F0 char
S_47f1  DB      '`Bankroll:`$', 0        ; DS:47F1 string
        PUBLIC  _g_gearText
_g_gearText DB      ' Gear: ', 0             ; DS:47FE char[]
        PUBLIC  _g_parts
_g_parts DW      13h                      ; DS:4806 struct Item[]
        DB      0, 1, 0, 0
        DW      73Ch
        DW      13h
        DB      0, 2, 0, 0
        DW      74Bh
        DW      13h
        DB      0, 4, 0, 0
        DW      75Ch
        DW      15Eh
        DB      1, 1, 0, 0
        DW      771h
        DW      154h
        DB      1, 2, 0, 0
        DW      7A4h
        DW      168h
        DB      1, 4, 0, 0
        DW      7D6h
        DW      1F4h
        DB      2, 1, 0, 0
        DW      804h
        DW      1DBh
        DB      2, 2, 0, 0
        DW      833h
        DW      20Dh
        DB      2, 4, 0, 0
        DW      853h
        DW      82h
        DB      0, 1, 1, 0
        DW      886h
        DW      78h
        DB      0, 2, 1, 0
        DW      89Fh
        DW      7Dh
        DB      0, 4, 1, 0
        DW      8B9h
        DW      0AFh
        DB      1, 1, 1, 0
        DW      8D8h
        DW      0A0h
        DB      1, 2, 1, 0
        DW      8F2h
        DW      0AAh
        DB      1, 4, 1, 0
        DW      90Eh
        DW      0FAh
        DB      2, 1, 1, 0
        DW      92Eh
        DW      104h
        DB      2, 2, 1, 0
        DW      949h
        DW      0F5h
        DB      2, 4, 1, 0
        DW      966h
        DW      113h
        DB      2, 7, 1, 0
        DW      987h
        DW      190h
        DB      3, 1, 1, 0
        DW      9A8h
        DW      195h
        DB      3, 2, 1, 0
        DW      9C2h
        DW      19Ah
        DB      3, 4, 1, 0
        DW      9DEh
        DW      19h
        DB      0, 7, 2, 0
        DW      9FEh
        DW      28h
        DB      1, 7, 2, 0
        DW      0A1Ch
        DW      3Ch
        DB      2, 7, 2, 0
        DW      0A3Ah
        DW      14h
        DB      0, 1, 3, 0
        DW      0A6Bh
        DW      14h
        DB      0, 2, 3, 0
        DW      0A85h
        DW      14h
        DB      0, 4, 3, 0
        DW      0AA1h
        DW      19h
        DB      1, 1, 3, 0
        DW      0AC1h
        DW      19h
        DB      1, 2, 3, 0
        DW      0ADBh
        DW      19h
        DB      1, 4, 3, 0
        DW      0AF7h
        DW      23h
        DB      2, 1, 3, 0
        DW      0B91h
        DW      23h
        DB      2, 2, 3, 0
        DW      0BB6h
        DW      23h
        DB      2, 4, 3, 0
        DW      0BDEh
        DW      23h
        DB      3, 1, 3, 0
        DW      0B17h
        DW      23h
        DB      3, 2, 3, 0
        DW      0B3Dh
        DW      23h
        DB      3, 4, 3, 0
        DW      0B66h
        DW      3Ch
        DB      4, 1, 3, 0
        DW      0C09h
        DW      3Ch
        DB      4, 2, 3, 0
        DW      0C24h
        DW      3Ch
        DB      4, 4, 3, 0
        DW      0C41h
        DW      32h
        DB      0, 7, 4, 0
        DW      0C62h
        DW      50h
        DB      1, 7, 4, 0
        DW      0C79h
        DW      96h
        DB      2, 7, 4, 0
        DW      0CACh
        DW      0
        DB      0, 0, 0, 0
        DW      0
        PUBLIC  _g_partWear
_g_partWear DW      0, 0                     ; DS:4966 int[]
        PUBLIC  _g_specPartWear
_g_specPartWear DW      0FF80h                   ; DS:496A int
        PUBLIC  _g_496c
_g_496c DW      0FF80h, 0                ; DS:496C int[2]
        PUBLIC  _g_betRaces
_g_betRaces DW      0                        ; DS:4970 int
        PUBLIC  _g_dragWins
_g_dragWins DW      0                        ; DS:4972 int
        PUBLIC  _g_roadWins
_g_roadWins DW      0                        ; DS:4974 int
        PUBLIC  _g_kingDragWon
_g_kingDragWon DW      0                        ; DS:4976 int
        PUBLIC  _g_manifoldFit
_g_manifoldFit DB      1, 0, 1, 0, 0            ; DS:4978 char[][5]
        DB      1, 1, 0, 1, 1
        DB      1, 1, 0, 1, 1
        EVEN
        PUBLIC  _g_carbFit
_g_carbFit DB      1, 0, 0                  ; DS:4988 char[][3]
        DB      0, 1, 1
        DB      1, 0, 0
        DB      0, 1, 1
        DB      0, 1, 1
        EVEN
        PUBLIC  _g_partPics
_g_partPics DW      434h, 435h, 436h, 0, 0   ; DS:4998 int[][5]
        DW      439h, 43Ah, 437h, 43Bh, 450h
        DW      43Ch, 43Dh, 43Eh, 0, 0
        DW      43Ch, 43Dh, 43Eh, 0, 0
        DW      43Ch, 43Dh, 43Eh, 0, 0
        PUBLIC  _g_boltPics
_g_boltPics DW      0, 43Fh, 440h, 441h      ; DS:49CA int[]
        PUBLIC  _g_49d2
_g_49d2 DW      44Bh, 44Ch, 44Dh, 44Eh, 47F0h  ; DS:49D2 int[]
        PUBLIC  _g_49dc
_g_49dc DW      OFFSET DGROUP:S_47f1     ; DS:49DC char *
        PUBLIC  _g_gearLabel
_g_gearLabel DW      OFFSET DGROUP:_g_gearText    ; DS:49DE char *
        PUBLIC  _g_list
_g_list DW      143 DUP (0)              ; DS:49E0 int[]

        .CODE S0000_TEXT

; mb_store_mode_a @ 0000:2B9A (linear 0x02B9A, 19 bytes)

        PUBLIC  _mb_store_mode_a
_mb_store_mode_a  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        mov     al,[bp+6]
        mov     _g_modeBitsA,al
        mov     al,[bp+8]
        mov     _g_biosModeA,al
        pop     ax
        pop     bp
        ret
_mb_store_mode_a  ENDP

; set_g47ec_47ed @ 0000:2BAD (linear 0x02BAD, 19 bytes)

        PUBLIC  _set_g47ec_47ed
_set_g47ec_47ed  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        mov     al,[bp+6]
        mov     _g_modeBitsB,al
        mov     al,[bp+8]
        mov     _g_biosModeB,al
        pop     ax
        pop     bp
        ret
_set_g47ec_47ed  ENDP

; mb_apply_mode_a @ 0000:2BC0 (linear 0x02BC0, 34 bytes)

        PUBLIC  _mb_apply_mode_a
_mb_apply_mode_a  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        sub     ax,ax
        mov     es,ax
        cli
        mov     al,es:[BIOS_EQUIP]
        and     al,0FFh - EQUIP_VIDEO_MASK
        or      al,_g_modeBitsA
        mov     es:[BIOS_EQUIP],al
        mov     ah,VIDEO_SET_MODE
        mov     al,_g_biosModeA
        sti
        int     BIOS_VIDEO
        pop     ax
        pop     bp
        ret
_mb_apply_mode_a  ENDP

; mb_apply_mode_b @ 0000:2BE2 (linear 0x02BE2, 34 bytes)

        PUBLIC  _mb_apply_mode_b
_mb_apply_mode_b  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        sub     ax,ax
        mov     es,ax
        cli
        mov     al,es:[BIOS_EQUIP]
        and     al,0FFh - EQUIP_VIDEO_MASK
        or      al,_g_modeBitsB
        mov     es:[BIOS_EQUIP],al
        mov     ah,VIDEO_SET_MODE
        mov     al,_g_biosModeB
        sti
        int     BIOS_VIDEO
        pop     ax
        pop     bp
        ret
_mb_apply_mode_b  ENDP

        END
