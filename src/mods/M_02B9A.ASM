        .MODEL MEDIUM

; sr-member 0x02B9A
; F_02B9A @ 0000:2B9A (linear 0x02B9A, 19 bytes)

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

; sr-member 0x02BAD
; set_g47ec_47ed @ 0000:2BAD (linear 0x02BAD, 19 bytes)

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

; sr-member 0x02BC0
; F_02BC0 @ 0000:2BC0 (linear 0x02BC0, 34 bytes)

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

; sr-member 0x02BE2
; F_02BE2 @ 0000:2BC0 (linear 0x02BE2, 34 bytes)

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
