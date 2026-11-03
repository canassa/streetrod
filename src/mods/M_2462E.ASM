        .MODEL MEDIUM

; sr-member 0x2462E
; F_2462E @ 2462:000E (linear 0x2462E, 76 bytes)
; Near helper shared by F_2467A and F_24740, which keep its tables in their own
; frame: [bp-110h] 16 bytes (copy of g_4eee), [bp-120h] 16 bytes (copy of
; g_4efe), [bp-100h] 256 bytes mapping a colour byte of the palette passed at
; [bp+10h] to its index+1.  Then clears the destination buffer ([bp+0Ah] far,
; [bp+0Eh] bytes).

        .DATA
        EXTRN   _g_4eee:BYTE
        EXTRN   _g_4efe:BYTE

        .CODE

        PUBLIC  _F_2462E
_F_2462E  PROC  NEAR
        push    ss
        pop     es
        mov     cx,8
        lea     di,[bp-110h]
        mov     si,OFFSET _g_4eee
        rep     movsw
        mov     cx,8
        lea     di,[bp-120h]
        mov     si,OFFSET _g_4efe
        rep     movsw
        sub     ax,ax
        mov     cx,80h
        lea     di,[bp-100h]
        rep     stosw
        mov     si,[bp+10h]
        mov     dx,16
        xor     ax,ax
L2462E_index_loop:
        mov     bx,ax
        mov     di,[bx+si]
        and     di,0FFh
        inc     al
        mov     [bp+di-100h],al
        cmp     ax,dx
        jl      L2462E_index_loop
        les     di,DWORD PTR [bp+0ah]
        mov     cx,[bp+0eh]
        shr     cx,1
        xor     ax,ax
        rep     stosw
        ret
_F_2462E  ENDP

; sr-member 0x2467A
; F_2467A @ 2462:005A (linear 0x2467A, 198 bytes)
; F_2467A(src_far, dst_far, count, palette): like F_24740, but ORs the pixels
; into the destination, one bit plane at a time: the plane colour word is kept
; at [bp-122h] (1111h, shifted left after each pass over the buffer).

        .CODE

        PUBLIC  _F_2467A
_F_2467A  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,122h
        push    di
        push    si
        push    ds
        call    _F_2462E
        les     di,DWORD PTR [bp+0ah]
        lds     si,DWORD PTR [bp+6]
        mov     dx,di
        add     dx,[bp+0eh]
        jnc     L2467A_start
        jmp     L2467A_done
L2467A_start:
        mov     ax,1111h
        mov     [bp-122h],ax
L2467A_next:
        cmp     di,dx
        jb      L2467A_decode
        les     di,DWORD PTR [bp+0ah]
        mov     ax,[bp-122h]
        shl     ax,1
        mov     [bp-122h],ax
        jnc     L2467A_decode
        jmp     L2467A_done
L2467A_decode:
        call    _F_24814
        or      al,al
        je      L2467A_skip
        cmp     al,0FFh
        je      L2467A_fill
        call    _F_247CB
        jmp     L2467A_next
L2467A_skip:
        shl     cx,1
        shl     cx,1
L2467A_skip_again:
        mov     bx,di
        add     bx,cx
        jc      L2467A_skip_wrap
        cmp     bx,dx
        jnb     L2467A_skip_wrap
        mov     di,bx
        jmp     L2467A_next
L2467A_skip_wrap:
        mov     ax,dx
        sub     ax,di
        sub     cx,ax
        les     di,DWORD PTR [bp+0ah]
        mov     ax,[bp-122h]
        shl     ax,1
        mov     [bp-122h],ax
        jc      L2467A_done
        jmp     L2467A_skip_again
L2467A_fill:
        shl     cx,1
        shl     cx,1
        mov     bx,cx
L2467A_fill_again:
        add     bx,di
        jc      L2467A_fill_wrap
        cmp     bx,dx
        jb      L2467A_fill_last
L2467A_fill_wrap:
        mov     ax,dx
        sub     ax,di
        mov     bx,cx
        sub     bx,ax
        mov     cx,ax
        shr     cx,1
        mov     ax,[bp-122h]
        jcxz    L2467A_fill_next
L2467A_fill_loop:
        or      es:[di],ax
        inc     di
        inc     di
        loop    L2467A_fill_loop
L2467A_fill_next:
        les     di,DWORD PTR [bp+0ah]
        mov     ax,[bp-122h]
        shl     ax,1
        mov     [bp-122h],ax
        jc      L2467A_done
        mov     cx,bx
        jmp     L2467A_fill_again
L2467A_fill_last:
        shr     cx,1
        mov     ax,[bp-122h]
        jcxz    L2467A_fill_end
L2467A_fill_loop2:
        or      es:[di],ax
        inc     di
        inc     di
        loop    L2467A_fill_loop2
L2467A_fill_end:
        jmp     L2467A_next
L2467A_done:
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_2467A  ENDP

; sr-member 0x24740
; F_24740 @ 2462:0120 (linear 0x24740, 139 bytes)
; F_24740(src_far, dst_far, count, palette): expand a compressed 4-bit image.
; Each pattern byte gives 8 bits; every set bit makes one 4-bit pixel 0Fh in
; the two output words.  Code 0 skips pixels, 0FFh fills with 0FFFFh.

        .CODE

        PUBLIC  _F_24740
_F_24740  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,122h
        push    di
        push    si
        push    ds
        call    _F_2462E
        les     di,DWORD PTR [bp+0ah]
        lds     si,DWORD PTR [bp+6]
        mov     dx,di
        add     dx,[bp+0eh]
        jnc     L24740_next
        jmp     SHORT L24740_done
L24740_next:
        call    _F_24814
        or      al,al
        je      L24740_skip
        cmp     al,0FFh
        je      L24740_fill
        xor     bx,bx
        rol     al,1
        jnc     L24740_w1
        or      bl,0F0h
L24740_w1:
        rol     al,1
        jnc     L24740_w2
        or      bl,0Fh
L24740_w2:
        rol     al,1
        jnc     L24740_w3
        or      bh,0F0h
L24740_w3:
        rol     al,1
        jnc     L24740_w4
        or      bh,0Fh
L24740_w4:
        mov     WORD PTR es:[di],bx
        inc     di
        inc     di
        xor     bx,bx
        rol     al,1
        jnc     L24740_w5
        or      bl,0F0h
L24740_w5:
        rol     al,1
        jnc     L24740_w6
        or      bl,0Fh
L24740_w6:
        rol     al,1
        jnc     L24740_w7
        or      bh,0F0h
L24740_w7:
        rol     al,1
        jnc     L24740_w8
        or      bh,0Fh
L24740_w8:
        mov     WORD PTR es:[di],bx
        inc     di
        inc     di
        jmp     SHORT L24740_check
L24740_skip:
        shl     cx,1
        shl     cx,1
        add     di,cx
        jmp     SHORT L24740_check
L24740_fill:
        shl     cx,1
        mov     ax,0FFFFh
        rep     stosw
L24740_check:
        cmp     di,dx
        jae     L24740_done
        jmp     L24740_next
L24740_done:
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_24740  ENDP

; sr-member 0x247CB
; F_247CB @ 2462:01AB (linear 0x247CB, 73 bytes)
; Near helper of F_2467A: expand the 8 bits of al into two words of 4-bit
; pixels, masking the colour word kept in the caller's frame ([bp-122h]), and
; OR them into the destination (es:di).

        .CODE

        PUBLIC  _F_247CB
_F_247CB  PROC  NEAR
        mov     bx,[bp-122h]
        mov     cx,bx
        rol     al,1
        jc      L247CB_b1
        and     bl,0Fh
L247CB_b1:
        rol     al,1
        jc      L247CB_b2
        and     bl,0F0h
L247CB_b2:
        rol     al,1
        jc      L247CB_b3
        and     bh,0Fh
L247CB_b3:
        rol     al,1
        jc      L247CB_b4
        and     bh,0F0h
L247CB_b4:
        or      es:[di],bx
        inc     di
        inc     di
        rol     al,1
        jc      L247CB_b5
        and     cl,0Fh
L247CB_b5:
        rol     al,1
        jc      L247CB_b6
        and     cl,0F0h
L247CB_b6:
        rol     al,1
        jc      L247CB_b7
        and     ch,0Fh
L247CB_b7:
        rol     al,1
        jc      L247CB_b8
        and     ch,0F0h
L247CB_b8:
        or      es:[di],cx
        inc     di
        inc     di
        ret
_F_247CB  ENDP

; sr-member 0x24814
; F_24814 @ 2462:01F4 (linear 0x24814, 53 bytes)
; Near helper of F_2467A and F_24740: fetch the next code from ds:si.
; A code byte of 0 or 0FFh is followed by a count byte: returns cx = count+1
; and al = the code.  Any other byte is a pixel pattern looked up through the
; 256-byte table in the caller's frame: returns al = pattern, cx = L24814_run
; length, or ah = 0 (return) when the byte isn't in the table.

        .CODE

        PUBLIC  _F_24814
_F_24814  PROC  NEAR
        lodsb
        or      al,al
        je      L24814_run
        cmp     al,0FFh
        je      L24814_run
        mov     bl,al
        xor     bh,bh
        xchg    bx,di
        mov     ah,[bp+di-100h]
        xchg    bx,di
        or      ah,ah
        jne     L24814_found
        ret
L24814_found:
        dec     ah
        mov     bl,ah
        xchg    bx,di
        mov     al,[bp+di-120h]
        mov     cl,[bp+di-110h]
        xor     ch,ch
        xchg    bx,di
        ret
L24814_run:
        mov     cl,al
        lodsb
        xor     ah,ah
        inc     ax
        xchg    cx,ax
        ret
_F_24814  ENDP

; sr-member 0x24849
; F_24849 @ 2462:0229 (linear 0x24849, 48 bytes)
; F_24849(src_far, dst_far, count): turn count bytes of packed 4-bit pixels
; into a byte-per-pair mask: a nibble that is zero sets the corresponding
; nibble of the mask (0F0h for the high one, 0Fh for the low one).

        .CODE

        PUBLIC  _F_24849
_F_24849  PROC  FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        push    ds
        les     di,DWORD PTR [bp+0ah]
        mov     cx,[bp+0eh]
        lds     si,DWORD PTR [bp+6]
        mov     bh,0F0h
        mov     bl,0Fh
L24849_next:
        xor     ah,ah
        lodsb
        xchg    ah,al
        test    ah,0F0h
        jne     L24849_low_nibble
        mov     al,bh
L24849_low_nibble:
        test    ah,0Fh
        jne     L24849_store
        or      al,bl
L24849_store:
        stosb
        loop    L24849_next
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_24849  ENDP
        END
