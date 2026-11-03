; F_1E0FE @ 1C11:1FEE (linear 0x1E0FE, 96 bytes)
; Fill whole screen lines with a 2-bit pattern, from line y1 down to the
; bottom clip line (g_clip_ymax). The colour is turned into a pattern
; byte via g_5b6a; every second line gets the pattern rotated by 2 bits.
; F_1E0FE(y1, colour)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_5b6a:BYTE

        .CODE

        EXTRN   _F_1DFA9:NEAR

        PUBLIC  _F_1E0FE
_F_1E0FE  PROC    FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+6]
        cmp     _g_clip_ymax,ax
        jl      done
        mov     bx,OFFSET _g_5b6a
        mov     ax,[bp+8]
        xor     ah,ah
        xlat
        mov     dh,al
        mov     dl,al
        xor     si,si
again:
        mov     ax,[bp+6]
        mov     bx,_g_clip_ymax
        mov     cx,bx
        sub     cx,ax
        shr     cx,1
        xor     ax,bx
        test    ax,1
        je      even
        inc     cx
even:
        or      cx,cx
        je      next
        mov     ax,[bp+6]
        xor     bx,bx
        push    cx
        call    _F_1DFA9
        pop     ax
        mov     cl,28h
        mul     cl
        mov     cx,ax
        mov     ax,dx
        mov     di,bx
        rep stosw
next:
        or      si,si
        jne     done
        inc     si
        inc     WORD PTR [bp+6]
        ror     dx,1
        ror     dx,1
        jmp     again
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_1E0FE  ENDP

        END
