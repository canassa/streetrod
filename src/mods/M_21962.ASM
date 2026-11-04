        .MODEL SMALL
; Written as a small-model file: .MODEL SMALL declares _TEXT (empty here) before any
; other segment, which is what orders the _TEXT of the runtime between segments
; 1C11 and 2196. Small model rejects .CODE name (A2063), so the far code has its own SEGMENT.

        .DATA
        EXTRN   _g_58e2:WORD

        ; data (recovered DGROUP layout)
        PUBLIC  _g_58de
_g_58de DW      1                        ; DS:58DE int
        PUBLIC  _g_58e0
_g_58e0 DW      1                        ; DS:58E0 int

S2196_TEXT SEGMENT WORD PUBLIC 'CODE'
        ASSUME  CS:S2196_TEXT

; F_21962 @ 2196:0002 (linear 0x21962, 27 bytes)
; Busy-wait delay: spin for n * g_58e2 units, in 16-bit chunks of a LOOP count
; (the high byte of the product counts the outer repetitions).
; F_21962(n)

        PUBLIC  _F_21962
_F_21962  PROC    FAR
        push    bp
        mov     bp,sp
        mov     ax,[bp+6]
        mul     _g_58e2
        xor     bh,bh
        mov     bl,dh
        mov     dh,dl
        mov     dl,ah
again:
        mov     cx,dx
spin1:
        loop    spin1
        dec     bx
        jns     again
        pop     bp
        ret
_F_21962  ENDP

; F_2197D @ 1C11:586D (linear 0x2197D, 136 bytes)
; Play a tune on the PC speaker. The argument points at a table of
; 3-word records (length, two frequency counters, duty/step word) ending
; with a zero length word; g_58de keeps the read pointer so the caller's
; table advances one record per step. The speaker is toggled through port 61h,
; g_58e0 is the delay count of the half-wave loop.
; F_2197D(table)

        PUBLIC  _F_2197D
_F_2197D  PROC    FAR
        push    bp
        mov     bp,sp
        mov     bx,[bp+6]
        mov     _g_58de,bx
next:
        mov     bx,_g_58de
        mov     ax,[bx]
        cmp     ax,0
        je      done
        mov     si,ax
        inc     bx
        inc     bx
        mov     dx,[bx]
        inc     bx
        inc     bx
        mov     dh,dl
        mov     dl,2
        mov     cx,[bx]
        inc     bx
        inc     bx
        mov     _g_58de,bx
        mov     bh,cl
        mov     cl,1
        xor     bl,bl
        mov     bp,2FFh
        cmp     bh,0
        jne     tone
        xor     bp,200h
        mov     bh,64h
tone:
        xor     ax,ax
        mov     di,ax
        sub     dl,1
        rcl     ax,1
        neg     ax
        and     al,dh
        add     dl,al
        mov     di,ax
        xor     ax,ax
        sub     cl,1
        rcl     ax,1
        neg     ax
        and     al,ch
        add     cl,al
        or      di,ax
        xor     ax,ax
        sub     bl,1
        rcl     ax,1
        sub     si,ax
        je      next
        neg     ax
        and     al,bh
        add     bl,al
        or      ax,di
        mov     al,4Ch
        out     61h,al
        and     ax,bp
        or      al,ah
        out     61h,al
        mov     ax,cx
        mov     cx,_g_58e0
spin2:
        loop    spin2
        mov     cx,ax
        jmp     tone
done:
        pop     bp
        ret
_F_2197D  ENDP
S2196_TEXT ENDS

        END
