        .MODEL MEDIUM

; sr-member 0x24F4A
; F_24F4A @ 24F4:000A (linear 0x24F4A, 2569 bytes)
; F_24F4A(dst_far, src_seg, si_off, bx_off, bp_off, bx_si_pitch, w, h,
;         bp_pitch, di_pitch): masked blit, three source streams in the
; segment src_seg: dst = (bx[] & bp[]) | si[], 16 bits at a time, fully
; unrolled (80 words, then one trailing byte for odd widths).
; The width picks the entry point in the unrolled code: the routine jumps
; into it (jmp dx) so exactly w/2 words are copied per row.  Entries with an
; offset below 80h assemble to 14 bytes, the 16 above to 18 bytes.
; Its scratch variables live in the code segment, in the 8 bytes it jumps
; over, so they are addressed through CS at fixed offsets (the routine sits
; at offset 0Ah of its code segment).

        .CODE

        ORG     0Ah

        PUBLIC  _F_24F4A
_F_24F4A  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        cmp     WORD PTR [bp+16h],0
        jg      L24F4A_haveh
        jmp     L24F4A_done
L24F4A_haveh:
        cmp     WORD PTR [bp+14h],0
        jg      L24F4A_havew
        jmp     L24F4A_done
savebp  DW      0
bppitch DW      0
dipitch DW      0
sipitch DW      0
L24F4A_havew:
        mov     cs:savebp,bp
        mov     ds,[bp+0Ah]
        mov     si,[bp+0Ch]
        mov     bx,[bp+0Eh]
        les     di,DWORD PTR [bp+6]
        mov     ax,[bp+1Ah]
        mov     cs:dipitch,ax
        mov     ax,[bp+18h]
        mov     cs:bppitch,ax
        mov     ax,[bp+12h]
        mov     cs:sipitch,ax
        mov     cx,[bp+14h]
        mov     ch,cl
        shr     cl,1
        mov     dx,40h
        test    ch,80h
        je      L24F4A_narrow
        mov     dx,50h
L24F4A_narrow:
        sub     dl,cl
        shl     dx,1
        mov     ax,dx
        shl     dx,1
        shl     dx,1
        shl     dx,1
        test    ch,80h
        jne     L24F4A_wide
        sub     dx,ax
        add     dx,120h
        jmp     SHORT L24F4A_havedx
L24F4A_wide:
        add     dx,ax
L24F4A_havedx:
        and     ch,1
        je      L24F4A_evenw
        jmp     NEAR PTR L24F4A_oddw
L24F4A_evenw:
        add     dx,OFFSET L24F4A_evenwords
        mov     cx,[bp+16h]
        mov     bp,[bp+10h]
L24F4A_evenrow:
        jmp     dx
L24F4A_evenwords:
k       =       9Eh
        REPT    80
        mov     ax,[bx+k]
        and     ax,ds:[bp+k]
        or      ax,[si+k]
        mov     es:[di+k],ax
k       =       k-2
        ENDM
        mov     ax,cs:sipitch
        add     si,ax
        add     bx,ax
        add     bp,cs:bppitch
        add     di,cs:dipitch
        dec     cx
        je      L24F4A_evenexit
        jmp     L24F4A_evenrow
L24F4A_evenexit:
        jmp     NEAR PTR L24F4A_finish
L24F4A_oddw:
        sub     dx,12h
        add     dx,OFFSET L24F4A_oddwords
        mov     cx,[bp+16h]
        mov     bp,[bp+10h]
L24F4A_oddrow:
        jmp     dx
L24F4A_oddwords:
k       =       9Dh
        REPT    79
        mov     ax,[bx+k]
        and     ax,ds:[bp+k]
        or      ax,[si+k]
        mov     es:[di+k],ax
k       =       k-2
        ENDM
        mov     al,[bx]
        and     al,ds:[bp]
        or      al,[si]
        mov     es:[di],al
        mov     ax,cs:sipitch
        add     si,ax
        add     bx,ax
        add     bp,cs:bppitch
        add     di,cs:dipitch
        dec     cx
        je      L24F4A_finish
        jmp     L24F4A_oddrow
L24F4A_finish:
        mov     bp,cs:savebp
L24F4A_done:
        pop     si
        pop     di
        pop     ds
        pop     bp
        ret
_F_24F4A  ENDP
        END
