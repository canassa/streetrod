; F_0E5C3 @ 0000:E5C3 (linear 0x0E5C3, 14 bytes)
; BIOS equipment word: 0x30 in the video bits means monochrome.

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_0E5C3
_F_0E5C3  PROC  FAR
        int     11h
        and     ax,30h
        cmp     ax,30h
        je      mono
        mov     ax,0
mono:
        ret
_F_0E5C3  ENDP

        END
