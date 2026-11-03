; F_02BE2 @ 0000:2BC0 (linear 0x02BE2, 34 bytes)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_47ec:BYTE
        EXTRN   _g_47ed:BYTE

        .CODE

        PUBLIC  _F_02BE2
_F_02BE2  PROC  FAR
        push    bp
        mov     bp,sp
        push    ax
        sub     ax,ax
        mov     es,ax
        cli
        mov     al,es:[0410h]
        and     al,0CFh
        or      al,_g_47ec
        mov     es:[0410h],al
        mov     ah,0
        mov     al,_g_47ed
        sti
        int     10h
        pop     ax
        pop     bp
        ret
_F_02BE2  ENDP

        END
