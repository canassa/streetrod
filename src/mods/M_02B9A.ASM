        .MODEL MEDIUM

        .DATA

        PUBLIC  _g_4718
_g_4718 DW      0                        ; DS:4718 int
        PUBLIC  _g_471a
_g_471a DW      0                        ; DS:471A int
        PUBLIC  _g_471c
_g_471c DB      0                        ; DS:471C char
        PUBLIC  _g_471d
_g_471d DB      0                        ; DS:471D char
        PUBLIC  _g_471e
_g_471e DW      0                        ; DS:471E int
        PUBLIC  _g_4720
_g_4720 DW      0                        ; DS:4720 int
        PUBLIC  _g_4722
_g_4722 DW      0                        ; DS:4722 int
        PUBLIC  _g_4724
_g_4724 DW      0                        ; DS:4724 int
        PUBLIC  _g_4726
_g_4726 DW      0                        ; DS:4726 int
        PUBLIC  _g_4728
_g_4728 DW      0                        ; DS:4728 int
        PUBLIC  _g_472a
_g_472a DW      9Eh                      ; DS:472A int
        PUBLIC  _g_472c
_g_472c DW      80h                      ; DS:472C int
        PUBLIC  _g_472e
_g_472e DB      0                        ; DS:472E char
        PUBLIC  _g_472f
_g_472f DB      0                        ; DS:472F char
        PUBLIC  _g_4730
_g_4730 DB      0                        ; DS:4730 char
        PUBLIC  _g_4731
_g_4731 DB      0                        ; DS:4731 char
        PUBLIC  _g_4732
_g_4732 DB      0                        ; DS:4732 char
        PUBLIC  _g_4733
_g_4733 DB      0                        ; DS:4733 signed char
        PUBLIC  _g_4734
_g_4734 DW      1                        ; DS:4734 int
        PUBLIC  _g_4736
_g_4736 DB      0                        ; DS:4736 char
        PUBLIC  _g_4737
_g_4737 DB      0                        ; DS:4737 char
        PUBLIC  _g_4738
_g_4738 DW      0                        ; DS:4738 int
        PUBLIC  _g_473a
_g_473a DW      0                        ; DS:473A int
        PUBLIC  _g_473c
_g_473c DW      0                        ; DS:473C int
        PUBLIC  _g_473e
_g_473e DW      0                        ; DS:473E int
        PUBLIC  _g_4740
_g_4740 DW      0                        ; DS:4740 int
        PUBLIC  _g_4742
_g_4742 DW      0                        ; DS:4742 int
        PUBLIC  _g_4744
_g_4744 DW      0                        ; DS:4744 int
        PUBLIC  _g_4746
_g_4746 DW      0                        ; DS:4746 int
        PUBLIC  _g_4748
_g_4748 DW      0                        ; DS:4748 int
        PUBLIC  _g_474a
_g_474a DW      0FFFFh                   ; DS:474A int
        PUBLIC  _g_474c
_g_474c DW      0, 1, 0FFFFh             ; DS:474C int[]
; DS:4752 int[]
        PUBLIC  _g_4752
_g_4752 DW      0, 1, 101h, 100h, 1FFh, 1, 0FFh, 0FF01h, 0FF00h, 0FFFFh
        PUBLIC  _g_4766
_g_4766 DB      0, 1Bh, '1234567890-', 0  ; DS:4766 char[]
        DB      0, 0, 'QWERTYUIOP', 0
        DB      0, 0Dh, 1Dh, 'ASDFGHJKL', 0
        DB      0, 0FCh, 0, 0F8h, 'ZXCVBNM', 0
        DB      5 DUP (0), 1Fh, 13 DUP (0), 0FEh, 0FCh, 0FAh, 0F2h, 0F8h
        DB      0, 0F6h, 0F6h, 0F4h, 0F2h, 0F0h, 1Fh, 0, 0, 1Fh
        DB      16 DUP (0)
        PUBLIC  _g_47cc
_g_47cc DB      0                        ; DS:47CC char
        PUBLIC  _g_47cd
_g_47cd DB      0                        ; DS:47CD char
        PUBLIC  _g_47ce
_g_47ce DB      1                        ; DS:47CE char
        PUBLIC  _g_47cf
_g_47cf DB      0                        ; DS:47CF char
        PUBLIC  _g_47d0
_g_47d0 DB      0                        ; DS:47D0 char
        PUBLIC  _g_47d1
_g_47d1 DB      1Dh, 1Eh, 1Fh            ; DS:47D1 char[3]
        PUBLIC  _g_47d4
_g_47d4 DW      0                        ; DS:47D4 int
        PUBLIC  _g_47d6
_g_47d6 DW      0                        ; DS:47D6 int
        PUBLIC  _g_47d8
_g_47d8 DW      8 DUP (0)                ; DS:47D8 int[8]
        PUBLIC  _g_47e8
_g_47e8 DW      0                        ; DS:47E8 int
        PUBLIC  _g_47ea
_g_47ea DB      0                        ; DS:47EA char
        PUBLIC  _g_47eb
_g_47eb DB      0                        ; DS:47EB char
        PUBLIC  _g_47ec
_g_47ec DB      0                        ; DS:47EC char
        PUBLIC  _g_47ed
_g_47ed DB      0                        ; DS:47ED char
        PUBLIC  _g_47ee
_g_47ee DW      0                        ; DS:47EE int
        PUBLIC  _g_47f0
_g_47f0 DB      0                        ; DS:47F0 char
S_47f1  DB      '`Bankroll:`$', 0        ; DS:47F1 string
        PUBLIC  _g_47fe
_g_47fe DB      ' Gear: ', 0             ; DS:47FE char[]
        PUBLIC  _g_4806
_g_4806 DW      13h                      ; DS:4806 struct Item[]
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
        PUBLIC  _g_4966
_g_4966 DW      0, 0                     ; DS:4966 int[]
        PUBLIC  _g_496a
_g_496a DW      0FF80h                   ; DS:496A int
        PUBLIC  _g_496c
_g_496c DW      0FF80h, 0                ; DS:496C int[2]
        PUBLIC  _g_4970
_g_4970 DW      0                        ; DS:4970 int
        PUBLIC  _g_4972
_g_4972 DW      0                        ; DS:4972 int
        PUBLIC  _g_4974
_g_4974 DW      0                        ; DS:4974 int
        PUBLIC  _g_4976
_g_4976 DW      0                        ; DS:4976 int
        PUBLIC  _g_4978
_g_4978 DB      1, 0, 1, 0, 0            ; DS:4978 char[][5]
        DB      1, 1, 0, 1, 1
        DB      1, 1, 0, 1, 1
        EVEN
        PUBLIC  _g_4988
_g_4988 DB      1, 0, 0                  ; DS:4988 char[][3]
        DB      0, 1, 1
        DB      1, 0, 0
        DB      0, 1, 1
        DB      0, 1, 1
        EVEN
        PUBLIC  _g_4998
_g_4998 DW      434h, 435h, 436h, 0, 0   ; DS:4998 int[][5]
        DW      439h, 43Ah, 437h, 43Bh, 450h
        DW      43Ch, 43Dh, 43Eh, 0, 0
        DW      43Ch, 43Dh, 43Eh, 0, 0
        DW      43Ch, 43Dh, 43Eh, 0, 0
        PUBLIC  _g_49ca
_g_49ca DW      0, 43Fh, 440h, 441h      ; DS:49CA int[]
        PUBLIC  _g_49d2
_g_49d2 DW      44Bh, 44Ch, 44Dh, 44Eh, 47F0h  ; DS:49D2 int[]
        PUBLIC  _g_49dc
_g_49dc DW      OFFSET DGROUP:S_47f1     ; DS:49DC char *
        PUBLIC  _g_49de
_g_49de DW      OFFSET DGROUP:_g_47fe    ; DS:49DE char *
        PUBLIC  _g_49e0
_g_49e0 DW      143 DUP (0)              ; DS:49E0 int[]

        .CODE S0000_TEXT

; F_02B9A @ 0000:2B9A (linear 0x02B9A, 19 bytes)

        PUBLIC  _F_02B9A
_F_02B9A  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        mov     al,[bp+6]
        mov     _g_47ea,al
        mov     al,[bp+8]
        mov     _g_47eb,al
        pop     ax
        pop     bp
        ret
_F_02B9A  ENDP

; set_g47ec_47ed @ 0000:2BAD (linear 0x02BAD, 19 bytes)

        PUBLIC  _set_g47ec_47ed
_set_g47ec_47ed  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        mov     al,[bp+6]
        mov     _g_47ec,al
        mov     al,[bp+8]
        mov     _g_47ed,al
        pop     ax
        pop     bp
        ret
_set_g47ec_47ed  ENDP

; F_02BC0 @ 0000:2BC0 (linear 0x02BC0, 34 bytes)

        PUBLIC  _F_02BC0
_F_02BC0  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        sub     ax,ax
        mov     es,ax
        cli
        mov     al,es:[0410h]
        and     al,0CFh
        or      al,_g_47ea
        mov     es:[0410h],al
        mov     ah,0
        mov     al,_g_47eb
        sti
        int     10h
        pop     ax
        pop     bp
        ret
_F_02BC0  ENDP

; F_02BE2 @ 0000:2BE2 (linear 0x02BE2, 34 bytes)

        PUBLIC  _F_02BE2
_F_02BE2  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        sub     ax,ax
        mov     es,ax
        cli
        mov     al,es:[0410h]
        and     al,0CFh
        or      al,_g_47ec
        mov     es:[0410h],al
        mov     ah,0
        mov     al,_g_47ed
        sti
        int     10h
        pop     ax
        pop     bp
        ret
_F_02BE2  ENDP

        END
