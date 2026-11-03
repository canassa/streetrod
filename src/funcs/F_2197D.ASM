; F_2197D @ 1C11:586D (linear 0x2197D, 136 bytes)
; Play a tune on the PC speaker. The argument points at a table of
; 3-word records (length, two frequency counters, duty/step word) ending
; with a zero length word; g_58de keeps the read pointer so the caller's
; table advances one record per step. The speaker is toggled through port 61h,
; g_58e0 is the delay count of the half-wave loop.
; F_2197D(table)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_58de:WORD
        EXTRN   _g_58e0:WORD

        .CODE

        PUBLIC  _F_2197D
_F_2197D  PROC    FAR
        push    bp
        mov     bp,sp
        mov     bx,[bp+6]
        mov     _g_58de,bx
next:
        mov     bx,_g_58de
        mov     ax,[bx]
        cmp     ax,0
        je      done
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
        jne     tone
        xor     bp,200h
        mov     bh,64h
tone:
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
        je      next
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
spin:
        loop    spin
        mov     cx,ax
        jmp     tone
done:
        pop     bp
        ret
_F_2197D  ENDP

        END
