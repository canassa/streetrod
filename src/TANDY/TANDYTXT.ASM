        .MODEL MEDIUM
        INCLUDE PC.INC

        EXTRN   _g_font:BYTE

        .CODE S24BE_TEXT


; tt_render(width, text, dest, fg, bg): draw a string with the 8x8 font in
; segment 0E9Fh into the 4-bit packed screen buffer (160 bytes per row).  The
; font has 9 bytes per glyph: a width byte, then eight row bits.  Returns the
; x position reached (the caller passes the field width and the start parity).

        PUBLIC  _tt_render
_tt_render  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,SEG _g_font
        mov     es,ax
        mov     ax,SCREEN_WIDTH
        sub     ax,[bp+6]
        mov     [bp-2],ax
        mov     ax,[bp+6]
        and     ax,1
        mov     [bp-4],ax
nextch:
        mov     bx,[bp+8]
        mov     al,[bx]
        or      al,al
        jne     havech
        jmp     done1
havech:
        inc     WORD PTR [bp+8]
        cbw
        sub     ax,20h
        jl      badch
        cmp     ax,5Ah
        jle     goodch
badch:
        xor     ax,ax
goodch:
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
        jl      fits
        sub     [bp-4],ax
        jmp     SHORT done1
fits:
        mov     si,[bp+0Ah]
        mov     ch,al
        mov     WORD PTR [bp-8],8
nextrow:
        mov     di,si
        inc     bx
        mov     dh,es:[bx]
        mov     al,[di]
        mov     ah,[bp-6]
        mov     cl,ch
        mov     dl,80h
nextpix:
        test    dh,dl
        je      isbg
        or      al,[bp+0Ch]
        jmp     SHORT havepix
isbg:
        or      al,[bp+0Eh]
havepix:
        xor     ah,1
        jne     lownib
        mov     [di],al
        inc     di
        xor     al,al
        jmp     SHORT pixdone
lownib:
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
pixdone:
        dec     cl
        je      rowdone
        shr     dl,1
        jmp     SHORT nextpix
rowdone:
        mov     [di],al
        dec     WORD PTR [bp-8]
        je      glyphdone
        add     si,0A0h
        jmp     SHORT nextrow
glyphdone:
        mov     ax,di
        sub     ax,si
        add     [bp+0Ah],ax
        jmp     nextch
done1:
        mov     ax,[bp-4]
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_tt_render  ENDP


; tt_copy(src, dstseg, dstoff, y, height, width, flags, ...): copy a strip of
; 4-bit pixels from a linear buffer (160 bytes per row) into the interleaved
; (4 banks of 2000h) screen.  Handles a leading half byte (merge the low
; nibble), the whole bytes, and a trailing half byte (merge the high nibble).

        PUBLIC  _tt_copy
_tt_copy  PROC    FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+8]
        mov     es,ax
        mov     ax,[bp+12h]
        or      ax,ax
        je      middle
        mov     cx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
lead:
        and     BYTE PTR es:[di],0F0h
        mov     al,[si]
        or      es:[di],al
        add     si,0A0h
        inc     dx
        test    dx,3
        je      lead_wrap
        add     di,2000h
        jmp     SHORT lead_next
lead_wrap:
        sub     di,6000h
        add     di,0A0h
lead_next:
        loop    lead
        inc     WORD PTR [bp+0Ah]
        inc     WORD PTR [bp+6]
middle:
        cmp     WORD PTR [bp+10h],0
        jle     trail
        mov     bx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
row:
        mov     cx,[bp+10h]
        shr     cx,1
        jnc     words
        movsb
        jcxz    rowend
words:
        rep     movsw
rowend:
        add     si,0A0h
        mov     ax,[bp+10h]
        sub     si,ax
        sub     di,ax
        inc     dx
        test    dx,3
        je      row_wrap
        add     di,2000h
        jmp     SHORT row_next
row_wrap:
        sub     di,6000h
        add     di,0A0h
row_next:
        dec     bx
        jne     row
        mov     ax,[bp+0Ah]
        add     ax,[bp+10h]
        mov     [bp+0Ah],ax
        mov     ax,[bp+6]
        add     ax,[bp+10h]
        mov     [bp+6],ax
trail:
        mov     ax,[bp+14h]
        test    ax,1
        je      done2
        mov     cx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
tail:
        and     BYTE PTR es:[di],0Fh
        mov     al,[si]
        or      es:[di],al
        add     si,0A0h
        inc     dx
        test    dx,3
        je      tail_wrap
        add     di,2000h
        jmp     SHORT tail_next
tail_wrap:
        sub     di,6000h
        add     di,0A0h
tail_next:
        loop    tail
done2:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_tt_copy  ENDP

        END
