        .MODEL MEDIUM

; sr-member 0x2487A
; F_2487A @ 2487:000A (linear 0x2487A, 545 bytes)
; F_2487A(x1, y1, x2, y2, color): Bresenham line in the 320x200 16-colour screen
; (four interlaced banks of 2000h bytes, two pixels per byte), the sibling of
; line_ega. Vertical lines go pixel by pixel, L2487A_horizontal ones fill whole bytes.
; si = bank step (+-2000h), di = step from the last bank back to the next row
; (+-0A0A0h, i.e. -5F60h).

        .CODE

        PUBLIC  _F_2487A
_F_2487A  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     si,2000h
        mov     di,0A0A0h
        mov     cx,[bp+10]
        sub     cx,[bp+6]               ; dx = x2 - x1
        je      L2487A_vertical
        jns     L2487A_x_ordered
        neg     cx
        mov     bx,[bp+10]
        xchg    [bp+6],bx
        mov     [bp+10],bx
        mov     bx,[bp+12]
        xchg    [bp+8],bx
        mov     [bp+12],bx
L2487A_x_ordered:
        mov     bx,[bp+12]
        sub     bx,[bp+8]               ; dy = y2 - y1
        jne     L2487A_y_nonzero
        jmp     L2487A_horizontal
L2487A_y_nonzero:
        jns     L2487A_y_down
        neg     bx
        neg     si
        neg     di
L2487A_y_down:
        mov     [bp-2],di
        mov     WORD PTR [bp-8],OFFSET L2487A_x_major
        cmp     bx,cx
        jle     L2487A_x_is_major
        mov     WORD PTR [bp-8],OFFSET L2487A_y_major
        xchg    bx,cx
L2487A_x_is_major:
        shl     bx,1
        mov     [bp-4],bx
        sub     bx,cx
        mov     di,bx
        sub     bx,cx
        mov     [bp-6],bx
        push    cx
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _F_24A9B
        mov     al,[bp+14]              ; color
        shl     ax,cl
        mov     dx,ax
        not     dh
        pop     cx
        inc     cx
        jmp     WORD PTR [bp-8]
L2487A_vertical:
        mov     ax,[bp+8]
        mov     bx,[bp+12]
        mov     cx,bx
        sub     cx,ax
        jge     L2487A_v_ordered
        neg     cx
        mov     ax,bx
        mov     [bp+8],ax
L2487A_v_ordered:
        inc     cx
        mov     bx,[bp+6]
        push    cx
        call    _F_24A9B
        mov     al,[bp+14]
        shl     ax,cl
        not     ah
        pop     cx
L2487A_v_loop:
        and     BYTE PTR es:[bx],ah
        or      BYTE PTR es:[bx],al
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        je      L2487A_v_bank
        add     bx,si
        loop    L2487A_v_loop
        jmp     L2487A_done
L2487A_v_bank:
        add     bx,di
        loop    L2487A_v_loop
        jmp     L2487A_done
L2487A_horizontal:
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _F_24A9B
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+10]
        and     cl,1
        jne     L2487A_h_odd
        mov     dl,0F0h
L2487A_h_odd:
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
        js      L2487A_h_middle
        or      cx,cx
        jne     L2487A_h_first
        and     dl,dh
        jmp     SHORT L2487A_h_last
L2487A_h_first:
        mov     ah,al
        and     ah,dh
        not     dh
        and     BYTE PTR es:[di],dh
        or      BYTE PTR es:[di],ah
        inc     di
        dec     cx
L2487A_h_middle:
        rep stosb
L2487A_h_last:
        and     al,dl
        not     dl
        and     BYTE PTR es:[di],dl
        or      BYTE PTR es:[di],al
        jmp     L2487A_done
L2487A_x_major:
        mov     ah,es:[bx]
L2487A_xm_or:
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
        jnc     L2487A_xm_next_byte
        or      di,di
        jns     L2487A_xm_diag
        add     di,[bp-4]
        loop    L2487A_xm_or
        mov     es:[bx],ah
        jmp     L2487A_done
L2487A_xm_diag:
        add     di,[bp-6]
        mov     es:[bx],ah
        test    si,8000h
        je      L2487A_xd_down
        test    WORD PTR [bp+8],3
        je      L2487A_xd_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT L2487A_xd_next
L2487A_xd_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT L2487A_xd_next
L2487A_xd_down:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     L2487A_xd_next
        sub     bx,si
        add     bx,[bp-2]
L2487A_xd_next:
        loop    L2487A_x_major
        jmp     L2487A_done
L2487A_xm_next_byte:
        mov     es:[bx],ah
        inc     bx
        or      di,di
        jns     L2487A_xn_diag
        add     di,[bp-4]
        loop    L2487A_x_major
        jmp     L2487A_done
L2487A_xn_diag:
        add     di,[bp-6]
        test    si,8000h
        je      L2487A_xn_down
        test    WORD PTR [bp+8],3
        je      L2487A_xn_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT L2487A_xn_next
L2487A_xn_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT L2487A_xn_next
L2487A_xn_down:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     L2487A_xn_next
        sub     bx,si
        add     bx,[bp-2]
L2487A_xn_next:
        dec     cx
        je      L2487A_done
        jmp     L2487A_x_major
L2487A_y_major:
        and     BYTE PTR es:[bx],dh
        or      BYTE PTR es:[bx],dl
        test    si,8000h
        je      L2487A_y_down2
        test    WORD PTR [bp+8],3
        je      L2487A_y_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT L2487A_y_step
L2487A_y_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT L2487A_y_step
L2487A_y_down2:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     L2487A_y_step
        sub     bx,si
        add     bx,[bp-2]
L2487A_y_step:
        or      di,di
        jns     L2487A_y_diag
        add     di,[bp-4]
        loop    L2487A_y_major
        jmp     SHORT L2487A_done
L2487A_y_diag:
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
        loop    L2487A_y_major
L2487A_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_2487A  ENDP

; sr-member 0x24A9B
; F_24A9B @ 2487:022B (linear 0x24A9B, 44 bytes)
; Pixel address helper for the nibble packed 320x200 screen, interlaced in
; four banks of 80 bytes per line.  In: ax = y, bx = x.
; Out: bx = offset, es = video segment, cl = 0 or 4 (shift of the pixel in
; its byte), ah = 0Fh (pixel mask).

        .DATA
        EXTRN   _g_767c:WORD
        EXTRN   _g_787a:WORD

        .CODE

        PUBLIC  _F_24A9B
_F_24A9B  PROC  NEAR
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
        add     bx,_g_767c
        mov     ax,_g_787a
        mov     es,ax
        mov     ah,1
        and     cl,ah
        xor     cl,ah
        je      L24A9B_even
        mov     cl,4
L24A9B_even:
        mov     ah,0Fh
        ret
_F_24A9B  ENDP

; sr-member 0x24AC7
; F_24AC7 @ 2487:0257 (linear 0x24AC7, 178 bytes)
; Clipped horizontal span fill in the 16-colour planar-less (nibble packed)
; framebuffer: fill_span(x1, x2, y, color), clipped to the L24AC7_clip window.

        .DATA
        EXTRN   _g_clip_xmin:WORD
        EXTRN   _g_clip_ymin:WORD
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_clip_xmax:WORD

        .CODE
        EXTRN   F_24A9B:NEAR

        PUBLIC  _fill_span_clipped
_fill_span_clipped  PROC  FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+10]              ; y
        cmp     _g_clip_ymin,ax
        jg      L24AC7_done_jmp
        cmp     _g_clip_ymax,ax
        jl      L24AC7_done_jmp
        mov     ax,[bp+6]               ; x1
        mov     bx,[bp+8]               ; x2
        mov     cx,_g_clip_xmin
        mov     dx,_g_clip_xmax
        cmp     cx,bx
        jg      L24AC7_done_jmp
        cmp     dx,ax
        jl      L24AC7_done_jmp
        jmp     short L24AC7_clip
L24AC7_done_jmp:
        jmp     short L24AC7_done
L24AC7_clip:
        cmp     ax,cx
        jg      L24AC7_x1_ok
        mov     [bp+6],cx
L24AC7_x1_ok:
        cmp     bx,dx
        jl      L24AC7_x2_ok
        mov     [bp+8],dx
L24AC7_x2_ok:
        mov     ax,[bp+6]
        cmp     ax,[bp+8]
        jle     L24AC7_ordered
        xchg    ax,[bp+8]
        mov     [bp+6],ax
L24AC7_ordered:
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        call    F_24A9B
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+8]
        and     cl,1
        jne     L24AC7_odd_end
        mov     dl,0F0h
L24AC7_odd_end:
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
        js      L24AC7_edge_mask
        or      cx,cx
        jne     L24AC7_first_byte
        and     dl,dh
        jmp     short L24AC7_last_byte
L24AC7_first_byte:
        mov     ah,al
        and     ah,dh
        not     dh
        and     es:[di],dh
        or      es:[di],ah
        inc     di
        dec     cx
L24AC7_edge_mask:
        rep stosb
L24AC7_last_byte:
        and     al,dl
        not     dl
        and     es:[di],dl
        or      es:[di],al
L24AC7_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_fill_span_clipped  ENDP

; sr-member 0x24B79
; F_24B79 @ 2487:0309 (linear 0x24B79, 104 bytes)
; F_24B79(y, color): fill the lines from y down to the bottom of the clip
; window with a colour, four interlaced banks one after the other.

        .DATA
        EXTRN   _g_clip_ymax:WORD

        .CODE

        PUBLIC  _F_24B79
_F_24B79  PROC  FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+6]               ; y
        cmp     _g_clip_ymax,ax
        jl      L24B79_done
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
L24B79_bank:
        mov     ax,[bp+6]
        mov     bx,_g_clip_ymax
        mov     cx,bx
        sub     cx,ax
        shr     cx,1
        shr     cx,1
        and     ax,3
        and     bx,3
        cmp     ax,bx
        je      L24B79_same_phase
        inc     cx
L24B79_same_phase:
        or      cx,cx
        je      L24B79_next
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
L24B79_next:
        cmp     si,3
        je      L24B79_done
        inc     si
        inc     WORD PTR [bp+6]
        jmp     L24B79_bank
L24B79_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_24B79  ENDP
        END
