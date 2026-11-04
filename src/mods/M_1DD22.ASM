        .MODEL MEDIUM

; F_1DD22 @ 1C11:1C12 (linear 0x1DD22, 647 bytes)
; Draw a line with 2 bits per pixel into the interleaved (CGA/Hercules
; style) screen, Bresenham style. A horizontal line is a byte-fill with edge
; masks, a vertical one a per-line loop, anything else steps along its major
; axis (L1DD22_xmajor / L1DD22_ymajor loops, picked through the pointer kept at [bp-8]).
; Local words: [bp-2] = alternate row step, [bp-4] = 2*L1DD22_minor, [bp-6] = error
; decrement, [bp-8] = loop to L1DD22_enter, [bp-0Ah] = Hercules double-write state
; (0 = off, 2/3 = on with the parity of the current row).
; F_1DD22(x1, y1, x2, y2, colour)

        .DATA
        EXTRN   _g_0042:WORD
        ; data (recovered DGROUP layout)
        PUBLIC _g_5b56
_g_5b56 LABEL BYTE ; +5B56 char @[4]
        DB 00h
        DB 055h
        DB 0AAh
        DB 0FFh
        PUBLIC _g_5b5a
_g_5b5a LABEL BYTE ; +5B5A char @[16]
        DB 00h
        DB 00h
        DB 02h
        DB 00h
        DB 03h
        DB 01h
        DB 02h
        DB 01h
        DB 01h
        DB 01h
        DB 02h
        DB 01h
        DB 02h
        DB 01h
        DB 01h
        DB 03h
        PUBLIC _g_5b6a
_g_5b6a LABEL BYTE ; +5B6A char @[16]
        DB 00h
        DB 00h
        DB 055h
        DB 0EEh
        DB 0FFh
        DB 00h
        DB 00h
        DB 00h
        DB 055h
        DB 077h
        DB 0AAh
        DB 00h
        DB 0AAh
        DB 055h
        DB 0AAh
        DB 0AAh
        PUBLIC _g_5b7a
_g_5b7a LABEL BYTE ; +5B7A char @[16]
        DB 00h
        DB 00h
        DB 055h
        DB 0BBh
        DB 0FFh
        DB 00h
        DB 00h
        DB 00h
        DB 055h
        DB 0DDh
        DB 0AAh
        DB 00h
        DB 0AAh
        DB 055h
        DB 0AAh
        DB 0AAh

        .CODE S1C11_TEXT

        PUBLIC  _F_1DD22
_F_1DD22  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,0Ah
        push    si
        push    di
        mov     al,[bp+14]
        call    _F_1E15E
        mov     [bp+14],al
        mov     si,2000h
        mov     di,0E050h
        mov     WORD PTR [bp-0Ah],0
        cmp     _g_0042,0
        je      L1DD22_gotdx
        cmp     _av787at,0B000h
        jb      L1DD22_gotdx
        cmp     _av787at,0B400h
        ja      L1DD22_gotdx
        mov     WORD PTR [bp-0Ah],2
        mov     ax,[bp+8]
        and     ax,1
        add     [bp-0Ah],ax
L1DD22_gotdx:
        mov     cx,[bp+10]
        sub     cx,[bp+6]
        je      L1DD22_vert
        jns     L1DD22_xordered
        neg     cx
        mov     bx,[bp+10]
        xchg    bx,[bp+6]
        mov     [bp+10],bx
        mov     bx,[bp+12]
        xchg    bx,[bp+8]
        mov     [bp+12],bx
        cmp     WORD PTR [bp-0Ah],0
        je      L1DD22_xordered
        mov     ax,[bp+8]
        and     ax,1
        or      ax,2
        mov     [bp-0Ah],ax
L1DD22_xordered:
        mov     bx,[bp+12]
        sub     bx,[bp+8]
        jne     L1DD22_notflat
        jmp     L1DD22_horiz
L1DD22_notflat:
        jns     L1DD22_ydown
        neg     bx
        neg     si
        neg     di
        xchg    si,di
L1DD22_ydown:
        mov     [bp-2],di
        mov     WORD PTR [bp-8],OFFSET L1DD22_xmajor
        cmp     bx,cx
        jle     L1DD22_minor
        mov     WORD PTR [bp-8],OFFSET L1DD22_ymajor
        xchg    bx,cx
L1DD22_minor:
        shl     bx,1
        mov     [bp-4],bx
        sub     bx,cx
        mov     di,bx
        sub     bx,cx
        mov     [bp-6],bx
        push    cx
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _F_1DFA9
        mov     al,[bp+14]
        shl     ax,cl
        mov     dx,ax
        not     dh
        pop     cx
        inc     cx
        test    bx,2000h
        je      L1DD22_enter
        xchg    si,[bp-2]
L1DD22_enter:
        jmp     WORD PTR [bp-8]
L1DD22_vert:
        mov     ax,[bp+8]
        mov     bx,[bp+12]
        mov     cx,bx
        sub     cx,ax
        jge     L1DD22_vlen
        neg     cx
        mov     ax,bx
L1DD22_vlen:
        inc     cx
        mov     bx,[bp+6]
        push    cx
        call    _F_1DFA9
        mov     al,[bp+14]
        shl     ax,cl
        not     ah
        pop     cx
        test    bx,si
        je      L1DD22_vloop
        xchg    si,di
L1DD22_vloop:
        and     es:[bx],ah
        or      es:[bx],al
        cmp     WORD PTR [bp-0Ah],0
        je      L1DD22_vnext
        test    WORD PTR [bp-0Ah],1
        je      L1DD22_vflip
        add     bx,2000h
        and     es:[bx],ah
        or      es:[bx],al
        sub     bx,2000h
L1DD22_vflip:
        xor     WORD PTR [bp-0Ah],1
L1DD22_vnext:
        add     bx,si
        xchg    si,di
        loop    L1DD22_vloop
        jmp     L1DD22_done
L1DD22_horiz:
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _F_1DFA9
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+10]
        and     cl,3
        xor     cl,3
        shl     cl,1
        shl     dl,cl
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        mov     cl,2
        shr     ax,cl
        shr     bx,cl
        mov     cx,ax
        sub     cx,bx
        mov     bx,OFFSET _g_5b56
        mov     al,[bp+14]
        xlat
        or      dh,dh
        js      L1DD22_hmiddle
        or      cx,cx
        jne     L1DD22_hlead
        and     dl,dh
        jmp     SHORT L1DD22_hlast
L1DD22_hlead:
        mov     ah,al
        and     ah,dh
        not     dh
        and     es:[di],dh
        or      es:[di],ah
        cmp     WORD PTR [bp-0Ah],0
        je      L1DD22_hlead1
        test    WORD PTR [bp-0Ah],1
        je      L1DD22_hlead1
        add     di,2000h
        and     es:[di],dh
        or      es:[di],ah
        sub     di,2000h
L1DD22_hlead1:
        inc     di
        dec     cx
L1DD22_hmiddle:
        cmp     WORD PTR [bp-0Ah],0
        je      L1DD22_hsingle
        test    WORD PTR [bp-0Ah],1
        je      L1DD22_hsingle
        push    cx
        push    di
        rep stosb
        pop     di
        pop     cx
        add     di,2000h
        rep stosb
        sub     di,2000h
        jmp     SHORT L1DD22_hlast
L1DD22_hsingle:
        rep stosb
L1DD22_hlast:
        and     al,dl
        not     dl
        and     es:[di],dl
        or      es:[di],al
        cmp     WORD PTR [bp-0Ah],0
        je      L1DD22_hend
        test    WORD PTR [bp-0Ah],1
        je      L1DD22_hend
        add     di,2000h
        and     es:[di],dl
        or      es:[di],al
        sub     di,2000h
L1DD22_hend:
        jmp     L1DD22_done
L1DD22_xmajor:
        mov     ah,es:[bx]
L1DD22_xand:
        and     ah,dh
        or      ah,dl
        ror     dl,1
        ror     dl,1
        ror     dh,1
        ror     dh,1
        jae     L1DD22_xnext
        or      di,di
        jns     L1DD22_xdiag
        add     di,[bp-4]
        loop    L1DD22_xand
        mov     es:[bx],ah
        jmp     L1DD22_done
L1DD22_xdiag:
        add     di,[bp-6]
        mov     es:[bx],ah
        cmp     WORD PTR [bp-0Ah],0
        je      L1DD22_xrow
        test    WORD PTR [bp-0Ah],1
        je      L1DD22_xflip
        add     bx,2000h
        mov     es:[bx],ah
        sub     bx,2000h
L1DD22_xflip:
        xor     WORD PTR [bp-0Ah],1
L1DD22_xrow:
        add     bx,si
        xchg    si,[bp-2]
        loop    L1DD22_xmajor
        jmp     SHORT L1DD22_done
L1DD22_xnext:
        mov     es:[bx],ah
        inc     bx
        or      di,di
        jns     L1DD22_xnext2
        add     di,[bp-4]
        dec     cx
        je      L1DD22_done
        jmp     L1DD22_xmajor
L1DD22_xnext2:
        add     di,[bp-6]
        add     bx,si
        xchg    si,[bp-2]
        dec     cx
        je      L1DD22_done
        jmp     L1DD22_xmajor
L1DD22_ymajor:
        and     es:[bx],dh
        or      es:[bx],dl
        cmp     WORD PTR [bp-0Ah],0
        je      L1DD22_yrow
        test    WORD PTR [bp-0Ah],1
        je      L1DD22_yflip
        add     bx,2000h
        and     es:[bx],dh
        or      es:[bx],dl
        sub     bx,2000h
L1DD22_yflip:
        xor     WORD PTR [bp-0Ah],1
L1DD22_yrow:
        add     bx,si
        xchg    si,[bp-2]
        or      di,di
        jns     L1DD22_ydiag
        add     di,[bp-4]
        loop    L1DD22_ymajor
        jmp     SHORT L1DD22_done
L1DD22_ydiag:
        add     di,[bp-6]
        ror     dl,1
        ror     dl,1
        ror     dh,1
        ror     dh,1
        cmc
        adc     bx,0
        loop    L1DD22_ymajor
L1DD22_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_1DD22  ENDP

; F_1DFA9 @ 1C11:1E99 (linear 0x1DFA9, 40 bytes)
; Pixel address helper for the interleaved 2-bit mode. In: ax = y, bx = x.
; Out: bx = byte offset of the pixel (scan line, bank and x/4 folded together) + at767ct, es = av787at, cl = 2 * (3 - (x & 3)).

        .DATA
        EXTRN   _at767ct:WORD
        EXTRN   _av787at:WORD

        .CODE S1C11_TEXT

        PUBLIC  _F_1DFA9
_F_1DFA9  PROC    NEAR
        mov     cl,bl
        xchg    ah,al
        shr     ax,1
        add     bh,al
        xor     al,al
        add     bx,ax
        shr     ax,1
        shr     ax,1
        add     bx,ax
        shr     bx,1
        shr     bx,1
        add     bx,_at767ct
        mov     ax,_av787at
        mov     es,ax
        mov     ah,3
        and     cl,ah
        xor     cl,ah
        shl     cl,1
        ret
_F_1DFA9  ENDP

; F_1DFD2 @ 1C11:1EC2 (linear 0x1DFD2, 300 bytes)
; Draw a horizontal line with 2 bits per pixel into the interleaved
; (CGA/Hercules style) screen, clipped to the L1DFD2_clip rectangle. On a Hercules
; segment (B000-B400 with g_0042 set) an odd line is also drawn 0x2000 bytes
; up, which is what the flag byte in the code segment records.
; F_1DFD2(x1, x2, y, colour)

        .DATA
        EXTRN   _g_5e42:BYTE
        EXTRN   _g_clip_ymin:WORD
        EXTRN   _g_clip_xmin:WORD
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_clip_xmax:WORD
        EXTRN   _g_0042:WORD
        EXTRN   _av787at:WORD

        .CODE S1C11_TEXT

dbl     DB      0

        PUBLIC  _F_1DFD2
_F_1DFD2  PROC    FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     BYTE PTR cs:dbl,0
        cmp     _g_0042,0
        je      L1DFD2_clip
        cmp     _av787at,0B000h
        jb      L1DFD2_clip
        cmp     _av787at,0B400h
        ja      L1DFD2_clip
        test    WORD PTR [bp+10],1
        je      L1DFD2_clip
        mov     BYTE PTR cs:dbl,1
L1DFD2_clip:
        mov     ax,[bp+10]
        cmp     _g_clip_ymin,ax
        jg      L1DFD2_skip1
        cmp     _g_clip_ymax,ax
        jl      L1DFD2_skip1
        mov     ax,[bp+6]
        mov     bx,[bp+8]
        mov     cx,_g_clip_xmin
        mov     dx,_g_clip_xmax
        cmp     cx,bx
        jg      L1DFD2_skip1
        cmp     dx,ax
        jl      L1DFD2_skip1
        jmp     SHORT L1DFD2_inside
L1DFD2_skip1:
        jmp     NEAR PTR L1DFD2_done
L1DFD2_inside:
        cmp     ax,cx
        jg      L1DFD2_xok
        mov     [bp+6],cx
L1DFD2_xok:
        cmp     bx,dx
        jl      L1DFD2_x2ok
        mov     [bp+8],dx
L1DFD2_x2ok:
        mov     ax,[bp+6]
        cmp     ax,[bp+8]
        jle     L1DFD2_ordered
        xchg    ax,[bp+8]
        mov     [bp+6],ax
L1DFD2_ordered:
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        call    _F_1DFA9
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+8]
        and     cl,3
        xor     cl,3
        shl     cl,1
        shl     dl,cl
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        mov     cl,2
        shr     ax,cl
        shr     bx,cl
        mov     cx,ax
        sub     cx,bx
        test    WORD PTR [bp+10],1
        je      L1DFD2_evenline
        mov     bx,OFFSET _g_5b6a
        jmp     SHORT L1DFD2_gotpat
L1DFD2_evenline:
        mov     bx,OFFSET _g_5b7a
L1DFD2_gotpat:
        mov     al,[bp+12]
        xlat
        or      dh,dh
        js      L1DFD2_middle
        or      cx,cx
        jne     L1DFD2_lead
        and     dl,dh
        jmp     SHORT L1DFD2_last
L1DFD2_lead:
        mov     ah,al
        and     ah,dh
        not     dh
        and     es:[di],dh
        or      es:[di],ah
        cmp     BYTE PTR cs:dbl,0
        je      L1DFD2_lead1
        add     di,2000h
        and     es:[di],dh
        or      es:[di],ah
        sub     di,2000h
L1DFD2_lead1:
        inc     di
        dec     cx
L1DFD2_middle:
        cmp     BYTE PTR cs:dbl,0
        je      L1DFD2_single
        push    cx
        push    di
        rep stosb
        pop     di
        pop     cx
        add     di,2000h
        rep stosb
        sub     di,2000h
        jmp     SHORT L1DFD2_last
L1DFD2_single:
        rep stosb
L1DFD2_last:
        and     al,dl
        not     dl
        and     es:[di],dl
        or      es:[di],al
        cmp     BYTE PTR cs:dbl,0
        je      L1DFD2_done
        add     di,2000h
        and     es:[di],dl
        or      es:[di],al
        sub     di,2000h
L1DFD2_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_1DFD2  ENDP

; F_1E0FE @ 1C11:1FEE (linear 0x1E0FE, 96 bytes)
; Fill whole screen lines with a 2-bit pattern, from line y1 down to the
; bottom clip line (g_clip_ymax). The colour is turned into a pattern
; byte via g_5b6a; every second line gets the pattern rotated by 2 bits.
; F_1E0FE(y1, colour)

        .DATA

        .CODE S1C11_TEXT

        PUBLIC  _F_1E0FE
_F_1E0FE  PROC    FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+6]
        cmp     _g_clip_ymax,ax
        jl      L1E0FE_done
        mov     bx,OFFSET _g_5b6a
        mov     ax,[bp+8]
        xor     ah,ah
        xlat
        mov     dh,al
        mov     dl,al
        xor     si,si
L1E0FE_again:
        mov     ax,[bp+6]
        mov     bx,_g_clip_ymax
        mov     cx,bx
        sub     cx,ax
        shr     cx,1
        xor     ax,bx
        test    ax,1
        je      L1E0FE_even
        inc     cx
L1E0FE_even:
        or      cx,cx
        je      L1E0FE_next
        mov     ax,[bp+6]
        xor     bx,bx
        push    cx
        call    _F_1DFA9
        pop     ax
        mov     cl,28h
        mul     cl
        mov     cx,ax
        mov     ax,dx
        mov     di,bx
        rep stosw
L1E0FE_next:
        or      si,si
        jne     L1E0FE_done
        inc     si
        inc     WORD PTR [bp+6]
        ror     dx,1
        ror     dx,1
        jmp     L1E0FE_again
L1E0FE_done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_1E0FE  ENDP

; F_1E15E @ 1C11:204E (linear 0x1E15E, 16 bytes)
; Map a colour in al to a CGA 2-bit pixel value: 0-3 stay as they are,
; 4-15 go through the table g_5b5a, anything above becomes 3.

        .DATA

        .CODE S1C11_TEXT

        PUBLIC  _F_1E15E
_F_1E15E  PROC    NEAR
        cmp     al,3
        jbe     L1E15E_done
        cmp     al,0Fh
        jbe     L1E15E_mapped
        mov     al,3
L1E15E_done:
        ret
L1E15E_mapped:
        mov     bx,OFFSET _g_5b5a
        xlat
        ret
_F_1E15E  ENDP

        END
