; F_02B9A @ 0000:2B9A (linear 0x02B9A, 19 bytes)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_47ea:BYTE
        EXTRN   _g_47eb:BYTE

        .CODE

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

        END
