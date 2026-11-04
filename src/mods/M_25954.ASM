        .MODEL MEDIUM

; F_25954 @ 2595:0004 (linear 0x25954, 2549 bytes)
; F_25954(dst_far, src_seg, si_off, bx_off, bx_si_pitch, di_pitch, w, h)
; sibling of F_24F4A: dst = (bx[] & dst) | si[] with the destination also
; the mask, 16 bits at a time, fully unrolled (80 words, then one trailing
; byte for odd widths).  The width picks the entry point in the unrolled
; code (jmp dx), so exactly w/2 words are L25954_done per row.  Entries with an
; offset below 80h assemble to 14 bytes, the 16 above to 18 bytes.
; Its scratch variables live in the code segment, in the 6 bytes it jumps
; over (routine at offset 04h of its code segment).

        .CODE S2595_TEXT

        PUBLIC  _F_25954
_F_25954  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        cmp     WORD PTR [bp+14h],0
        jg      L25954_haveh
        jmp     L25954_done
L25954_haveh:
        cmp     WORD PTR [bp+12h],0
        jg      L25954_havew
        jmp     L25954_done
savebp  DW      0
dipitch DW      0
sipitch DW      0
L25954_havew:
        mov     cs:savebp,bp
        mov     ds,[bp+0Ah]
        mov     si,[bp+0Ch]
        les     di,DWORD PTR [bp+6]
        mov     ax,[bp+16h]
        mov     cs:dipitch,ax
        mov     ax,[bp+10h]
        mov     cs:sipitch,ax
        mov     cx,[bp+12h]
        mov     ch,cl
        shr     cl,1
        mov     dx,40h
        test    ch,80h
        je      L25954_narrow
        mov     dx,50h
L25954_narrow:
        sub     dl,cl
        shl     dx,1
        mov     ax,dx
        shl     dx,1
        shl     dx,1
        shl     dx,1
        test    ch,80h
        jne     L25954_wide
        sub     dx,ax
        add     dx,120h
        jmp     SHORT L25954_havedx
L25954_wide:
        add     dx,ax
L25954_havedx:
        and     ch,1
        je      L25954_evenw
        jmp     NEAR PTR L25954_oddw
L25954_evenw:
        add     dx,OFFSET L25954_evenwords
        mov     cx,[bp+14h]
        mov     bx,[bp+0Eh]
        mov     bp,cs:dipitch
L25954_evenrow:
        jmp     dx
L25954_evenwords:
k       =       9Eh
        REPT    80
        mov     ax,[bx+k]
        and     ax,es:[di+k]
        or      ax,[si+k]
        mov     es:[di+k],ax
k       =       k-2
        ENDM
        mov     ax,cs:sipitch
        add     si,ax
        add     bx,ax
        add     di,bp
        dec     cx
        je      L25954_evenexit
        jmp     L25954_evenrow
L25954_evenexit:
        jmp     NEAR PTR L25954_finish
L25954_oddw:
        sub     dx,12h
        add     dx,OFFSET L25954_oddwords
        mov     cx,[bp+14h]
        mov     bx,[bp+0Eh]
        mov     bp,cs:dipitch
L25954_oddrow:
        jmp     dx
L25954_oddwords:
k       =       9Dh
        REPT    79
        mov     ax,[bx+k]
        and     ax,es:[di+k]
        or      ax,[si+k]
        mov     es:[di+k],ax
k       =       k-2
        ENDM
        mov     al,[bx]
        and     al,es:[di]
        or      al,[si]
        mov     es:[di],al
        mov     ax,cs:sipitch
        add     si,ax
        add     bx,ax
        add     di,bp
        dec     cx
        je      L25954_finish
        jmp     L25954_oddrow
L25954_finish:
        mov     bp,cs:savebp
L25954_done:
        pop     si
        pop     di
        pop     ds
        pop     bp
        ret
_F_25954  ENDP

        END
