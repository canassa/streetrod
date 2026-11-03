; F_26537 @ 2645:00E7 (linear 0x26537, 100 bytes)
; F_26537(y, colour): EGA, fill the rows from y down to the bottom of the clip
; rectangle by copying within segment g_787a with the set/reset colour (a
; latch copy of 40 bytes per row), then restore the graphics controller.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_5c1e:WORD
        EXTRN   _g_787a:WORD

        .CODE

        PUBLIC  _F_26537
_F_26537  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,[bp+6]
        cmp     _g_clip_ymax,ax
        jl      done
        mov     dx,3CEh
        mov     ah,[bp+8]
        xor     al,al
        out     dx,ax
        mov     ax,0F01h
        out     dx,ax
        mov     ax,_g_5c1e
        mov     al,3
        out     dx,ax
        push    ds
        mov     ax,_g_clip_ymax
        mov     dx,[bp+6]
        sub     ax,dx
        mov     dx,28h
        mul     dx
        mov     cx,ax
        mov     ax,_g_787a
        mov     es,ax
        push    es
        pop     ds
        mov     ax,[bp+6]
        mov     dx,28h
        mul     dx
        mov     di,ax
        mov     si,di
        mov     dx,3CEh
        mov     al,8
        mov     ah,0FFh
        out     dx,ax
        rep     movsb
        pop     ds
        xor     ax,ax
        out     dx,ax
        inc     ax
        out     dx,ax
        mov     al,3
        out     dx,ax
        mov     ax,0FF08h
        out     dx,ax
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_26537  ENDP

        END
