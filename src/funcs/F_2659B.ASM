; F_2659B @ 2645:014B (linear 0x2659B, 31 bytes)
; F_2659B(ax = y, bx = x): EGA address of a pixel.  Out: bx = y * 40 + x / 8,
; es = g_787a, cl = 7 - (x & 7), ah = 1.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_787a:WORD

        .CODE

        PUBLIC  _F_2659B
_F_2659B  PROC    NEAR
        mov     cl,bl
        push    dx
        mov     dx,28h
        mul     dx
        pop     dx
        shr     bx,1
        shr     bx,1
        shr     bx,1
        add     bx,ax
        mov     ax,_g_787a
        mov     es,ax
        and     cl,7
        xor     cl,7
        mov     ah,1
        ret
_F_2659B  ENDP

        END
