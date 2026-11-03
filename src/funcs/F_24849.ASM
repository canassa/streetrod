; F_24849 @ 2462:0229 (linear 0x24849, 48 bytes)
; F_24849(src_far, dst_far, count): turn count bytes of packed 4-bit pixels
; into a byte-per-pair mask: a nibble that is zero sets the corresponding
; nibble of the mask (0F0h for the high one, 0Fh for the low one).

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_24849
_F_24849  PROC  FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        push    ds
        les     di,DWORD PTR [bp+0ah]
        mov     cx,[bp+0eh]
        lds     si,DWORD PTR [bp+6]
        mov     bh,0F0h
        mov     bl,0Fh
next:
        xor     ah,ah
        lodsb
        xchg    ah,al
        test    ah,0F0h
        jne     low_nibble
        mov     al,bh
low_nibble:
        test    ah,0Fh
        jne     store
        or      al,bl
store:
        stosb
        loop    next
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_24849  ENDP

        END
