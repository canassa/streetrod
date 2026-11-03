; F_24814 @ 2462:01F4 (linear 0x24814, 53 bytes)
; Near helper of F_2467A and F_24740: fetch the next code from ds:si.
; A code byte of 0 or 0FFh is followed by a count byte: returns cx = count+1
; and al = the code.  Any other byte is a pixel pattern looked up through the
; 256-byte table in the caller's frame: returns al = pattern, cx = run
; length, or ah = 0 (return) when the byte isn't in the table.

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_24814
_F_24814  PROC  NEAR
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
_F_24814  ENDP

        END
