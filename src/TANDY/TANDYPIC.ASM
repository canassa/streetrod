        .MODEL MEDIUM

        .DATA
        EXTRN   _g_rleCounts:BYTE
        EXTRN   _g_rleValues:BYTE

        .CODE S2462_TEXT

; tp_init @ 2462:000E (linear 0x2462E, 76 bytes)
; Near helper shared by tp_unpack and tp_unpack_mask, which keep its tables in their own
; frame: [bp-110h] 16 bytes (copy of g_rleCounts), [bp-120h] 16 bytes (copy of
; g_rleValues), [bp-100h] 256 bytes mapping a colour byte of the palette passed at
; [bp+10h] to its index+1.  Then clears the destination buffer ([bp+0Ah] far,
; [bp+0Eh] bytes).

        PUBLIC  _tp_init
_tp_init  PROC  NEAR
        push    ss
        pop     es
        mov     cx,8
        lea     di,[bp-110h]
        mov     si,OFFSET _g_rleCounts
        rep     movsw
        mov     cx,8
        lea     di,[bp-120h]
        mov     si,OFFSET _g_rleValues
        rep     movsw
        sub     ax,ax
        mov     cx,80h
        lea     di,[bp-100h]
        rep     stosw
        mov     si,[bp+10h]
        mov     dx,16
        xor     ax,ax
index_loop:
        mov     bx,ax
        mov     di,[bx+si]
        and     di,0FFh
        inc     al
        mov     [bp+di-100h],al
        cmp     ax,dx
        jl      index_loop
        les     di,DWORD PTR [bp+0ah]
        mov     cx,[bp+0eh]
        shr     cx,1
        xor     ax,ax
        rep     stosw
        ret
_tp_init  ENDP

; tp_unpack @ 2462:005A (linear 0x2467A, 198 bytes)
; tp_unpack(src_far, dst_far, count, palette): like tp_unpack_mask, but ORs the pixels
; into the destination, one bit plane at a time: the plane colour word is kept
; at [bp-122h] (1111h, shifted left after each pass over the buffer).

        PUBLIC  _tp_unpack
_tp_unpack  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,122h
        push    di
        push    si
        push    ds
        call    _tp_init
        les     di,DWORD PTR [bp+0ah]
        lds     si,DWORD PTR [bp+6]
        mov     dx,di
        add     dx,[bp+0eh]
        jnc     start
        jmp     done1
start:
        mov     ax,1111h
        mov     [bp-122h],ax
next1:
        cmp     di,dx
        jb      decode
        les     di,DWORD PTR [bp+0ah]
        mov     ax,[bp-122h]
        shl     ax,1
        mov     [bp-122h],ax
        jnc     decode
        jmp     done1
decode:
        call    _tp_read_code
        or      al,al
        je      skip1
        cmp     al,0FFh
        je      fill1
        call    _tp_or_nibbles
        jmp     next1
skip1:
        shl     cx,1
        shl     cx,1
skip_again:
        mov     bx,di
        add     bx,cx
        jc      skip_wrap
        cmp     bx,dx
        jnb     skip_wrap
        mov     di,bx
        jmp     next1
skip_wrap:
        mov     ax,dx
        sub     ax,di
        sub     cx,ax
        les     di,DWORD PTR [bp+0ah]
        mov     ax,[bp-122h]
        shl     ax,1
        mov     [bp-122h],ax
        jc      done1
        jmp     skip_again
fill1:
        shl     cx,1
        shl     cx,1
        mov     bx,cx
fill_again:
        add     bx,di
        jc      fill_wrap
        cmp     bx,dx
        jb      fill_last
fill_wrap:
        mov     ax,dx
        sub     ax,di
        mov     bx,cx
        sub     bx,ax
        mov     cx,ax
        shr     cx,1
        mov     ax,[bp-122h]
        jcxz    fill_next
fill_loop:
        or      es:[di],ax
        inc     di
        inc     di
        loop    fill_loop
fill_next:
        les     di,DWORD PTR [bp+0ah]
        mov     ax,[bp-122h]
        shl     ax,1
        mov     [bp-122h],ax
        jc      done1
        mov     cx,bx
        jmp     fill_again
fill_last:
        shr     cx,1
        mov     ax,[bp-122h]
        jcxz    fill_end
fill_loop2:
        or      es:[di],ax
        inc     di
        inc     di
        loop    fill_loop2
fill_end:
        jmp     next1
done1:
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_tp_unpack  ENDP

; tp_unpack_mask @ 2462:0120 (linear 0x24740, 139 bytes)
; tp_unpack_mask(src_far, dst_far, count, palette): expand a compressed 4-bit image.
; Each pattern byte gives 8 bits; every set bit makes one 4-bit pixel 0Fh in
; the two output words.  Code 0 skips pixels, 0FFh fills with 0FFFFh.

        PUBLIC  _tp_unpack_mask
_tp_unpack_mask  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,122h
        push    di
        push    si
        push    ds
        call    _tp_init
        les     di,DWORD PTR [bp+0ah]
        lds     si,DWORD PTR [bp+6]
        mov     dx,di
        add     dx,[bp+0eh]
        jnc     next2
        jmp     SHORT done2
next2:
        call    _tp_read_code
        or      al,al
        je      skip2
        cmp     al,0FFh
        je      fill2
        xor     bx,bx
        rol     al,1
        jnc     w1
        or      bl,0F0h
w1:
        rol     al,1
        jnc     w2
        or      bl,0Fh
w2:
        rol     al,1
        jnc     w3
        or      bh,0F0h
w3:
        rol     al,1
        jnc     w4
        or      bh,0Fh
w4:
        mov     WORD PTR es:[di],bx
        inc     di
        inc     di
        xor     bx,bx
        rol     al,1
        jnc     w5
        or      bl,0F0h
w5:
        rol     al,1
        jnc     w6
        or      bl,0Fh
w6:
        rol     al,1
        jnc     w7
        or      bh,0F0h
w7:
        rol     al,1
        jnc     w8
        or      bh,0Fh
w8:
        mov     WORD PTR es:[di],bx
        inc     di
        inc     di
        jmp     SHORT check
skip2:
        shl     cx,1
        shl     cx,1
        add     di,cx
        jmp     SHORT check
fill2:
        shl     cx,1
        mov     ax,0FFFFh
        rep     stosw
check:
        cmp     di,dx
        jae     done2
        jmp     next2
done2:
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_tp_unpack_mask  ENDP

; tp_or_nibbles @ 2462:01AB (linear 0x247CB, 73 bytes)
; Near helper of tp_unpack: expand the 8 bits of al into two words of 4-bit
; pixels, masking the colour word kept in the caller's frame ([bp-122h]), and
; OR them into the destination (es:di).

        PUBLIC  _tp_or_nibbles
_tp_or_nibbles  PROC  NEAR
        mov     bx,[bp-122h]
        mov     cx,bx
        rol     al,1
        jc      b1
        and     bl,0Fh
b1:
        rol     al,1
        jc      b2
        and     bl,0F0h
b2:
        rol     al,1
        jc      b3
        and     bh,0Fh
b3:
        rol     al,1
        jc      b4
        and     bh,0F0h
b4:
        or      es:[di],bx
        inc     di
        inc     di
        rol     al,1
        jc      b5
        and     cl,0Fh
b5:
        rol     al,1
        jc      b6
        and     cl,0F0h
b6:
        rol     al,1
        jc      b7
        and     ch,0Fh
b7:
        rol     al,1
        jc      b8
        and     ch,0F0h
b8:
        or      es:[di],cx
        inc     di
        inc     di
        ret
_tp_or_nibbles  ENDP

; tp_read_code @ 2462:01F4 (linear 0x24814, 53 bytes)
; Near helper of tp_unpack and tp_unpack_mask: fetch the next code from ds:si.
; A code byte of 0 or 0FFh is followed by a count byte: returns cx = count+1
; and al = the code.  Any other byte is a pixel pattern looked up through the
; 256-byte table in the caller's frame: returns al = pattern, cx = run
; length, or ah = 0 (return) when the byte isn't in the table.

        PUBLIC  _tp_read_code
_tp_read_code  PROC  NEAR
        lodsb
        or      al,al
        je      run
        cmp     al,0FFh
        je      run
        mov     bl,al
        xor     bh,bh
        xchg    bx,di
        mov     ah,[bp+di-100h]
        xchg    bx,di
        or      ah,ah
        jne     found
        ret
found:
        dec     ah
        mov     bl,ah
        xchg    bx,di
        mov     al,[bp+di-120h]
        mov     cl,[bp+di-110h]
        xor     ch,ch
        xchg    bx,di
        ret
run:
        mov     cl,al
        lodsb
        xor     ah,ah
        inc     ax
        xchg    cx,ax
        ret
_tp_read_code  ENDP

; tp_make_mask @ 2462:0229 (linear 0x24849, 48 bytes)
; tp_make_mask(src_far, dst_far, count): turn count bytes of packed 4-bit pixels
; into a byte-per-pair mask: a nibble that is zero sets the corresponding
; nibble of the mask (0F0h for the high one, 0Fh for the low one).

        PUBLIC  _tp_make_mask
_tp_make_mask  PROC  FAR
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
next3:
        xor     ah,ah
        lodsb
        xchg    ah,al
        test    ah,0F0h
        jne     low_nibble
        mov     al,bh
low_nibble:
        test    ah,0Fh
        jne     store
        or      al,bl
store:
        stosb
        loop    next3
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_tp_make_mask  ENDP

        END
