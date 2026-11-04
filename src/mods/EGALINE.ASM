        .MODEL MEDIUM

        .DATA
        EXTRN   _av787at:WORD
        EXTRN   _g_5c1e:WORD

        .CODE S21A0_TEXT

; egaline_draw @ 21A0:23BC (linear 0x23DBC, 370 bytes)
; Bresenham line in the 640x200 planar EGA screen (40 bytes per row),
; egaline_draw(x1, y1, x2, y2, color), using write mode 3 style set/reset
; (GC regs: 0 = set/reset, 1 = enable set/reset, 3 = function, 8 = bit mask).
; Vertical lines are done pixel by pixel, horizontal lines with
; whole-byte writes through a latched copy.

        PUBLIC  _egaline_draw
_egaline_draw  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     dx,3CEh
        mov     ah,[bp+14]              ; color
        xor     al,al
        out     dx,ax
        mov     ax,0F01h
        out     dx,ax
        mov     ax,_g_5c1e
        mov     al,3
        out     dx,ax
        mov     si,40                   ; row stride
        mov     cx,[bp+10]
        sub     cx,[bp+6]               ; dx = x2 - x1
        je      vertical
        jns     x_ordered
        neg     cx
        mov     bx,[bp+10]
        xchg    [bp+6],bx
        mov     [bp+10],bx
        mov     bx,[bp+12]
        xchg    [bp+8],bx
        mov     [bp+12],bx
x_ordered:
        mov     bx,[bp+12]
        sub     bx,[bp+8]               ; dy = y2 - y1
        je      horizontal
        jns     y_down
        neg     bx
        neg     si
y_down:
        mov     [bp-2],si
        mov     WORD PTR [bp-8],OFFSET x_major
        cmp     bx,cx
        jle     x_is_major
        mov     WORD PTR [bp-8],OFFSET y_major
        xchg    bx,cx
x_is_major:
        shl     bx,1
        mov     [bp-4],bx
        sub     bx,cx
        mov     si,bx
        sub     bx,cx
        mov     [bp-6],bx
        push    cx
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _egaline_pixel_addr
        mov     di,bx
        shl     ah,cl
        mov     bl,ah
        mov     al,8
        pop     cx
        inc     cx
        jmp     WORD PTR [bp-8]
vertical:
        mov     ax,[bp+8]
        mov     bx,[bp+12]
        mov     cx,bx
        sub     cx,ax
        jge     v_ordered
        neg     cx
        mov     ax,bx
v_ordered:
        inc     cx
        mov     bx,[bp+6]
        push    cx
        call    _egaline_pixel_addr
        shl     ah,cl
        mov     al,8
        out     dx,ax
        pop     cx
v_loop:
        or      BYTE PTR es:[bx],al
        add     bx,si
        loop    v_loop
        jmp     done
horizontal:
        push    ds
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _egaline_pixel_addr
        mov     di,bx
        mov     dh,ah
        not     dh
        shl     dh,cl
        not     dh
        mov     cx,[bp+10]
        and     cl,7
        xor     cl,7
        mov     dl,0FFh
        shl     dl,cl
        mov     ax,[bp+10]
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
        js      h_middle
        or      cx,cx
        jne     h_first
        and     bl,bh
        jmp     SHORT h_last
h_first:
        mov     ah,bh
        out     dx,ax
        movsb
        dec     cx
h_middle:
        mov     ah,0FFh
        out     dx,ax
        rep     movsb
h_last:
        mov     ah,bl
        out     dx,ax
        movsb
        pop     ds
        jmp     SHORT done
x_major:
        mov     ah,bl
xm_or:
        or      ah,bl
        ror     bl,1
        jb      xm_next_byte
        or      si,si
        jns     xm_diag
        add     si,[bp-4]
        loop    xm_or
        out     dx,ax
        or      BYTE PTR es:[di],al
        jmp     SHORT done
xm_diag:
        add     si,[bp-6]
        out     dx,ax
        or      BYTE PTR es:[di],al
        add     di,[bp-2]
        loop    x_major
        jmp     SHORT done
xm_next_byte:
        out     dx,ax
        or      BYTE PTR es:[di],al
        inc     di
        or      si,si
        jns     xm_next_diag
        add     si,[bp-4]
        loop    x_major
        jmp     SHORT done
xm_next_diag:
        add     si,[bp-6]
        add     di,[bp-2]
        loop    x_major
        jmp     SHORT done
y_major:
        mov     bx,[bp-2]
ym_loop:
        out     dx,ax
        or      BYTE PTR es:[di],al
        add     di,bx
        or      si,si
        jns     ym_diag
        add     si,[bp-4]
        loop    ym_loop
        jmp     SHORT done
ym_diag:
        add     si,[bp-6]
        ror     ah,1
        adc     di,0
        loop    ym_loop
done:
        xor     ax,ax
        out     dx,ax
        inc     ax
        out     dx,ax
        mov     al,3
        out     dx,ax
        mov     ax,0FF08h
        out     dx,ax
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_egaline_draw  ENDP

; egaline_pixel_addr @ 21A0:252E (linear 0x23F2E, 31 bytes)
; Planar pixel address helper. In: ax = y, bx = x.
; Out: bx = y*40 + x/8, es = video segment, cl = 7 - (x & 7), ah = 1.

        PUBLIC  _egaline_pixel_addr
_egaline_pixel_addr  PROC    NEAR
        mov     cl,bl
        push    dx
        mov     dx,40
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
_egaline_pixel_addr  ENDP

; egaline_get_pixel @ 21A0:254D (linear 0x23F4D, 26 bytes)
; EGA bit-plane reader. CX selects a bit, ES:SI addresses the pixel byte;
; DX is the graphics controller port. Returns the four-plane colour in BL.

        PUBLIC  _egaline_get_pixel
_egaline_get_pixel  PROC    NEAR
        mov     ax,0304h
        mov     ch,1
        shl     ch,cl
        xor     bl,bl
plane:
        out     dx,ax
        mov     bh,es:[si]
        and     bh,ch
        shr     bh,cl
        shl     bl,1
        or      bl,bh
        dec     ah
        jge     plane
        ret
_egaline_get_pixel  ENDP

        END
