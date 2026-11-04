        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _g_hercules:WORD

        .CODE S1C11_TEXT

; Copy a rectangle of bytes between far buffers:
; cd_blit_rect(src far, dst far, width, rows, src_skip, dst_skip)

        PUBLIC  _cd_blit_rect
_cd_blit_rect  PROC  FAR
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
rowloop1:
        mov     cx,[bp+14]              ; width in bytes
        shr     cx,1
        jnc     no_odd
        movsb
no_odd:
        rep movsw
        dec     ax
        je      done1
        add     si,bx
        add     di,dx
        jmp     rowloop1
done1:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_cd_blit_rect  ENDP

; Copy rows of bytes into an interleaved (CGA/Hercules style) screen:
; the destination row address alternates between the even bank and the
; odd bank 0x2000 bytes further on. On a Hercules segment (B000-B400 with
; g_hercules set) an odd starting row is also written 0x2000 bytes up first.
; cd_blit_to_ilv(src far, dst far, width, rows, src_skip, dst_skip, row)

        PUBLIC  _cd_blit_to_ilv
_cd_blit_to_ilv  PROC    FAR
        push    bp
        mov     bp,sp
        push    ds
        push    di
        push    si
        mov     ah,0
        cmp     _g_hercules,0
        je      setup1
        les     di,DWORD PTR [bp+10]    ; dst
        mov     cx,es
        shr     di,1
        shr     di,1
        shr     di,1
        shr     di,1
        add     cx,di
        cmp     cx,MONO_SEGMENT
        jb      setup1
        cmp     cx,0B400h
        ja      setup1
        mov     ah,1
setup1:
        lds     si,DWORD PTR [bp+6]     ; src
        les     di,DWORD PTR [bp+10]    ; dst
        mov     al,[bp+16]              ; rows
        mov     dx,2000h
        sub     dx,[bp+20]              ; dst_skip
        mov     bx,[bp+22]              ; first row number
rowloop2:
        cmp     ah,0
        je      copy1
        test    bx,1
        je      copy1
        push    di
        add     di,2000h
        mov     cx,[bp+14]              ; width in bytes
        shr     cx,1
        jnc     even1a
        movsb
even1a:
        rep movsw
        sub     si,[bp+14]
        pop     di
copy1:
        mov     cx,[bp+14]
        shr     cx,1
        jnc     even2a
        movsb
even2a:
        rep movsw
        dec     al
        je      done2
        add     si,[bp+18]              ; src_skip
        inc     bx
        test    bx,1
        je      evenrow
        add     di,2000h
        sub     di,[bp+14]
        jmp     rowloop2
evenrow:
        sub     di,dx
        jmp     rowloop2
done2:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_cd_blit_to_ilv  ENDP

; Copy a rectangle between two interleaved (CGA/Hercules style) screens
; of 80 bytes per line, the odd lines living 0x2000 bytes after the even ones:
; cd_blit_ilv(src far, dst far, width, rows, src_x, src_y, dst_x, dst_y)

        PUBLIC  _cd_blit_ilv
_cd_blit_ilv  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,6
        push    ds
        push    di
        push    si
        mov     WORD PTR [bp-2],0
        cmp     _g_hercules,0
        je      setup2
        les     di,DWORD PTR [bp+10]    ; dst
        mov     cx,es
        shr     di,1
        shr     di,1
        shr     di,1
        shr     di,1
        add     cx,di
        cmp     cx,MONO_SEGMENT
        jb      setup2
        cmp     cx,0B400h
        ja      setup2
        mov     WORD PTR [bp-2],1
setup2:
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
rowloop3:
        cmp     WORD PTR [bp-2],0
        je      copy2
        test    bx,1
        je      copy2
        push    di
        add     di,2000h
        mov     cx,[bp+14]
        shr     cx,1
        jnc     even1b
        movsb
even1b:
        rep movsw
        sub     si,[bp+14]
        pop     di
copy2:
        mov     cx,[bp+14]
        shr     cx,1
        jnc     even2b
        movsb
even2b:
        rep movsw
        dec     ax
        je      done3
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
        jmp     rowloop3
dstnext:
        add     di,[bp-6]
        jmp     rowloop3
done3:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_cd_blit_ilv  ENDP

        END
