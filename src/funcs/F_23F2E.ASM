; F_23F2E @ 21A0:252E (linear 0x23F2E, 31 bytes)
; Planar pixel address helper. In: ax = y, bx = x.
; Out: bx = y*40 + x/8, es = video segment, cl = 7 - (x & 7), ah = 1.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_787a:WORD

        .CODE

        PUBLIC  _F_23F2E
_F_23F2E  PROC    NEAR
        mov     cl,bl
        push    dx
        mov     dx,40
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
_F_23F2E  ENDP

        END
