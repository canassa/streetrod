        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _av787at:WORD
        EXTRN   _g_egaGcFunction:WORD
        EXTRN   _g_viewRect:BYTE
        EXTRN   _g_viewClip:WORD
        EXTRN   _r_vid:WORD

        .CODE S2645_TEXT

; egaspan_begin @ 2645:0008 (linear 0x26458, 21 bytes)
; EGA/VGA: enable set/reset on all four planes (GC index 1 = 0Fh), then
; program GC index 3 (data rotate / function select) from the high byte of g_egaGcFunction.

        PUBLIC  _egaspan_begin
_egaspan_begin  PROC    FAR
        cmp     _r_vid+12,-2
        jne     done1
        mov     dx,GC_INDEX
        mov     ax,(SEQ_ALL_PLANES SHL 8) OR GC_ENABLE_SET_RESET
        out     dx,ax
        mov     ax,_g_egaGcFunction
        mov     al,GC_DATA_ROTATE
        out     dx,ax
done1:
        ret
_egaspan_begin  ENDP

; egaspan_end @ 2645:001D (linear 0x2646D, 23 bytes)
; Restore the graphics controller defaults: set/reset 0, enable set/reset 0,
; function select 0, bit mask FFh.

        PUBLIC  _egaspan_end
_egaspan_end  PROC    FAR
        cmp     _r_vid+12,-2
        jne     done2
        mov     dx,GC_INDEX
        xor     ax,ax
        out     dx,ax
        inc     ax
        out     dx,ax
        mov     al,GC_DATA_ROTATE
        out     dx,ax
        mov     ax,(GC_MASK_ALL SHL 8) OR GC_BIT_MASK
        out     dx,ax
done2:
        ret
_egaspan_end  ENDP

; egaspan_fill @ 2645:0034 (linear 0x26484, 179 bytes)
; egaspan_fill(x1, x2, y, colour): EGA horizontal line from x1 to x2 on row y,
; clipped to the clip rectangle.  Set/reset colour in GC index 0, bit mask
; (GC index 8) used for the partial first and last bytes.

        PUBLIC  _egaspan_fill
_egaspan_fill  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,[bp+0Ah]
        cmp     _g_viewClip+2,ax
        jg      reject
        cmp     _g_viewClip+4,ax
        jl      reject
        mov     ax,[bp+6]
        mov     bx,[bp+8]
        mov     cx,_g_viewClip
        mov     dx,_g_viewClip+6
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
        mov     dx,GC_INDEX
        mov     ah,[bp+0Ch]
        xor     al,al
        out     dx,ax
        push    ds
        mov     ax,[bp+0Ah]
        mov     bx,[bp+6]
        call    _egaspan_pixel_addr
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
        mov     dx,GC_INDEX
        mov     al,GC_BIT_MASK
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
_egaspan_fill  ENDP

; egaspan_fill_below @ 2645:00E7 (linear 0x26537, 100 bytes)
; egaspan_fill_below(y, colour): EGA, fill the rows from y down to the bottom of the clip
; rectangle by copying within segment av787at with the set/reset colour (a
; latch copy of 40 bytes per row), then restore the graphics controller.

        PUBLIC  _egaspan_fill_below
_egaspan_fill_below  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,[bp+6]
        cmp     _g_viewClip+4,ax
        jl      done4
        mov     dx,GC_INDEX
        mov     ah,[bp+8]
        xor     al,al
        out     dx,ax
        mov     ax,(SEQ_ALL_PLANES SHL 8) OR GC_ENABLE_SET_RESET
        out     dx,ax
        mov     ax,_g_egaGcFunction
        mov     al,GC_DATA_ROTATE
        out     dx,ax
        push    ds
        mov     ax,_g_viewClip+4
        mov     dx,[bp+6]
        sub     ax,dx
        mov     dx,EGA_PITCH
        mul     dx
        mov     cx,ax
        mov     ax,_av787at
        mov     es,ax
        push    es
        pop     ds
        mov     ax,[bp+6]
        mov     dx,EGA_PITCH
        mul     dx
        mov     di,ax
        mov     si,di
        mov     dx,GC_INDEX
        mov     al,GC_BIT_MASK
        mov     ah,0FFh
        out     dx,ax
        rep     movsb
        pop     ds
        xor     ax,ax
        out     dx,ax
        inc     ax
        out     dx,ax
        mov     al,GC_DATA_ROTATE
        out     dx,ax
        mov     ax,(GC_MASK_ALL SHL 8) OR GC_BIT_MASK
        out     dx,ax
done4:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_egaspan_fill_below  ENDP

; egaspan_pixel_addr @ 2645:014B (linear 0x2659B, 31 bytes)
; egaspan_pixel_addr(ax = y, bx = x): EGA address of a pixel.  Out: bx = y * 40 + x / 8,
; es = av787at, cl = 7 - (x & 7), ah = 1.

        PUBLIC  _egaspan_pixel_addr
_egaspan_pixel_addr  PROC    NEAR
        mov     cl,bl
        push    dx
        mov     dx,EGA_PITCH
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
_egaspan_pixel_addr  ENDP

        END
