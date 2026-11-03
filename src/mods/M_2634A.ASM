        .MODEL MEDIUM

; sr-member 0x2634A
; F_2634A @ 2634:000A (linear 0x2634A, 183 bytes)
; F_2634A(dst_far, src_seg, src_off, mask_off, dst_pitch, x_bytes, w, h):
; masked blit.  For each L2634A_row: dst = (dst & mask) | (~mask & src), words first
; and then one odd byte.  Its scratch variables live in the code segment, in
; the 6 bytes it jumps over, so they are addressed through CS at fixed offsets
; (the routine sits at offset 0Ah of its code segment).

        .CODE

        ORG     0Ah

        PUBLIC  _F_2634A
_F_2634A  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        mov     cx,[bp+14h]
        cmp     cx,0
        jg      L2634A_havew
        jmp     L2634A_done
L2634A_havew:
        mov     ax,[bp+12h]
        cmp     ax,0
        jg      L2634A_haveh
        jmp     L2634A_done
savebp  DW      0
skip    DW      0
words   DB      0
odd     DB      0
L2634A_haveh:
        mov     ch,al
        mov     al,ch
        and     al,1
        mov     cs:odd,al
        and     ch,0FEh
        mov     dx,[bp+10h]
        mov     al,ch
        xor     ah,ah
        sub     dx,ax
        mov     cs:skip,dx
        mov     dx,[bp+16h]
        sub     dl,ch
        shr     ch,1
        mov     cs:words,ch
        mov     cs:savebp,bp
        mov     ds,[bp+0Ah]
        mov     si,[bp+0Ch]
        les     di,DWORD PTR [bp+6]
        mov     ax,[bp+0Eh]
        mov     bp,ax
L2634A_row:
        mov     ch,cs:words
        cmp     ch,0
        je      L2634A_lastbyte
L2634A_col:
        mov     ax,ds:[bp]
        mov     bx,es:[di]
        and     bx,ax
        not     ax
        and     ax,[si]
        or      ax,bx
        mov     es:[di],ax
        add     bp,2
        add     si,2
        add     di,2
        dec     ch
        jne     L2634A_col
        cmp     cs:odd,0
        je      L2634A_rowend
L2634A_lastbyte:
        mov     al,ds:[bp]
        mov     bl,es:[di]
        and     bl,al
        not     al
        and     al,[si]
        or      al,bl
        mov     es:[di],al
L2634A_rowend:
        mov     ax,cs:skip
        add     bp,ax
        add     si,ax
        add     di,dx
        dec     cl
        jne     L2634A_row
        mov     bp,cs:savebp
L2634A_done:
        pop     si
        pop     di
        pop     ds
        pop     bp
        ret
_F_2634A  ENDP

; sr-member 0x26401
; F_26401 @ 2634:00C1 (linear 0x26401, 86 bytes)
; F_26401(src_far, dst_far, bytes): OR together the four bit planes of a
; source of bytes (a quarter of it each, plane n at src + n * bytes/4) and
; store the complement, byte by byte for an odd count and then word by word.

        .CODE

        PUBLIC  _F_26401
_F_26401  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        lds     si,DWORD PTR [bp+6]
        les     di,DWORD PTR [bp+0Ah]
        mov     cx,[bp+0Eh]
        shr     cx,1
        shr     cx,1
        mov     dx,cx
        mov     ax,cx
        shr     ax,1
        jnc     L26401_words
        xor     al,al
        or      al,[si]
        mov     bx,dx
        or      al,[bx+si]
        add     bx,dx
        or      al,[bx+si]
        add     bx,dx
        or      al,[bx+si]
        not     al
        mov     es:[di],al
        dec     cx
        je      L26401_done
        inc     di
        inc     si
L26401_words:
        xor     ax,ax
        or      ax,[si]
        mov     bx,dx
        or      ax,[bx+si]
        add     bx,dx
        or      ax,[bx+si]
        add     bx,dx
        or      ax,[bx+si]
        not     ax
        mov     es:[di],ax
        inc     di
        inc     di
        inc     si
        inc     si
        dec     cx
        loop    L26401_words
L26401_done:
        pop     si
        pop     di
        pop     ds
        pop     bp
        ret
_F_26401  ENDP
        END
