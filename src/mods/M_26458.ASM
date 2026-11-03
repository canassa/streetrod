        .MODEL MEDIUM

; sr-member 0x26458
; F_26458 @ 2645:0008 (linear 0x26458, 21 bytes)
; EGA/VGA: enable set/reset on all four planes (GC index 1 = 0Fh), then
; program GC index 3 (data rotate / function select) from the high byte of g_5c1e.

        .DATA
        EXTRN   _g_5e42:BYTE
        EXTRN   _g_clip_ymin:WORD
        EXTRN   _g_clip_xmin:WORD
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_clip_xmax:WORD
        EXTRN   _g_vid:WORD
        EXTRN   _g_5c1e:WORD

        .CODE

        PUBLIC  _F_26458
_F_26458  PROC    FAR
        cmp     _g_vid+12,-2
        jne     L26458_done
        mov     dx,3CEh
        mov     ax,0F01h
        out     dx,ax
        mov     ax,_g_5c1e
        mov     al,3
        out     dx,ax
L26458_done:
        ret
_F_26458  ENDP

; sr-member 0x2646D
; F_2646D @ 2645:001D (linear 0x2646D, 23 bytes)
; Restore the graphics controller defaults: set/reset 0, enable set/reset 0,
; function select 0, bit mask FFh.

        .DATA
        EXTRN   _g_vid:WORD

        .CODE

        PUBLIC  _F_2646D
_F_2646D  PROC    FAR
        cmp     _g_vid+12,-2
        jne     L2646D_done
        mov     dx,3CEh
        xor     ax,ax
        out     dx,ax
        inc     ax
        out     dx,ax
        mov     al,3
        out     dx,ax
        mov     ax,0FF08h
        out     dx,ax
L2646D_done:
        ret
_F_2646D  ENDP

; sr-member 0x26484
; F_26484 @ 2645:0034 (linear 0x26484, 179 bytes)
; F_26484(x1, x2, y, colour): EGA horizontal line from x1 to x2 on row y,
; clipped to the clip rectangle.  Set/reset colour in GC index 0, bit mask
; (GC index 8) used for the partial L26484_first and L26484_last bytes.

        .DATA

        .CODE

        PUBLIC  _F_26484
_F_26484  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,[bp+0Ah]
        cmp     _g_clip_ymin,ax
        jg      L26484_reject
        cmp     _g_clip_ymax,ax
        jl      L26484_reject
        mov     ax,[bp+6]
        mov     bx,[bp+8]
        mov     cx,_g_clip_xmin
        mov     dx,_g_clip_xmax
        cmp     cx,bx
        jg      L26484_reject
        cmp     dx,ax
        jl      L26484_reject
        jmp     SHORT L26484_inside
L26484_reject:
        jmp     SHORT L26484_done
L26484_inside:
        cmp     ax,cx
        jg      L26484_leftok
        mov     [bp+6],cx
L26484_leftok:
        cmp     bx,dx
        jl      L26484_rightok
        mov     [bp+8],dx
L26484_rightok:
        mov     ax,[bp+6]
        cmp     ax,[bp+8]
        jle     L26484_ordered
        xchg    [bp+8],ax
        mov     [bp+6],ax
L26484_ordered:
        mov     dx,3CEh
        mov     ah,[bp+0Ch]
        xor     al,al
        out     dx,ax
        push    ds
        mov     ax,[bp+0Ah]
        mov     bx,[bp+6]
        call    _F_2659B
        mov     di,bx
        mov     dh,ah
        not     dh
        shl     dh,cl
        not     dh
        mov     cx,[bp+8]
        and     cl,7
        xor     cl,7
        mov     dl,0FFh
        shl     dl,cl
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        mov     cl,3
        shr     ax,cl
        shr     bx,cl
        mov     cx,ax
        sub     cx,bx
        mov     bx,dx
        mov     dx,3CEh
        mov     al,8
        push    es
        pop     ds
        mov     si,di
        or      bh,bh
        js      L26484_middle
        or      cx,cx
        jne     L26484_first
        and     bl,bh
        jmp     SHORT L26484_last
L26484_first:
        mov     ah,bh
        out     dx,ax
        movsb
        dec     cx
L26484_middle:
        mov     ah,0FFh
        out     dx,ax
        rep     movsb
L26484_last:
        mov     ah,bl
        out     dx,ax
        movsb
        pop     ds
L26484_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_26484  ENDP

; sr-member 0x26537
; F_26537 @ 2645:00E7 (linear 0x26537, 100 bytes)
; F_26537(y, colour): EGA, fill the rows from y down to the bottom of the clip
; rectangle by copying within segment g_787a with the set/reset colour (a
; latch copy of 40 bytes per row), then restore the graphics controller.

        .DATA
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
        jl      L26537_done
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
L26537_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_26537  ENDP

; sr-member 0x2659B
; F_2659B @ 2645:014B (linear 0x2659B, 31 bytes)
; F_2659B(ax = y, bx = x): EGA address of a pixel.  Out: bx = y * 40 + x / 8,
; es = g_787a, cl = 7 - (x & 7), ah = 1.

        .DATA
        EXTRN   _g_787a:WORD

        .CODE

        PUBLIC  _F_2659B
_F_2659B  PROC    NEAR
        mov     cl,bl
        push    dx
        mov     dx,28h
        mul     dx
        pop     dx
        shr     bx,1
        shr     bx,1
        shr     bx,1
        add     bx,ax
        mov     ax,_g_787a
        mov     es,ax
        and     cl,7
        xor     cl,7
        mov     ah,1
        ret
_F_2659B  ENDP
        END
