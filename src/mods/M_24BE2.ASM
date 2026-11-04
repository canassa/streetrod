        .MODEL MEDIUM

; F_24BE2 @ 24BE:0002 (linear 0x24BE2, 201 bytes)
; F_24BE2(width, text, dest, fg, bg): draw a string with the 8x8 font in
; segment 0E9Fh into the 4-bit packed screen buffer (160 bytes per row).  The
; font has 9 bytes per glyph: a width byte, then eight row bits.  Returns the
; x position reached (the caller passes the field width and the start parity).

        EXTRN   _g_font:BYTE

        .CODE S24BE_TEXT

        PUBLIC  _F_24BE2
_F_24BE2  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,SEG _g_font
        mov     es,ax
        mov     ax,320
        sub     ax,[bp+6]
        mov     [bp-2],ax
        mov     ax,[bp+6]
        and     ax,1
        mov     [bp-4],ax
L24BE2_nextch:
        mov     bx,[bp+8]
        mov     al,[bx]
        or      al,al
        jne     L24BE2_havech
        jmp     L24BE2_done
L24BE2_havech:
        inc     WORD PTR [bp+8]
        cbw
        sub     ax,20h
        jl      L24BE2_badch
        cmp     ax,5Ah
        jle     L24BE2_goodch
L24BE2_badch:
        xor     ax,ax
L24BE2_goodch:
        mov     bx,ax
        shl     ax,1
        shl     ax,1
        shl     ax,1
        add     bx,ax
        mov     ax,0
        add     bx,ax
        mov     ax,[bp-4]
        and     ax,1
        mov     [bp-6],al
        mov     al,es:[bx]
        xor     ah,ah
        add     [bp-4],ax
        mov     cx,[bp-2]
        cmp     [bp-4],cx
        jl      L24BE2_fits
        sub     [bp-4],ax
        jmp     SHORT L24BE2_done
L24BE2_fits:
        mov     si,[bp+0Ah]
        mov     ch,al
        mov     WORD PTR [bp-8],8
L24BE2_nextrow:
        mov     di,si
        inc     bx
        mov     dh,es:[bx]
        mov     al,[di]
        mov     ah,[bp-6]
        mov     cl,ch
        mov     dl,80h
L24BE2_nextpix:
        test    dh,dl
        je      L24BE2_isbg
        or      al,[bp+0Ch]
        jmp     SHORT L24BE2_havepix
L24BE2_isbg:
        or      al,[bp+0Eh]
L24BE2_havepix:
        xor     ah,1
        jne     L24BE2_lownib
        mov     [di],al
        inc     di
        xor     al,al
        jmp     SHORT L24BE2_pixdone
L24BE2_lownib:
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
L24BE2_pixdone:
        dec     cl
        je      L24BE2_rowdone
        shr     dl,1
        jmp     SHORT L24BE2_nextpix
L24BE2_rowdone:
        mov     [di],al
        dec     WORD PTR [bp-8]
        je      L24BE2_glyphdone
        add     si,0A0h
        jmp     SHORT L24BE2_nextrow
L24BE2_glyphdone:
        mov     ax,di
        sub     ax,si
        add     [bp+0Ah],ax
        jmp     L24BE2_nextch
L24BE2_done:
        mov     ax,[bp-4]
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_24BE2  ENDP

; F_24CAB @ 24BE:00CB (linear 0x24CAB, 216 bytes)
; F_24CAB(src, dstseg, dstoff, y, height, width, flags, ...): copy a strip of
; 4-bit pixels from a linear buffer (160 bytes per L24CAB_row) into the interleaved
; (4 banks of 2000h) screen.  Handles a leading half byte (merge the low
; nibble), the whole bytes, and a trailing half byte (merge the high nibble).

        .CODE S24BE_TEXT

        PUBLIC  _F_24CAB
_F_24CAB  PROC    FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+8]
        mov     es,ax
        mov     ax,[bp+12h]
        or      ax,ax
        je      L24CAB_middle
        mov     cx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
L24CAB_lead:
        and     BYTE PTR es:[di],0F0h
        mov     al,[si]
        or      es:[di],al
        add     si,0A0h
        inc     dx
        test    dx,3
        je      L24CAB_lead_wrap
        add     di,2000h
        jmp     SHORT L24CAB_lead_next
L24CAB_lead_wrap:
        sub     di,6000h
        add     di,0A0h
L24CAB_lead_next:
        loop    L24CAB_lead
        inc     WORD PTR [bp+0Ah]
        inc     WORD PTR [bp+6]
L24CAB_middle:
        cmp     WORD PTR [bp+10h],0
        jle     L24CAB_trail
        mov     bx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
L24CAB_row:
        mov     cx,[bp+10h]
        shr     cx,1
        jnc     L24CAB_words
        movsb
        jcxz    L24CAB_rowend
L24CAB_words:
        rep     movsw
L24CAB_rowend:
        add     si,0A0h
        mov     ax,[bp+10h]
        sub     si,ax
        sub     di,ax
        inc     dx
        test    dx,3
        je      L24CAB_row_wrap
        add     di,2000h
        jmp     SHORT L24CAB_row_next
L24CAB_row_wrap:
        sub     di,6000h
        add     di,0A0h
L24CAB_row_next:
        dec     bx
        jne     L24CAB_row
        mov     ax,[bp+0Ah]
        add     ax,[bp+10h]
        mov     [bp+0Ah],ax
        mov     ax,[bp+6]
        add     ax,[bp+10h]
        mov     [bp+6],ax
L24CAB_trail:
        mov     ax,[bp+14h]
        test    ax,1
        je      L24CAB_done
        mov     cx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
L24CAB_tail:
        and     BYTE PTR es:[di],0Fh
        mov     al,[si]
        or      es:[di],al
        add     si,0A0h
        inc     dx
        test    dx,3
        je      L24CAB_tail_wrap
        add     di,2000h
        jmp     SHORT L24CAB_tail_next
L24CAB_tail_wrap:
        sub     di,6000h
        add     di,0A0h
L24CAB_tail_next:
        loop    L24CAB_tail
L24CAB_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_24CAB  ENDP

        END
