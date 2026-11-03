; F_02DB6 @ 0000:2DB6 (linear 0x02DB6, 13 bytes)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_47d0:BYTE

        .CODE

        PUBLIC  _F_02DB6
_F_02DB6  PROC  FAR
        cli
        mov     al,_g_47d0
        xor     ah,ah
        mov     _g_47d0,0
        sti
        ret
_F_02DB6  ENDP

        END
