; F_1C11C @ 1c11:000c (linear 0x1C11C, 48 bytes)
; Copy a rectangle of bytes between far buffers:
; blit_rect(src far, dst far, width, rows, src_skip, dst_skip)

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _blit_rect
_blit_rect  PROC  FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        cld
        lds     si,DWORD PTR [bp+6]     ; src
        les     di,DWORD PTR [bp+10]    ; dst
        mov     ax,[bp+16]              ; rows
        mov     dx,[bp+20]              ; dst_skip
        mov     bx,[bp+18]              ; src_skip
rowloop:
        mov     cx,[bp+14]              ; width in bytes
        shr     cx,1
        jnc     no_odd
        movsb
no_odd:
        rep movsw
        dec     ax
        je      done
        add     si,bx
        add     di,dx
        jmp     rowloop
done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_blit_rect  ENDP

        END
