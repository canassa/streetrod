        .MODEL MEDIUM

; sr-member 0x24D84
; F_24D84 @ 24D8:0004 (linear 0x24D84, 67 bytes)
; F_24D84(src_far, dst_far, bytes, rows, src_skip, dst_wrap, y): copy rows of
; bytes into the interleaved screen.  Four banks of 2000h; after every fourth
; L24D84_row the destination steps back by 6000h and forward by dst_wrap.

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
L24D84_row:
        mov     cx,[bp+0Eh]
        shr     cx,1
        jnc     L24D84_words
        movsb
L24D84_words:
        rep     movsw
        dec     ax
        je      L24D84_done
        add     si,[bp+12h]
        inc     bx
        test    bx,3
        je      L24D84_wrap
        add     di,dx
        jmp     SHORT L24D84_row
L24D84_wrap:
        add     di,[bp+14h]
        sub     di,6000h
        jmp     SHORT L24D84_row
L24D84_done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_24D84  ENDP

; sr-member 0x24DC7
; F_24DC7 @ 24D8:0047 (linear 0x24DC7, 177 bytes)
; F_24DC7(src_far, dst_far, bytes, rows, sx, sy, dx, dy): like F_24D84 but
; both ends are positions in the interleaved screen: offset = (y / 4) * 160 +
; x, plus 2000h per (y & 1) and 4000h per (y & 2).

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
        je      L24DC7_s1
        add     si,2000h
L24DC7_s1:
        test    dx,2
        je      L24DC7_s2
        add     si,4000h
L24DC7_s2:
        test    bx,1
        je      L24DC7_d1
        add     di,2000h
L24DC7_d1:
        test    bx,2
        je      L24DC7_d2
        add     di,4000h
L24DC7_d2:
        mov     ax,2000h
        sub     ax,[bp+0Eh]
        mov     [bp-2],ax
        mov     ax,0A0h
        sub     ax,6000h
        sub     ax,[bp+0Eh]
        mov     [bp-4],ax
        mov     ax,[bp+10h]
L24DC7_row:
        mov     cx,[bp+0Eh]
        shr     cx,1
        jnc     L24DC7_words
        movsb
L24DC7_words:
        rep     movsw
        dec     ax
        je      L24DC7_done
        inc     dx
        test    dx,3
        je      L24DC7_swrap
        add     si,[bp-2]
        jmp     SHORT L24DC7_sdone
L24DC7_swrap:
        add     si,[bp-4]
L24DC7_sdone:
        inc     bx
        test    bx,3
        je      L24DC7_dwrap
        add     di,[bp-2]
        jmp     SHORT L24DC7_row
L24DC7_dwrap:
        add     di,[bp-4]
        jmp     SHORT L24DC7_row
L24DC7_done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_24DC7  ENDP
        END
