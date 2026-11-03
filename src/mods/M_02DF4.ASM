        .MODEL MEDIUM

; sr-member 0x02DF4
; F_02DF4 @ 0000:2DF4 (linear 0x02DF4, 159 bytes)

        .DATA
        EXTRN   _g_4718:WORD
        EXTRN   _g_471c:BYTE
        EXTRN   _g_472a:WORD
        EXTRN   _g_472c:WORD
        EXTRN   _g_4732:BYTE
        EXTRN   _g_4736:BYTE
        EXTRN   _g_4738:WORD
        EXTRN   _g_473a:WORD
        EXTRN   _g_47e8:WORD

        .CODE

        PUBLIC  _F_02DF4
_F_02DF4  PROC  FAR
        push    ax
        push    bx
        push    cx
        push    dx
        push    di
        push    bp
        push    ds
        push    es
        mov     ax,DGROUP
        mov     ds,ax
        mov     es,ax
        mov     _g_47e8,sp
        cli
        mov     dx,201h
        mov     cx,400h
        mov     bx,3
        out     dx,al
        out     dx,al
L02DF4_poll:   in      al,dx
        and     al,bl
        cmp     al,bl
        loope   L02DF4_poll
        jcxz    L02DF4_timeout
        xor     al,bl
        push    ax
        push    cx
        dec     cx
        xor     bl,al
        jne     L02DF4_poll
        sti
        mov     dl,2
        mov     di,OFFSET _g_4718
L02DF4_next:   pop     bx
        sub     bx,400h
        neg     bx
        mov     bp,bx
        pop     ax
        mov     cx,2
L02DF4_axis:   shr     al,1
        jae     L02DF4_skip
        mov     bx,bp
        sub     bx,[di+6]
        mov     [di],bx
        dec     dl
L02DF4_skip:   inc     di
        inc     di
        loop    L02DF4_axis
        sub     di,4
        or      dl,dl
        jne     L02DF4_next
        mov     dx,201h
        in      al,dx
        not     al
        mov     cl,4
        shr     al,cl
        and     al,3
        mov     _g_471c,al
        je      L02DF4_nobtn
        cmp     _g_4732,0
        jne     L02DF4_held
        mov     _g_4736,al
        mov     ax,_g_472a
        mov     _g_4738,ax
        mov     ax,_g_472c
        mov     _g_473a,ax
L02DF4_held:   or      _g_4732,2
        jmp     SHORT L02DF4_done
L02DF4_nobtn:  and     _g_4732,0FDh
L02DF4_done:   pop     es
        pop     ds
        pop     bp
        pop     di
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret
L02DF4_timeout: sti
        mov     sp,_g_47e8
        jmp     L02DF4_done
_F_02DF4  ENDP
        END
