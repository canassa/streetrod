        .MODEL MEDIUM

        .CODE S2595_TEXT

; F_25954 @ 2595:0004 (linear 0x25954, 2549 bytes)
; F_25954(dst_far, src_seg, si_off, bx_off, bx_si_pitch, di_pitch, w, h)
; sibling of F_24F4A: dst = (bx[] & dst) | si[] with the destination also
; the mask, 16 bits at a time, fully unrolled (80 words, then one trailing
; byte for odd widths).  The width picks the entry point in the unrolled
; code (jmp dx), so exactly w/2 words are done per row.  Entries with an
; offset below 80h assemble to 14 bytes, the 16 above to 18 bytes.
; Its scratch variables live in the code segment, in the 6 bytes it jumps
; over (routine at offset 04h of its code segment).

        PUBLIC  _F_25954
_F_25954  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        cmp     WORD PTR [bp+14h],0
        jg      haveh
        jmp     done
haveh:
        cmp     WORD PTR [bp+12h],0
        jg      havew
        jmp     done
savebp  DW      0
dipitch DW      0
sipitch DW      0
havew:
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
        je      narrow
        mov     dx,50h
narrow:
        sub     dl,cl
        shl     dx,1
        mov     ax,dx
        shl     dx,1
        shl     dx,1
        shl     dx,1
        test    ch,80h
        jne     wide
        sub     dx,ax
        add     dx,120h
        jmp     SHORT havedx
wide:
        add     dx,ax
havedx:
        and     ch,1
        je      evenw
        jmp     NEAR PTR oddw
evenw:
        add     dx,OFFSET evenwords
        mov     cx,[bp+14h]
        mov     bx,[bp+0Eh]
        mov     bp,cs:dipitch
evenrow:
        jmp     dx
evenwords:
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
        je      evenexit
        jmp     evenrow
evenexit:
        jmp     NEAR PTR finish
oddw:
        sub     dx,12h
        add     dx,OFFSET oddwords
        mov     cx,[bp+14h]
        mov     bx,[bp+0Eh]
        mov     bp,cs:dipitch
oddrow:
        jmp     dx
oddwords:
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
        je      finish
        jmp     oddrow
finish:
        mov     bp,cs:savebp
done:
        pop     si
        pop     di
        pop     ds
        pop     bp
        ret
_F_25954  ENDP

        END
