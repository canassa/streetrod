; Zero-initialized scratch Bitmap descriptor.
SCRATCH SEGMENT PARA PUBLIC 'FAR_DATA'
        PUBLIC  _g_egaPage3

_g_egaPage3 LABEL   BYTE                ; struct Bitmap far
        DW      0, 0, 0                  ; width, height, pixelBytes
        DB      30 DUP (0)               ; pad
        DD      0, 0                     ; bits, mask
        DW      0                        ; pitch
        DB      0, 0                     ; kind, alignment
SCRATCH ENDS

        END
