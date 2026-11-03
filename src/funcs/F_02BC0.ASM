; F_02BC0 @ 0000:2BC0 (linear 0x02BC0, 34 bytes)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_47ea:BYTE
        EXTRN   _g_47eb:BYTE

        .CODE

        PUBLIC  _F_02BC0
_F_02BC0  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        sub     ax,ax
        mov     es,ax
        cli
        mov     al,es:[0410h]
        and     al,0CFh
        or      al,_g_47ea
        mov     es:[0410h],al
        mov     ah,0
        mov     al,_g_47eb
        sti
        int     10h
        pop     ax
        pop     bp
        ret
_F_02BC0  ENDP

        END
