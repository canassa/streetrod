        .MODEL MEDIUM

        .CODE S24D8_TEXT


; ty_to_screen(src_far, dst_far, bytes, rows, src_skip, dst_wrap, y): copy rows of
; bytes into the interleaved screen.  Four banks of 2000h; after every fourth
; row the destination steps back by 6000h and forward by dst_wrap.

        PUBLIC  _ty_to_screen
_ty_to_screen  PROC    FAR
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
row1:
        mov     cx,[bp+0Eh]
        shr     cx,1
        jnc     words1
        movsb
words1:
        rep     movsw
        dec     ax
        je      done1
        add     si,[bp+12h]
        inc     bx
        test    bx,3
        je      wrap
        add     di,dx
        jmp     SHORT row1
wrap:
        add     di,[bp+14h]
        sub     di,6000h
        jmp     SHORT row1
done1:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_ty_to_screen  ENDP


; ty_screen(src_far, dst_far, bytes, rows, sx, sy, dx, dy): like ty_to_screen but
; both ends are positions in the interleaved screen: offset = (y / 4) * 160 +
; x, plus 2000h per (y & 1) and 4000h per (y & 2).

        PUBLIC  _ty_screen
_ty_screen  PROC    FAR
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
row2:
        mov     cx,[bp+0Eh]
        shr     cx,1
        jnc     words2
        movsb
words2:
        rep     movsw
        dec     ax
        je      done2
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
        jmp     SHORT row2
dwrap:
        add     di,[bp-4]
        jmp     SHORT row2
done2:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_ty_screen  ENDP

        END
