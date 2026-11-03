; F_26484 @ 2645:0034 (linear 0x26484, 179 bytes)
; F_26484(x1, x2, y, colour): EGA horizontal line from x1 to x2 on row y,
; clipped to the clip rectangle.  Set/reset colour in GC index 0, bit mask
; (GC index 8) used for the partial first and last bytes.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_clip_xmin:WORD
        EXTRN   _g_clip_ymin:WORD
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_clip_xmax:WORD

        .CODE
        EXTRN   _F_2659B:NEAR

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
        jmp     SHORT done
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
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_26484  ENDP

        END
