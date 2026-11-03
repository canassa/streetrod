; F_26401 @ 2634:00C1 (linear 0x26401, 86 bytes)
; F_26401(src_far, dst_far, bytes): OR together the four bit planes of a
; source of bytes (a quarter of it each, plane n at src + n * bytes/4) and
; store the complement, byte by byte for an odd count and then word by word.

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_26401
_F_26401  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        lds     si,DWORD PTR [bp+6]
        les     di,DWORD PTR [bp+0Ah]
        mov     cx,[bp+0Eh]
        shr     cx,1
        shr     cx,1
        mov     dx,cx
        mov     ax,cx
        shr     ax,1
        jnc     words
        xor     al,al
        or      al,[si]
        mov     bx,dx
        or      al,[bx+si]
        add     bx,dx
        or      al,[bx+si]
        add     bx,dx
        or      al,[bx+si]
        not     al
        mov     es:[di],al
        dec     cx
        je      done
        inc     di
        inc     si
words:
        xor     ax,ax
        or      ax,[si]
        mov     bx,dx
        or      ax,[bx+si]
        add     bx,dx
        or      ax,[bx+si]
        add     bx,dx
        or      ax,[bx+si]
        not     ax
        mov     es:[di],ax
        inc     di
        inc     di
        inc     si
        inc     si
        dec     cx
        loop    words
done:
        pop     si
        pop     di
        pop     ds
        pop     bp
        ret
_F_26401  ENDP

        END
