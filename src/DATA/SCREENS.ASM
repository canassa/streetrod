; Three screen Bitmap descriptors.
        INCLUDE PC.INC
        INCLUDE MAIN.INC
SCREENS SEGMENT PARA PUBLIC 'FAR_DATA'
        PUBLIC  _g_screens
        PUBLIC  _g_screen1

_g_screens LABEL   BYTE                  ; struct Bitmap far[]
        DW      SCREEN_WIDTH, SCREEN_HEIGHT, 8000           ; width, height, pixelBytes
        DB      22 DUP (0), 0FFh, 0FFh, 6 DUP (0)  ; pad
        DD      EGA_VRAM, 0            ; bits, mask
        DW      EGA_PITCH                       ; pitch
        DB      VID_EGA, 0                  ; kind, alignment

_g_screen1 LABEL   BYTE                  ; struct Bitmap far
        DW      SCREEN_WIDTH, SCREEN_HEIGHT, 8000           ; width, height, pixelBytes
        DB      24 DUP (0), 0FFh, 5 DUP (0)  ; pad
        DD      0A2000000h, 0            ; bits, mask
        DW      EGA_PITCH                       ; pitch
        DB      VID_EGA, 0                  ; kind, alignment

        DW      SCREEN_WIDTH, SCREEN_HEIGHT, 16384          ; width, height, pixelBytes
        DB      24 DUP (0), 0FFh, 5 DUP (0)  ; pad
        DD      EGA_VRAM_PAGE1, 0            ; bits, mask
        DW      80                       ; pitch
        DB      VID_CGA, 0                  ; kind, alignment
SCREENS ENDS

        END
