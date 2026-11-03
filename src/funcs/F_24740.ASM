; F_24740 @ 2462:0120 (linear 0x24740, 139 bytes)
; F_24740(src_far, dst_far, count, palette): expand a compressed 4-bit image.
; Each pattern byte gives 8 bits; every set bit makes one 4-bit pixel 0Fh in
; the two output words.  Code 0 skips pixels, 0FFh fills with 0FFFFh.

        .MODEL  MEDIUM
        .CODE
        EXTRN   _F_2462E:NEAR
        EXTRN   _F_24814:NEAR

        PUBLIC  _F_24740
_F_24740  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,122h
        push    di
        push    si
        push    ds
        call    _F_2462E
        les     di,DWORD PTR [bp+0ah]
        lds     si,DWORD PTR [bp+6]
        mov     dx,di
        add     dx,[bp+0eh]
        jnc     next
        jmp     SHORT done
next:
        call    _F_24814
        or      al,al
        je      skip
        cmp     al,0FFh
        je      fill
        xor     bx,bx
        rol     al,1
        jnc     w1
        or      bl,0F0h
w1:
        rol     al,1
        jnc     w2
        or      bl,0Fh
w2:
        rol     al,1
        jnc     w3
        or      bh,0F0h
w3:
        rol     al,1
        jnc     w4
        or      bh,0Fh
w4:
        mov     WORD PTR es:[di],bx
        inc     di
        inc     di
        xor     bx,bx
        rol     al,1
        jnc     w5
        or      bl,0F0h
w5:
        rol     al,1
        jnc     w6
        or      bl,0Fh
w6:
        rol     al,1
        jnc     w7
        or      bh,0F0h
w7:
        rol     al,1
        jnc     w8
        or      bh,0Fh
w8:
        mov     WORD PTR es:[di],bx
        inc     di
        inc     di
        jmp     SHORT check
skip:
        shl     cx,1
        shl     cx,1
        add     di,cx
        jmp     SHORT check
fill:
        shl     cx,1
        mov     ax,0FFFFh
        rep     stosw
check:
        cmp     di,dx
        jae     done
        jmp     next
done:
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_24740  ENDP

        END
