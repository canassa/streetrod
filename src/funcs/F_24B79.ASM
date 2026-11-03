; F_24B79 @ 2487:0309 (linear 0x24B79, 104 bytes)
; F_24B79(y, color): fill the lines from y down to the bottom of the clip
; window with a colour, four interlaced banks one after the other.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_clip_ymax:WORD

        .CODE
        EXTRN   _F_24A9B:NEAR

        PUBLIC  _F_24B79
_F_24B79  PROC  FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+6]               ; y
        cmp     _g_clip_ymax,ax
        jl      done
        mov     ax,[bp+8]               ; color
        mov     ah,al
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
        or      al,ah
        mov     dh,al
        mov     dl,al
        xor     si,si
bank:
        mov     ax,[bp+6]
        mov     bx,_g_clip_ymax
        mov     cx,bx
        sub     cx,ax
        shr     cx,1
        shr     cx,1
        and     ax,3
        and     bx,3
        cmp     ax,bx
        je      same_phase
        inc     cx
same_phase:
        or      cx,cx
        je      next
        mov     ax,[bp+6]
        xor     bx,bx
        push    cx
        call    _F_24A9B
        pop     ax
        mov     cl,80
        mul     cl
        mov     cx,ax
        mov     ax,dx
        mov     di,bx
        rep     stosw
next:
        cmp     si,3
        je      done
        inc     si
        inc     WORD PTR [bp+6]
        jmp     bank
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_24B79  ENDP

        END
