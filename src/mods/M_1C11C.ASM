        .MODEL MEDIUM

; F_1C11C @ 1c11:000c (linear 0x1C11C, 48 bytes)
; Copy a rectangle of bytes between far buffers:
; blit_rect(src far, dst far, width, rows, src_skip, dst_skip)

        .CODE S1C11_TEXT

        PUBLIC  _blit_rect
_blit_rect  PROC  FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        cld
        lds     si,DWORD PTR [bp+6]     ; src
        les     di,DWORD PTR [bp+10]    ; dst
        mov     ax,[bp+16]              ; rows
        mov     dx,[bp+20]              ; dst_skip
        mov     bx,[bp+18]              ; src_skip
L1C11C_rowloop:
        mov     cx,[bp+14]              ; width in bytes
        shr     cx,1
        jnc     L1C11C_no_odd
        movsb
L1C11C_no_odd:
        rep movsw
        dec     ax
        je      L1C11C_done
        add     si,bx
        add     di,dx
        jmp     L1C11C_rowloop
L1C11C_done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_blit_rect  ENDP

; F_1C14C @ 1C11:003C (linear 0x1C14C, 136 bytes)
; Copy rows of bytes into an interleaved (CGA/Hercules style) screen:
; the destination row address alternates between the even bank and the
; odd bank 0x2000 bytes further on. On a Hercules segment (B000-B400 with
; g_0042 set) an odd starting row is also written 0x2000 bytes up first.
; F_1C14C(src far, dst far, width, rows, src_skip, dst_skip, row)

        .DATA
        EXTRN   _g_0042:WORD

        .CODE S1C11_TEXT

        PUBLIC  _F_1C14C
_F_1C14C  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        mov     ah,0
        cmp     _g_0042,0
        je      L1C14C_setup
        les     di,DWORD PTR [bp+10]    ; dst
        mov     cx,es
        shr     di,1
        shr     di,1
        shr     di,1
        shr     di,1
        add     cx,di
        cmp     cx,0B000h
        jb      L1C14C_setup
        cmp     cx,0B400h
        ja      L1C14C_setup
        mov     ah,1
L1C14C_setup:
        lds     si,DWORD PTR [bp+6]     ; src
        les     di,DWORD PTR [bp+10]    ; dst
        mov     al,[bp+16]              ; rows
        mov     dx,2000h
        sub     dx,[bp+20]              ; dst_skip
        mov     bx,[bp+22]              ; first row number
L1C14C_rowloop:
        cmp     ah,0
        je      L1C14C_copy
        test    bx,1
        je      L1C14C_copy
        push    di
        add     di,2000h
        mov     cx,[bp+14]              ; width in bytes
        shr     cx,1
        jnc     L1C14C_even1
        movsb
L1C14C_even1:
        rep movsw
        sub     si,[bp+14]
        pop     di
L1C14C_copy:
        mov     cx,[bp+14]
        shr     cx,1
        jnc     L1C14C_even2
        movsb
L1C14C_even2:
        rep movsw
        dec     al
        je      L1C14C_done
        add     si,[bp+18]              ; src_skip
        inc     bx
        test    bx,1
        je      L1C14C_evenrow
        add     di,2000h
        sub     di,[bp+14]
        jmp     L1C14C_rowloop
L1C14C_evenrow:
        sub     di,dx
        jmp     L1C14C_rowloop
L1C14C_done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_1C14C  ENDP

; F_1C1D4 @ 1C11:00C4 (linear 0x1C1D4, 228 bytes)
; Copy a rectangle between two interleaved (CGA/Hercules style) screens
; of 80 bytes per line, the odd lines living 0x2000 bytes after the even ones:
; F_1C1D4(src far, dst far, width, rows, src_x, src_y, dst_x, dst_y)

        .DATA
        EXTRN   _g_0042:WORD

        .CODE S1C11_TEXT

        PUBLIC  _F_1C1D4
_F_1C1D4  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,6
        push    ds
        push    di
        push    si
        mov     WORD PTR [bp-2],0
        cmp     _g_0042,0
        je      L1C1D4_setup
        les     di,DWORD PTR [bp+10]    ; dst
        mov     cx,es
        shr     di,1
        shr     di,1
        shr     di,1
        shr     di,1
        add     cx,di
        cmp     cx,0B000h
        jb      L1C1D4_setup
        cmp     cx,0B400h
        ja      L1C1D4_setup
        mov     WORD PTR [bp-2],1
L1C1D4_setup:
        lds     si,DWORD PTR [bp+6]     ; src
        les     di,DWORD PTR [bp+10]    ; dst
        mov     ax,50h
        mov     bx,[bp+24]              ; dst_y
        mov     bh,bl
        shr     bl,1
        mul     bl
        add     di,ax
        add     di,[bp+22]              ; dst_x
        mov     ax,50h
        mov     dx,[bp+20]              ; src_y
        mov     dh,dl
        shr     dl,1
        mul     dl
        add     si,ax
        add     si,[bp+18]              ; src_x
        mov     dl,dh
        xor     dh,dh
        mov     bl,bh
        xor     bh,bh
        test    dx,1
        je      L1C1D4_srceven
        add     si,2000h
L1C1D4_srceven:
        test    bx,1
        je      L1C1D4_dsteven
        add     di,2000h
L1C1D4_dsteven:
        mov     ax,2000h
        sub     ax,[bp+14]
        mov     [bp-4],ax               ; step from an even line to the odd one
        mov     ax,50h
        sub     ax,2000h
        sub     ax,[bp+14]
        mov     [bp-6],ax               ; step from an odd line to the next even one
        mov     ax,[bp+16]              ; rows
L1C1D4_rowloop:
        cmp     WORD PTR [bp-2],0
        je      L1C1D4_copy
        test    bx,1
        je      L1C1D4_copy
        push    di
        add     di,2000h
        mov     cx,[bp+14]
        shr     cx,1
        jnc     L1C1D4_even1
        movsb
L1C1D4_even1:
        rep movsw
        sub     si,[bp+14]
        pop     di
L1C1D4_copy:
        mov     cx,[bp+14]
        shr     cx,1
        jnc     L1C1D4_even2
        movsb
L1C1D4_even2:
        rep movsw
        dec     ax
        je      L1C1D4_done
        inc     dx
        test    dx,1
        je      L1C1D4_srcnext
        add     si,[bp-4]
        jmp     SHORT L1C1D4_srcdone
L1C1D4_srcnext:
        add     si,[bp-6]
L1C1D4_srcdone:
        inc     bx
        test    bx,1
        je      L1C1D4_dstnext
        add     di,[bp-4]
        jmp     L1C1D4_rowloop
L1C1D4_dstnext:
        add     di,[bp-6]
        jmp     L1C1D4_rowloop
L1C1D4_done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_1C1D4  ENDP

; F_1C2B8 @ 1C11:01A8 (linear 0x1C2B8, 18 bytes)
; Install the default blit hooks: copy the 15-entry far-pointer table
; at g_5b04 over the hook table starting at b_78a2.

        .DATA
        EXTRN   _b_78a2:DWORD
        EXTRN   _g_5b04:DWORD

        .CODE S1C11_TEXT

        PUBLIC  _F_1C2B8
_F_1C2B8  PROC    FAR
        push    di
        push    si
        mov     di,OFFSET _b_78a2
        mov     si,OFFSET _g_5b04
        push    ds
        pop     es
        mov     cx,1Eh
        repne   movsw
        pop     si
        pop     di
        ret
_F_1C2B8  ENDP

        END
