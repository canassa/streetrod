; Screen Bitmap descriptor.
SCREEN2 SEGMENT PARA PUBLIC 'FAR_DATA'
        PUBLIC  _g_2fa3_0000

_g_2fa3_0000 LABEL   BYTE                ; struct Bitmap far
        DW      320, 200, 8000           ; width, height, planesize
        DB      24 DUP (0), 0FFh, 5 DUP (0)  ; pad
        DD      0A0000000h, 0            ; bits, alt
        DW      40                       ; pitch
        DB      0FEh, 0                  ; flags, alignment
SCREEN2 ENDS

        END
