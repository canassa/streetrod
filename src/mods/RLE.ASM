        .MODEL MEDIUM

        .DATA
        EXTRN   _g_4eee:BYTE
        EXTRN   _g_4efe:BYTE

        .CODE S0E92_TEXT

; rle_decode @ 0E92:0006 (linear 0x0E926, 201 bytes)
; Run-length decoder with a byte dictionary (tables g_4eee and g_4efe copied to
; the stack). 0 and 0FFh start a literal run: the next byte is the repeat count.

        PUBLIC  _rle_decode
_rle_decode  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,12Ch
        push    di
        push    si
        push    es
        push    ds
        mov     WORD PTR [bp-8],80h
        lea     ax,[bp-2Ah]
        mov     [bp-2],ax
        sub     ax,ax
        mov     cx,80h
        lea     di,[bp-12Ah]
        push    ss
        pop     es
        rep     stosw
        sub     cx,cx
        mov     si,[bp+10h]
        mov     dx,[bp+12h]
        jmp     test_n
fill_idx:
        mov     bx,cx
        mov     di,[bx+si]
        and     di,0FFh
        mov     ax,cx
        inc     al
        mov     [bp+di-12Ah],al
        inc     cx
test_n:
        cmp     cx,dx
        jl      fill_idx
        mov     [bp-8],cx
        mov     WORD PTR [bp-8],0
        mov     cx,8
        lea     di,[bp-2Ah]
        mov     si,OFFSET _g_4eee
        push    ss
        pop     es
        rep     movsw
        mov     cx,8
        lea     di,[bp-1Ah]
        mov     si,OFFSET _g_4efe
        rep     movsw
        mov     dx,[bp+0Eh]
        mov     [bp-0Ah],dx
        les     di,DWORD PTR [bp+6]
        lds     bx,DWORD PTR [bp+0Ah]
next:
        cmp     dx,0
        jle     fin
        dec     dx
        mov     al,es:[di]
        inc     di
        or      al,al
        je      run
        cmp     al,0FFh
        jne     other
run:
        dec     dx
        mov     cl,es:[di]
        inc     di
        sub     ch,ch
        add     [bp-0Ah],cx
        dec     WORD PTR [bp-0Ah]
        inc     cx
fill:
        mov     [bx],al
        inc     bx
        dec     cx
        jne     fill
        jmp     next
other:
        sub     ah,ah
        mov     si,ax
        mov     ah,[bp+si-12Ah]
        or      ah,ah
        jne     coded
        mov     [bx],al
        inc     bx
        jmp     next
coded:
        mov     al,ah
        sub     ah,ah
        mov     si,ax
        mov     al,[bp+si-1Bh]
        mov     cl,[bp+si-2Bh]
        sub     ch,ch
        add     [bp-0Ah],cx
        dec     WORD PTR [bp-0Ah]
        jmp     fill
fin:
        mov     ax,[bp-0Ah]
        pop     ds
        pop     es
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_rle_decode  ENDP
        EVEN

        END
