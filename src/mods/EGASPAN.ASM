        .MODEL MEDIUM

        .DATA
        EXTRN   _av787at:WORD
        EXTRN   _g_5c1e:WORD
        EXTRN   _g_5e42:BYTE
        EXTRN   _g_clip_xmax:WORD
        EXTRN   _g_clip_xmin:WORD
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_clip_ymin:WORD
        EXTRN   _r_vid:WORD

        .CODE S2645_TEXT

; F_26458 @ 2645:0008 (linear 0x26458, 21 bytes)
; EGA/VGA: enable set/reset on all four planes (GC index 1 = 0Fh), then
; program GC index 3 (data rotate / function select) from the high byte of g_5c1e.

        PUBLIC  _F_26458
_F_26458  PROC    FAR
        cmp     _r_vid+12,-2
        jne     done1
        mov     dx,3CEh
        mov     ax,0F01h
        out     dx,ax
        mov     ax,_g_5c1e
        mov     al,3
        out     dx,ax
done1:
        ret
_F_26458  ENDP

; F_2646D @ 2645:001D (linear 0x2646D, 23 bytes)
; Restore the graphics controller defaults: set/reset 0, enable set/reset 0,
; function select 0, bit mask FFh.

        PUBLIC  _F_2646D
_F_2646D  PROC    FAR
        cmp     _r_vid+12,-2
        jne     done2
        mov     dx,3CEh
        xor     ax,ax
        out     dx,ax
        inc     ax
        out     dx,ax
        mov     al,3
        out     dx,ax
        mov     ax,0FF08h
        out     dx,ax
done2:
        ret
_F_2646D  ENDP

; F_26484 @ 2645:0034 (linear 0x26484, 179 bytes)
; F_26484(x1, x2, y, colour): EGA horizontal line from x1 to x2 on row y,
; clipped to the clip rectangle.  Set/reset colour in GC index 0, bit mask
; (GC index 8) used for the partial first and last bytes.

        PUBLIC  _F_26484
_F_26484  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,[bp+0Ah]
        cmp     _g_clip_ymin,ax
        jg      reject
        cmp     _g_clip_ymax,ax
        jl      reject
        mov     ax,[bp+6]
        mov     bx,[bp+8]
        mov     cx,_g_clip_xmin
        mov     dx,_g_clip_xmax
        cmp     cx,bx
        jg      reject
        cmp     dx,ax
        jl      reject
        jmp     SHORT inside
reject:
        jmp     SHORT done3
inside:
        cmp     ax,cx
        jg      leftok
        mov     [bp+6],cx
leftok:
        cmp     bx,dx
        jl      rightok
        mov     [bp+8],dx
rightok:
        mov     ax,[bp+6]
        cmp     ax,[bp+8]
        jle     ordered
        xchg    [bp+8],ax
        mov     [bp+6],ax
ordered:
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
        js      middle
        or      cx,cx
        jne     first
        and     bl,bh
        jmp     SHORT last
first:
        mov     ah,bh
        out     dx,ax
        movsb
        dec     cx
middle:
        mov     ah,0FFh
        out     dx,ax
        rep     movsb
last:
        mov     ah,bl
        out     dx,ax
        movsb
        pop     ds
done3:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_26484  ENDP

; F_26537 @ 2645:00E7 (linear 0x26537, 100 bytes)
; F_26537(y, colour): EGA, fill the rows from y down to the bottom of the clip
; rectangle by copying within segment av787at with the set/reset colour (a
; latch copy of 40 bytes per row), then restore the graphics controller.

        PUBLIC  _F_26537
_F_26537  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,[bp+6]
        cmp     _g_clip_ymax,ax
        jl      done4
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
        mov     ax,_av787at
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
done4:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_26537  ENDP

; F_2659B @ 2645:014B (linear 0x2659B, 31 bytes)
; F_2659B(ax = y, bx = x): EGA address of a pixel.  Out: bx = y * 40 + x / 8,
; es = av787at, cl = 7 - (x & 7), ah = 1.

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
        mov     ax,_av787at
        mov     es,ax
        and     cl,7
        xor     cl,7
        mov     ah,1
        ret
_F_2659B  ENDP

        END
