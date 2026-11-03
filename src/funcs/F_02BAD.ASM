; set_g47ec_47ed @ 0000:2BAD (linear 0x02BAD, 19 bytes)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_47ec:BYTE
        EXTRN   _g_47ed:BYTE

        .CODE

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

        END
