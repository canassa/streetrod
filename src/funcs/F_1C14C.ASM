; F_1C14C @ 1C11:003C (linear 0x1C14C, 136 bytes)
; Copy rows of bytes into an interleaved (CGA/Hercules style) screen:
; the destination row address alternates between the even bank and the
; odd bank 0x2000 bytes further on. On a Hercules segment (B000-B400 with
; g_0042 set) an odd starting row is also written 0x2000 bytes up first.
; F_1C14C(src far, dst far, width, rows, src_skip, dst_skip, row)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_0042:WORD

        .CODE

        PUBLIC  _F_1C14C
_F_1C14C  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        mov     ah,0
        cmp     _g_0042,0
        je      setup
        les     di,DWORD PTR [bp+10]    ; dst
        mov     cx,es
        shr     di,1
        shr     di,1
        shr     di,1
        shr     di,1
        add     cx,di
        cmp     cx,0B000h
        jb      setup
        cmp     cx,0B400h
        ja      setup
        mov     ah,1
setup:
        lds     si,DWORD PTR [bp+6]     ; src
        les     di,DWORD PTR [bp+10]    ; dst
        mov     al,[bp+16]              ; rows
        mov     dx,2000h
        sub     dx,[bp+20]              ; dst_skip
        mov     bx,[bp+22]              ; first row number
rowloop:
        cmp     ah,0
        je      copy
        test    bx,1
        je      copy
        push    di
        add     di,2000h
        mov     cx,[bp+14]              ; width in bytes
        shr     cx,1
        jnc     even1
        movsb
even1:
        rep movsw
        sub     si,[bp+14]
        pop     di
copy:
        mov     cx,[bp+14]
        shr     cx,1
        jnc     even2
        movsb
even2:
        rep movsw
        dec     al
        je      done
        add     si,[bp+18]              ; src_skip
        inc     bx
        test    bx,1
        je      evenrow
        add     di,2000h
        sub     di,[bp+14]
        jmp     rowloop
evenrow:
        sub     di,dx
        jmp     rowloop
done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_1C14C  ENDP

        END
