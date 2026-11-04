        .MODEL MEDIUM

; F_24E78 @ 24E7:0008 (linear 0x24E78, 131 bytes)
; F_24E78(src_far, dst_far, width, height, x, y, pitch): EGA latch copy
; (write mode 1) of a width x height byte rectangle between two planar
; buffers with the same pitch.  Does nothing if width or height < 1.

        .CODE S24E7_TEXT

        PUBLIC  _F_24E78
_F_24E78  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    si
        push    di
        cmp     WORD PTR [bp+0Eh],1
        jl      L24E78_done
        cmp     WORD PTR [bp+10h],1
        jl      L24E78_done
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
        je      L24E78_column
        cmp     ax,[bp+16h]
        je      L24E78_whole
        mov     bx,[bp+10h]
        mov     dx,[bp+16h]
        sub     dx,[bp+0Eh]
L24E78_rows:
        mov     cx,[bp+0Eh]
        rep     movsb
        add     si,dx
        add     di,dx
        dec     bx
        jne     L24E78_rows
        jmp     SHORT L24E78_reset
L24E78_column:
        mov     cx,[bp+10h]
        mov     dx,[bp+16h]
        sub     dx,1
L24E78_colloop:
        movsb
        add     si,dx
        add     di,dx
        loop    L24E78_colloop
        jmp     SHORT L24E78_reset
L24E78_whole:
        mov     dx,[bp+16h]
        mov     ax,[bp+10h]
        mul     dl
        mov     cx,ax
        rep     movsb
L24E78_reset:
        mov     dx,3CEh
        mov     al,5
        out     dx,al
        inc     dx
        mov     al,0
        out     dx,al
L24E78_done:
        pop     di
        pop     si
        pop     ds
        pop     bp
        ret
_F_24E78  ENDP

; F_24EFB @ 24E7:008B (linear 0x24EFB, 79 bytes)
; F_24EFB(src_far, width, height, dst_far, src_pitch): EGA latch copy (write
; mode 1) of a width x height byte rectangle into a 40 byte per row screen.
; Does nothing if width or height < 1.

        .CODE S24E7_TEXT

        PUBLIC  _F_24EFB
_F_24EFB  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    si
        push    di
        cmp     WORD PTR [bp+0Ch],1
        jl      L24EFB_done
        cmp     WORD PTR [bp+0Ah],1
        jl      L24EFB_done
        cld
        mov     dx,3CEh
        mov     al,5
        out     dx,al
        inc     dx
        mov     al,1
        out     dx,al
        lds     si,DWORD PTR [bp+6]
        les     di,DWORD PTR [bp+0Eh]
        mov     bl,[bp+0Ch]
        mov     ax,[bp+0Ah]
        mov     dx,[bp+12h]
        sub     dx,ax
        mov     bh,28h
        sub     bh,al
L24EFB_rows:
        mov     cx,ax
        rep     movsb
        add     si,dx
        mov     cl,bh
        add     di,cx
        dec     bl
        jne     L24EFB_rows
        mov     dx,3CEh
        mov     al,5
        out     dx,al
        inc     dx
        mov     al,0
        out     dx,al
L24EFB_done:
        pop     di
        pop     si
        pop     ds
        pop     bp
        ret
_F_24EFB  ENDP

        END
