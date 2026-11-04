; Sound-effect word instruction streams.
EFFECTS SEGMENT PARA PUBLIC 'FAR_DATA'
        PUBLIC  _g_pumpSound
        PUBLIC  _g_jackSound
        PUBLIC  _g_2f93_0050
        PUBLIC  _g_partMoveSound
        PUBLIC  _g_stickerSound
        PUBLIC  _g_2f93_00b0
        PUBLIC  _g_wheelRollSound

_g_pumpSound LABEL   WORD                ; int far[]
        DW      0, 0, 1, 23A1h, 3, 0, 1, 1FBEh
        DW      3, 0, 1, 1DF6h, 3, 0, 1, 1FBEh
        DW      3, 0, 0, 1, 0, 0

_g_jackSound LABEL   WORD                ; int far[]
        DW      0, 0, 2, 8E88h, 2, 0, 2, 5F1Eh
        DW      2, 8E88h, 3, 7120h, 11h, 0, 0, 1
        DW      0, 0

_g_2f93_0050 LABEL   WORD                ; int far[]
        DW      0, 0, 2, 0FFFFh, 6, 0, 2, 0FDFFh
        DW      6, 0, 2, 0EFA2h, 6, 0, 2, 0FDFFh
        DW      6, 0, 0, 1, 0, 0

_g_partMoveSound LABEL   WORD                ; int far[]
        DW      0, 0, 3, 0AAh, 3, 9B5Ch, 3, 0AFh
        DW      3, 33C9h, 7, 0C0h, 7, 7485h, 0, 1
        DW      0, 0

_g_stickerSound LABEL   WORD                ; int far[]
        DW      0, 0, 24h, 0F8h, 0, 1, 0, 0

_g_2f93_00b0 LABEL   WORD                ; int far[]
        DW      0, 0, 7, 2E9Bh, 0, 1, 0, 0

_g_wheelRollSound LABEL   WORD                ; int far[]
        DW      0, 0, 1, 8E88h, 6, 0, 2, 0
        DW      1, 7EF6h, 6, 0, 2, 0, 1, 77D8h
        DW      6, 0, 2, 0, 1, 7EF6h, 6, 0
        DW      2, 0, 0, 1, 0, 0, 0, 0
EFFECTS ENDS

        END
