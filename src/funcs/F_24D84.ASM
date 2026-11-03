; F_24D84 @ 24D8:0004 (linear 0x24D84, 67 bytes)
; F_24D84(src_far, dst_far, bytes, rows, src_skip, dst_wrap, y): copy rows of
; bytes into the interleaved screen.  Four banks of 2000h; after every fourth
; row the destination steps back by 6000h and forward by dst_wrap.

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_24D84
_F_24D84  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        lds     si,DWORD PTR [bp+6]
        les     di,DWORD PTR [bp+0Ah]
        mov     dx,2000h
        sub     dx,[bp+0Eh]
        mov     ax,[bp+10h]
        mov     bx,[bp+16h]
row:
        mov     cx,[bp+0Eh]
        shr     cx,1
        jnc     words
        movsb
words:
        rep     movsw
        dec     ax
        je      done
        add     si,[bp+12h]
        inc     bx
        test    bx,3
        je      wrap
        add     di,dx
        jmp     SHORT row
wrap:
        add     di,[bp+14h]
        sub     di,6000h
        jmp     SHORT row
done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_24D84  ENDP

        END
