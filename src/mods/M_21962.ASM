        .MODEL MEDIUM

; sr-member 0x21962
; F_21962 @ 2196:0002 (linear 0x21962, 27 bytes)
; Busy-wait delay: L21962_spin for n * g_58e2 units, in 16-bit chunks of a LOOP count
; (the high byte of the product counts the outer repetitions).
; F_21962(n)

        .DATA
        EXTRN   _g_58e2:WORD
        ; data (recovered DGROUP layout)
        EVEN
        PUBLIC _g_58de
_g_58de LABEL BYTE ; +58DE int @
        DW 01h
        EVEN
        PUBLIC _g_58e0
_g_58e0 LABEL BYTE ; +58E0 int @
        DW 01h








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
L21962_again:
        mov     cx,dx
L21962_spin:
        loop    L21962_spin
        dec     bx
        jns     L21962_again
        pop     bp
        ret
_F_21962  ENDP

; sr-member 0x2197D
; F_2197D @ 1C11:586D (linear 0x2197D, 136 bytes)
; Play a tune on the PC speaker. The argument points at a table of
; 3-word records (length, two frequency counters, duty/step word) ending
; with a zero length word; g_58de keeps the read pointer so the caller's
; table advances one record per step. The speaker is toggled through port 61h,
; g_58e0 is the delay count of the half-wave loop.
; F_2197D(table)

        .DATA

        .CODE

        PUBLIC  _F_2197D
_F_2197D  PROC    FAR
        push    bp
        mov     bp,sp
        mov     bx,[bp+6]
        mov     _g_58de,bx
L2197D_next:
        mov     bx,_g_58de
        mov     ax,[bx]
        cmp     ax,0
        je      L2197D_done
        mov     si,ax
        inc     bx
        inc     bx
        mov     dx,[bx]
        inc     bx
        inc     bx
        mov     dh,dl
        mov     dl,2
        mov     cx,[bx]
        inc     bx
        inc     bx
        mov     _g_58de,bx
        mov     bh,cl
        mov     cl,1
        xor     bl,bl
        mov     bp,2FFh
        cmp     bh,0
        jne     L2197D_tone
        xor     bp,200h
        mov     bh,64h
L2197D_tone:
        xor     ax,ax
        mov     di,ax
        sub     dl,1
        rcl     ax,1
        neg     ax
        and     al,dh
        add     dl,al
        mov     di,ax
        xor     ax,ax
        sub     cl,1
        rcl     ax,1
        neg     ax
        and     al,ch
        add     cl,al
        or      di,ax
        xor     ax,ax
        sub     bl,1
        rcl     ax,1
        sub     si,ax
        je      L2197D_next
        neg     ax
        and     al,bh
        add     bl,al
        or      ax,di
        mov     al,4Ch
        out     61h,al
        and     ax,bp
        or      al,ah
        out     61h,al
        mov     ax,cx
        mov     cx,_g_58e0
L2197D_spin:
        loop    L2197D_spin
        mov     cx,ax
        jmp     L2197D_tone
L2197D_done:
        pop     bp
        ret
_F_2197D  ENDP
        END
