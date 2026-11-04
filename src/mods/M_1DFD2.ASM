        .MODEL MEDIUM

; sr-member 0x1DFD2
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
        EXTRN   _g_5b6a:BYTE
        EXTRN   _g_5b7a:BYTE

        .CODE

        EXTRN   _F_1DFA9:NEAR

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

; sr-member 0x1E0FE
; F_1E0FE @ 1C11:1FEE (linear 0x1E0FE, 96 bytes)
; Fill whole screen lines with a 2-bit pattern, from line y1 down to the
; bottom clip line (g_clip_ymax). The colour is turned into a pattern
; byte via g_5b6a; every second line gets the pattern rotated by 2 bits.
; F_1E0FE(y1, colour)

        .DATA
        EXTRN   _g_5b6a:BYTE

        .CODE

        EXTRN   _F_1DFA9:NEAR

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

; sr-member 0x1E15E
; F_1E15E @ 1C11:204E (linear 0x1E15E, 16 bytes)
; Map a colour in al to a CGA 2-bit pixel value: 0-3 stay as they are,
; 4-15 go through the table g_5b5a, anything above becomes 3.

        .DATA
        EXTRN   _g_5b5a:BYTE

        .CODE

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
