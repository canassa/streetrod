; F_24CAB @ 24BE:00CB (linear 0x24CAB, 216 bytes)
; F_24CAB(src, dstseg, dstoff, y, height, width, flags, ...): copy a strip of
; 4-bit pixels from a linear buffer (160 bytes per row) into the interleaved
; (4 banks of 2000h) screen.  Handles a leading half byte (merge the low
; nibble), the whole bytes, and a trailing half byte (merge the high nibble).

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_24CAB
_F_24CAB  PROC    FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+8]
        mov     es,ax
        mov     ax,[bp+12h]
        or      ax,ax
        je      middle
        mov     cx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
lead:
        and     BYTE PTR es:[di],0F0h
        mov     al,[si]
        or      es:[di],al
        add     si,0A0h
        inc     dx
        test    dx,3
        je      lead_wrap
        add     di,2000h
        jmp     SHORT lead_next
lead_wrap:
        sub     di,6000h
        add     di,0A0h
lead_next:
        loop    lead
        inc     WORD PTR [bp+0Ah]
        inc     WORD PTR [bp+6]
middle:
        cmp     WORD PTR [bp+10h],0
        jle     trail
        mov     bx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
row:
        mov     cx,[bp+10h]
        shr     cx,1
        jnc     words
        movsb
        jcxz    rowend
words:
        rep     movsw
rowend:
        add     si,0A0h
        mov     ax,[bp+10h]
        sub     si,ax
        sub     di,ax
        inc     dx
        test    dx,3
        je      row_wrap
        add     di,2000h
        jmp     SHORT row_next
row_wrap:
        sub     di,6000h
        add     di,0A0h
row_next:
        dec     bx
        jne     row
        mov     ax,[bp+0Ah]
        add     ax,[bp+10h]
        mov     [bp+0Ah],ax
        mov     ax,[bp+6]
        add     ax,[bp+10h]
        mov     [bp+6],ax
trail:
        mov     ax,[bp+14h]
        test    ax,1
        je      done
        mov     cx,[bp+0Eh]
        mov     di,[bp+0Ah]
        mov     si,[bp+6]
        mov     dx,[bp+0Ch]
tail:
        and     BYTE PTR es:[di],0Fh
        mov     al,[si]
        or      es:[di],al
        add     si,0A0h
        inc     dx
        test    dx,3
        je      tail_wrap
        add     di,2000h
        jmp     SHORT tail_next
tail_wrap:
        sub     di,6000h
        add     di,0A0h
tail_next:
        loop    tail
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_24CAB  ENDP

        END
