        .MODEL MEDIUM

; sr-member 0x24EFB
; F_24EFB @ 24E7:008B (linear 0x24EFB, 79 bytes)
; F_24EFB(src_far, width, height, dst_far, src_pitch): EGA latch copy (write
; mode 1) of a width x height byte rectangle into a 40 byte per row screen.
; Does nothing if width or height < 1.

        .CODE

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
