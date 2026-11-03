; F_24E78 @ 24E7:0008 (linear 0x24E78, 131 bytes)
; F_24E78(src_far, dst_far, width, height, x, y, pitch): EGA latch copy
; (write mode 1) of a width x height byte rectangle between two planar
; buffers with the same pitch.  Does nothing if width or height < 1.

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_24E78
_F_24E78  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    si
        push    di
        cmp     WORD PTR [bp+0Eh],1
        jl      done
        cmp     WORD PTR [bp+10h],1
        jl      done
        cld
        mov     dx,3CEh
        mov     al,5
        out     dx,al
        inc     dx
        mov     al,1
        out     dx,al
        lds     si,DWORD PTR [bp+6]
        les     di,DWORD PTR [bp+0Ah]
        mov     dx,[bp+16h]
        mov     ax,[bp+14h]
        mul     dl
        add     ax,[bp+12h]
        add     si,ax
        add     di,ax
        mov     ax,[bp+0Eh]
        cmp     ax,1
        je      column
        cmp     ax,[bp+16h]
        je      whole
        mov     bx,[bp+10h]
        mov     dx,[bp+16h]
        sub     dx,[bp+0Eh]
rows:
        mov     cx,[bp+0Eh]
        rep     movsb
        add     si,dx
        add     di,dx
        dec     bx
        jne     rows
        jmp     SHORT reset
column:
        mov     cx,[bp+10h]
        mov     dx,[bp+16h]
        sub     dx,1
colloop:
        movsb
        add     si,dx
        add     di,dx
        loop    colloop
        jmp     SHORT reset
whole:
        mov     dx,[bp+16h]
        mov     ax,[bp+10h]
        mul     dl
        mov     cx,ax
        rep     movsb
reset:
        mov     dx,3CEh
        mov     al,5
        out     dx,al
        inc     dx
        mov     al,0
        out     dx,al
done:
        pop     di
        pop     si
        pop     ds
        pop     bp
        ret
_F_24E78  ENDP

        END
