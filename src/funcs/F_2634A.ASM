; F_2634A @ 2634:000A (linear 0x2634A, 183 bytes)
; F_2634A(dst_far, src_seg, src_off, mask_off, dst_pitch, x_bytes, w, h):
; masked blit.  For each row: dst = (dst & mask) | (~mask & src), words first
; and then one odd byte.  Its scratch variables live in the code segment, in
; the 6 bytes it jumps over, so they are addressed through CS at fixed offsets
; (the routine sits at offset 0Ah of its code segment).

        .MODEL  MEDIUM
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
        jg      havew
        jmp     done
havew:
        mov     ax,[bp+12h]
        cmp     ax,0
        jg      haveh
        jmp     done
savebp  DW      0
skip    DW      0
words   DB      0
odd     DB      0
haveh:
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
row:
        mov     ch,cs:words
        cmp     ch,0
        je      lastbyte
col:
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
        jne     col
        cmp     cs:odd,0
        je      rowend
lastbyte:
        mov     al,ds:[bp]
        mov     bl,es:[di]
        and     bl,al
        not     al
        and     al,[si]
        or      al,bl
        mov     es:[di],al
rowend:
        mov     ax,cs:skip
        add     bp,ax
        add     si,ax
        add     di,dx
        dec     cl
        jne     row
        mov     bp,cs:savebp
done:
        pop     si
        pop     di
        pop     ds
        pop     bp
        ret
_F_2634A  ENDP

        END
