; F_21962 @ 2196:0002 (linear 0x21962, 27 bytes)
; Busy-wait delay: spin for n * g_58e2 units, in 16-bit chunks of a LOOP count
; (the high byte of the product counts the outer repetitions).
; F_21962(n)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_58e2:WORD

        .CODE

        PUBLIC  _F_21962
_F_21962  PROC    FAR
        push    bp
        mov     bp,sp
        mov     ax,[bp+6]
        mul     _g_58e2
        xor     bh,bh
        mov     bl,dh
        mov     dh,dl
        mov     dl,ah
again:
        mov     cx,dx
spin:
        loop    spin
        dec     bx
        jns     again
        pop     bp
        ret
_F_21962  ENDP

        END
