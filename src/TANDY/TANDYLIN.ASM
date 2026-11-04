        .MODEL MEDIUM

        .DATA
        EXTRN   _g_targetPixelOffset:WORD
        EXTRN   _g_videoAddrSegment:WORD
        EXTRN   _g_viewRect:BYTE
        EXTRN   _g_viewClip:WORD

        .CODE S2487_TEXT

; tl_draw @ 2487:000A (linear 0x2487A, 545 bytes)
; tl_draw(x1, y1, x2, y2, color): Bresenham line in the 320x200 16-colour screen
; (four interlaced banks of 2000h bytes, two pixels per byte), the sibling of
; el_draw. Vertical lines go pixel by pixel, horizontal ones fill whole bytes.
; si = bank step (+-2000h), di = step from the last bank back to the next row
; (+-0A0A0h, i.e. -5F60h).

        PUBLIC  _tl_draw
_tl_draw  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     si,2000h
        mov     di,0A0A0h
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
        jne     y_nonzero
        jmp     horizontal
y_nonzero:
        jns     y_down
        neg     bx
        neg     si
        neg     di
y_down:
        mov     [bp-2],di
        mov     WORD PTR [bp-8],OFFSET x_major
        cmp     bx,cx
        jle     x_is_major
        mov     WORD PTR [bp-8],OFFSET y_major
        xchg    bx,cx
x_is_major:
        shl     bx,1
        mov     [bp-4],bx
        sub     bx,cx
        mov     di,bx
        sub     bx,cx
        mov     [bp-6],bx
        push    cx
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _tl_pixel_addr
        mov     al,[bp+14]              ; color
        shl     ax,cl
        mov     dx,ax
        not     dh
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
        mov     [bp+8],ax
v_ordered:
        inc     cx
        mov     bx,[bp+6]
        push    cx
        call    _tl_pixel_addr
        mov     al,[bp+14]
        shl     ax,cl
        not     ah
        pop     cx
v_loop:
        and     BYTE PTR es:[bx],ah
        or      BYTE PTR es:[bx],al
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        je      v_bank
        add     bx,si
        loop    v_loop
        jmp     done1
v_bank:
        add     bx,di
        loop    v_loop
        jmp     done1
horizontal:
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _tl_pixel_addr
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+10]
        and     cl,1
        jne     h_odd
        mov     dl,0F0h
h_odd:
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        shr     ax,1
        shr     bx,1
        mov     cx,ax
        sub     cx,bx
        mov     al,[bp+14]
        mov     ah,al
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
        or      al,ah
        or      dh,dh
        js      h_middle
        or      cx,cx
        jne     h_first
        and     dl,dh
        jmp     SHORT h_last
h_first:
        mov     ah,al
        and     ah,dh
        not     dh
        and     BYTE PTR es:[di],dh
        or      BYTE PTR es:[di],ah
        inc     di
        dec     cx
h_middle:
        rep stosb
h_last:
        and     al,dl
        not     dl
        and     BYTE PTR es:[di],dl
        or      BYTE PTR es:[di],al
        jmp     done1
x_major:
        mov     ah,es:[bx]
xm_or:
        and     ah,dh
        or      ah,dl
        ror     dl,1
        ror     dl,1
        ror     dl,1
        ror     dl,1
        ror     dh,1
        ror     dh,1
        ror     dh,1
        ror     dh,1
        jnc     xm_next_byte
        or      di,di
        jns     xm_diag
        add     di,[bp-4]
        loop    xm_or
        mov     es:[bx],ah
        jmp     done1
xm_diag:
        add     di,[bp-6]
        mov     es:[bx],ah
        test    si,8000h
        je      xd_down
        test    WORD PTR [bp+8],3
        je      xd_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT xd_next
xd_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT xd_next
xd_down:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     xd_next
        sub     bx,si
        add     bx,[bp-2]
xd_next:
        loop    x_major
        jmp     done1
xm_next_byte:
        mov     es:[bx],ah
        inc     bx
        or      di,di
        jns     xn_diag
        add     di,[bp-4]
        loop    x_major
        jmp     done1
xn_diag:
        add     di,[bp-6]
        test    si,8000h
        je      xn_down
        test    WORD PTR [bp+8],3
        je      xn_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT xn_next
xn_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT xn_next
xn_down:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     xn_next
        sub     bx,si
        add     bx,[bp-2]
xn_next:
        dec     cx
        je      done1
        jmp     x_major
y_major:
        and     BYTE PTR es:[bx],dh
        or      BYTE PTR es:[bx],dl
        test    si,8000h
        je      y_down2
        test    WORD PTR [bp+8],3
        je      y_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT y_step
y_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT y_step
y_down2:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     y_step
        sub     bx,si
        add     bx,[bp-2]
y_step:
        or      di,di
        jns     y_diag
        add     di,[bp-4]
        loop    y_major
        jmp     SHORT done1
y_diag:
        add     di,[bp-6]
        ror     dl,1
        ror     dl,1
        ror     dl,1
        ror     dl,1
        ror     dh,1
        ror     dh,1
        ror     dh,1
        ror     dh,1
        cmc
        adc     bx,0
        loop    y_major
done1:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_tl_draw  ENDP

; tl_pixel_addr @ 2487:022B (linear 0x24A9B, 44 bytes)
; Pixel address helper for the nibble packed 320x200 screen, interlaced in
; four banks of 80 bytes per line.  In: ax = y, bx = x.
; Out: bx = offset, es = video segment, cl = 0 or 4 (shift of the pixel in
; its byte), ah = 0Fh (pixel mask).

        PUBLIC  _tl_pixel_addr
_tl_pixel_addr  PROC  NEAR
        mov     cl,bl
        xchg    ah,al
        shr     ax,1
        shr     ax,1
        add     bh,al
        xor     al,al
        add     bx,ax
        shr     ax,1
        shr     ax,1
        add     bx,ax
        shr     bx,1
        add     bx,_g_targetPixelOffset
        mov     ax,_g_videoAddrSegment
        mov     es,ax
        mov     ah,1
        and     cl,ah
        xor     cl,ah
        je      even1
        mov     cl,4
even1:
        mov     ah,0Fh
        ret
_tl_pixel_addr  ENDP

; tl_span @ 2487:0257 (linear 0x24AC7, 178 bytes)
; Clipped horizontal span fill in the 16-colour planar-less (nibble packed)
; framebuffer: fill_span(x1, x2, y, color), clipped to the clip window.

        PUBLIC  _tl_span
_tl_span  PROC  FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+10]              ; y
        cmp     _g_viewClip+2,ax
        jg      done_jmp
        cmp     _g_viewClip+4,ax
        jl      done_jmp
        mov     ax,[bp+6]               ; x1
        mov     bx,[bp+8]               ; x2
        mov     cx,_g_viewClip
        mov     dx,_g_viewClip+6
        cmp     cx,bx
        jg      done_jmp
        cmp     dx,ax
        jl      done_jmp
        jmp     short clip
done_jmp:
        jmp     short done2
clip:
        cmp     ax,cx
        jg      x1_ok
        mov     [bp+6],cx
x1_ok:
        cmp     bx,dx
        jl      x2_ok
        mov     [bp+8],dx
x2_ok:
        mov     ax,[bp+6]
        cmp     ax,[bp+8]
        jle     ordered
        xchg    ax,[bp+8]
        mov     [bp+6],ax
ordered:
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        call    _tl_pixel_addr
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+8]
        and     cl,1
        jne     odd_end
        mov     dl,0F0h
odd_end:
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        shr     ax,1
        shr     bx,1
        mov     cx,ax
        sub     cx,bx
        mov     al,[bp+12]
        mov     ah,al
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
        or      al,ah
        or      dh,dh
        js      edge_mask
        or      cx,cx
        jne     first_byte
        and     dl,dh
        jmp     short last_byte
first_byte:
        mov     ah,al
        and     ah,dh
        not     dh
        and     es:[di],dh
        or      es:[di],ah
        inc     di
        dec     cx
edge_mask:
        rep stosb
last_byte:
        and     al,dl
        not     dl
        and     es:[di],dl
        or      es:[di],al
done2:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_tl_span  ENDP

; tl_fill_below @ 2487:0309 (linear 0x24B79, 104 bytes)
; tl_fill_below(y, color): fill the lines from y down to the bottom of the clip
; window with a colour, four interlaced banks one after the other.

        PUBLIC  _tl_fill_below
_tl_fill_below  PROC  FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+6]               ; y
        cmp     _g_viewClip+4,ax
        jl      done3
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
        mov     bx,_g_viewClip+4
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
        call    _tl_pixel_addr
        pop     ax
        mov     cl,80
        mul     cl
        mov     cx,ax
        mov     ax,dx
        mov     di,bx
        rep     stosw
next:
        cmp     si,3
        je      done3
        inc     si
        inc     WORD PTR [bp+6]
        jmp     bank
done3:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_tl_fill_below  ENDP

        END
