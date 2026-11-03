; F_24DC7 @ 24D8:0047 (linear 0x24DC7, 177 bytes)
; F_24DC7(src_far, dst_far, bytes, rows, sx, sy, dx, dy): like F_24D84 but
; both ends are positions in the interleaved screen: offset = (y / 4) * 160 +
; x, plus 2000h per (y & 1) and 4000h per (y & 2).

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_24DC7
_F_24DC7  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,4
        push    ds
        push    di
        push    si
        lds     si,DWORD PTR [bp+6]
        les     di,DWORD PTR [bp+0Ah]
        mov     ax,0A0h
        mov     bx,[bp+18h]
        mov     bh,bl
        shr     bl,1
        shr     bl,1
        mul     bl
        add     di,ax
        add     di,[bp+16h]
        mov     ax,0A0h
        mov     dx,[bp+14h]
        mov     dh,dl
        shr     dl,1
        shr     dl,1
        mul     dl
        add     si,ax
        add     si,[bp+12h]
        mov     dl,dh
        xor     dh,dh
        mov     bl,bh
        xor     bh,bh
        test    dx,1
        je      s1
        add     si,2000h
s1:
        test    dx,2
        je      s2
        add     si,4000h
s2:
        test    bx,1
        je      d1
        add     di,2000h
d1:
        test    bx,2
        je      d2
        add     di,4000h
d2:
        mov     ax,2000h
        sub     ax,[bp+0Eh]
        mov     [bp-2],ax
        mov     ax,0A0h
        sub     ax,6000h
        sub     ax,[bp+0Eh]
        mov     [bp-4],ax
        mov     ax,[bp+10h]
row:
        mov     cx,[bp+0Eh]
        shr     cx,1
        jnc     words
        movsb
words:
        rep     movsw
        dec     ax
        je      done
        inc     dx
        test    dx,3
        je      swrap
        add     si,[bp-2]
        jmp     SHORT sdone
swrap:
        add     si,[bp-4]
sdone:
        inc     bx
        test    bx,3
        je      dwrap
        add     di,[bp-2]
        jmp     SHORT row
dwrap:
        add     di,[bp-4]
        jmp     SHORT row
done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_24DC7  ENDP

        END
