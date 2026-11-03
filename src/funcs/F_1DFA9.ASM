; F_1DFA9 @ 1C11:1E99 (linear 0x1DFA9, 40 bytes)
; Pixel address helper for the interleaved 2-bit mode. In: ax = y, bx = x.
; Out: bx = byte offset of the pixel (scan line, bank and x/4 folded together) + g_767c, es = g_787a, cl = 2 * (3 - (x & 3)).

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_767c:WORD
        EXTRN   _g_787a:WORD

        .CODE

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
        add     bx,_g_767c
        mov     ax,_g_787a
        mov     es,ax
        mov     ah,3
        and     cl,ah
        xor     cl,ah
        shl     cl,1
        ret
_F_1DFA9  ENDP

        END
