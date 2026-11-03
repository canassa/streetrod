; F_2467A @ 2462:005A (linear 0x2467A, 198 bytes)
; F_2467A(src_far, dst_far, count, palette): like F_24740, but ORs the pixels
; into the destination, one bit plane at a time: the plane colour word is kept
; at [bp-122h] (1111h, shifted left after each pass over the buffer).

        .MODEL  MEDIUM
        .CODE
        EXTRN   _F_2462E:NEAR
        EXTRN   _F_247CB:NEAR
        EXTRN   _F_24814:NEAR

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
        jnc     start
        jmp     done
start:
        mov     ax,1111h
        mov     [bp-122h],ax
next:
        cmp     di,dx
        jb      decode
        les     di,DWORD PTR [bp+0ah]
        mov     ax,[bp-122h]
        shl     ax,1
        mov     [bp-122h],ax
        jnc     decode
        jmp     done
decode:
        call    _F_24814
        or      al,al
        je      skip
        cmp     al,0FFh
        je      fill
        call    _F_247CB
        jmp     next
skip:
        shl     cx,1
        shl     cx,1
skip_again:
        mov     bx,di
        add     bx,cx
        jc      skip_wrap
        cmp     bx,dx
        jnb     skip_wrap
        mov     di,bx
        jmp     next
skip_wrap:
        mov     ax,dx
        sub     ax,di
        sub     cx,ax
        les     di,DWORD PTR [bp+0ah]
        mov     ax,[bp-122h]
        shl     ax,1
        mov     [bp-122h],ax
        jc      done
        jmp     skip_again
fill:
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
        jc      done
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
        jmp     next
done:
        pop     ds
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_2467A  ENDP

        END
