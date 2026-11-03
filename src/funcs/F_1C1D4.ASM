; F_1C1D4 @ 1C11:00C4 (linear 0x1C1D4, 228 bytes)
; Copy a rectangle between two interleaved (CGA/Hercules style) screens
; of 80 bytes per line, the odd lines living 0x2000 bytes after the even ones:
; F_1C1D4(src far, dst far, width, rows, src_x, src_y, dst_x, dst_y)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_0042:WORD

        .CODE

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
        mov     WORD PTR [bp-2],1
setup:
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
        je      srceven
        add     si,2000h
srceven:
        test    bx,1
        je      dsteven
        add     di,2000h
dsteven:
        mov     ax,2000h
        sub     ax,[bp+14]
        mov     [bp-4],ax               ; step from an even line to the odd one
        mov     ax,50h
        sub     ax,2000h
        sub     ax,[bp+14]
        mov     [bp-6],ax               ; step from an odd line to the next even one
        mov     ax,[bp+16]              ; rows
rowloop:
        cmp     WORD PTR [bp-2],0
        je      copy
        test    bx,1
        je      copy
        push    di
        add     di,2000h
        mov     cx,[bp+14]
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
        dec     ax
        je      done
        inc     dx
        test    dx,1
        je      srcnext
        add     si,[bp-4]
        jmp     SHORT srcdone
srcnext:
        add     si,[bp-6]
srcdone:
        inc     bx
        test    bx,1
        je      dstnext
        add     di,[bp-4]
        jmp     rowloop
dstnext:
        add     di,[bp-6]
        jmp     rowloop
done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_1C1D4  ENDP

        END
