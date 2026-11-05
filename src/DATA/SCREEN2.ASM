; Screen Bitmap descriptor.
        INCLUDE PC.INC
        INCLUDE MAIN.INC
SCREEN2 SEGMENT PARA PUBLIC 'FAR_DATA'
        PUBLIC  _g_egaPage0

_g_egaPage0 LABEL   BYTE                ; struct Bitmap far
        DW      SCREEN_WIDTH, SCREEN_HEIGHT, 8000           ; width, height, pixelBytes
        DB      24 DUP (0), 0FFh, 5 DUP (0)  ; pad
        DD      EGA_VRAM, 0            ; bits, mask
        DW      EGA_PITCH                       ; pitch
        DB      VID_EGA, 0                  ; kind, alignment
SCREEN2 ENDS

        END
