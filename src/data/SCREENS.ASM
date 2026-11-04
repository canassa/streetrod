; Three screen Bitmap descriptors.
        INCLUDE PC.INC
SCREENS SEGMENT PARA PUBLIC 'FAR_DATA'
        PUBLIC  _g_screens
        PUBLIC  _g_screen1

_g_screens LABEL   BYTE                  ; struct Bitmap far[]
        DW      SCREEN_WIDTH, SCREEN_HEIGHT, 8000           ; width, height, planesize
        DB      22 DUP (0), 0FFh, 0FFh, 6 DUP (0)  ; pad
        DD      EGA_VRAM, 0            ; bits, alt
        DW      EGA_PITCH                       ; pitch
        DB      0FEh, 0                  ; flags, alignment

_g_screen1 LABEL   BYTE                  ; struct Bitmap far
        DW      SCREEN_WIDTH, SCREEN_HEIGHT, 8000           ; width, height, planesize
        DB      24 DUP (0), 0FFh, 5 DUP (0)  ; pad
        DD      0A2000000h, 0            ; bits, alt
        DW      EGA_PITCH                       ; pitch
        DB      0FEh, 0                  ; flags, alignment

        DW      SCREEN_WIDTH, SCREEN_HEIGHT, 16384          ; width, height, planesize
        DB      24 DUP (0), 0FFh, 5 DUP (0)  ; pad
        DD      EGA_VRAM_PAGE1, 0            ; bits, alt
        DW      80                       ; pitch
        DB      0FDh, 0                  ; flags, alignment
SCREENS ENDS

        END
