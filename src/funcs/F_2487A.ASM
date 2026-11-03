; F_2487A @ 2487:000A (linear 0x2487A, 545 bytes)
; F_2487A(x1, y1, x2, y2, color): Bresenham line in the 320x200 16-colour screen
; (four interlaced banks of 2000h bytes, two pixels per byte), the sibling of
; line_ega. Vertical lines go pixel by pixel, horizontal ones fill whole bytes.
; si = bank step (+-2000h), di = step from the last bank back to the next row
; (+-0A0A0h, i.e. -5F60h).

        .MODEL  MEDIUM
        .CODE
        EXTRN   _F_24A9B:NEAR

        PUBLIC  _F_2487A
_F_2487A  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     si,2000h
        mov     di,0A0A0h
        mov     cx,[bp+10]
        sub     cx,[bp+6]               ; dx = x2 - x1
        je      vertical
        jns     x_ordered
        neg     cx
        mov     bx,[bp+10]
        xchg    [bp+6],bx
        mov     [bp+10],bx
        mov     bx,[bp+12]
        xchg    [bp+8],bx
        mov     [bp+12],bx
x_ordered:
        mov     bx,[bp+12]
        sub     bx,[bp+8]               ; dy = y2 - y1
        jne     y_nonzero
        jmp     horizontal
y_nonzero:
        jns     y_down
        neg     bx
        neg     si
        neg     di
y_down:
        mov     [bp-2],di
        mov     WORD PTR [bp-8],OFFSET x_major
        cmp     bx,cx
        jle     x_is_major
        mov     WORD PTR [bp-8],OFFSET y_major
        xchg    bx,cx
x_is_major:
        shl     bx,1
        mov     [bp-4],bx
        sub     bx,cx
        mov     di,bx
        sub     bx,cx
        mov     [bp-6],bx
        push    cx
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _F_24A9B
        mov     al,[bp+14]              ; color
        shl     ax,cl
        mov     dx,ax
        not     dh
        pop     cx
        inc     cx
        jmp     WORD PTR [bp-8]
vertical:
        mov     ax,[bp+8]
        mov     bx,[bp+12]
        mov     cx,bx
        sub     cx,ax
        jge     v_ordered
        neg     cx
        mov     ax,bx
        mov     [bp+8],ax
v_ordered:
        inc     cx
        mov     bx,[bp+6]
        push    cx
        call    _F_24A9B
        mov     al,[bp+14]
        shl     ax,cl
        not     ah
        pop     cx
v_loop:
        and     BYTE PTR es:[bx],ah
        or      BYTE PTR es:[bx],al
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        je      v_bank
        add     bx,si
        loop    v_loop
        jmp     done
v_bank:
        add     bx,di
        loop    v_loop
        jmp     done
horizontal:
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _F_24A9B
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+10]
        and     cl,1
        jne     h_odd
        mov     dl,0F0h
h_odd:
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        shr     ax,1
        shr     bx,1
        mov     cx,ax
        sub     cx,bx
        mov     al,[bp+14]
        mov     ah,al
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
        or      al,ah
        or      dh,dh
        js      h_middle
        or      cx,cx
        jne     h_first
        and     dl,dh
        jmp     SHORT h_last
h_first:
        mov     ah,al
        and     ah,dh
        not     dh
        and     BYTE PTR es:[di],dh
        or      BYTE PTR es:[di],ah
        inc     di
        dec     cx
h_middle:
        rep stosb
h_last:
        and     al,dl
        not     dl
        and     BYTE PTR es:[di],dl
        or      BYTE PTR es:[di],al
        jmp     done
x_major:
        mov     ah,es:[bx]
xm_or:
        and     ah,dh
        or      ah,dl
        ror     dl,1
        ror     dl,1
        ror     dl,1
        ror     dl,1
        ror     dh,1
        ror     dh,1
        ror     dh,1
        ror     dh,1
        jnc     xm_next_byte
        or      di,di
        jns     xm_diag
        add     di,[bp-4]
        loop    xm_or
        mov     es:[bx],ah
        jmp     done
xm_diag:
        add     di,[bp-6]
        mov     es:[bx],ah
        test    si,8000h
        je      xd_down
        test    WORD PTR [bp+8],3
        je      xd_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT xd_next
xd_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT xd_next
xd_down:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     xd_next
        sub     bx,si
        add     bx,[bp-2]
xd_next:
        loop    x_major
        jmp     done
xm_next_byte:
        mov     es:[bx],ah
        inc     bx
        or      di,di
        jns     xn_diag
        add     di,[bp-4]
        loop    x_major
        jmp     done
xn_diag:
        add     di,[bp-6]
        test    si,8000h
        je      xn_down
        test    WORD PTR [bp+8],3
        je      xn_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT xn_next
xn_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT xn_next
xn_down:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     xn_next
        sub     bx,si
        add     bx,[bp-2]
xn_next:
        dec     cx
        je      done
        jmp     x_major
y_major:
        and     BYTE PTR es:[bx],dh
        or      BYTE PTR es:[bx],dl
        test    si,8000h
        je      y_down2
        test    WORD PTR [bp+8],3
        je      y_up_bank
        add     bx,si
        dec     WORD PTR [bp+8]
        jmp     SHORT y_step
y_up_bank:
        add     bx,[bp-2]
        dec     WORD PTR [bp+8]
        jmp     SHORT y_step
y_down2:
        add     bx,si
        inc     WORD PTR [bp+8]
        test    WORD PTR [bp+8],3
        jne     y_step
        sub     bx,si
        add     bx,[bp-2]
y_step:
        or      di,di
        jns     y_diag
        add     di,[bp-4]
        loop    y_major
        jmp     SHORT done
y_diag:
        add     di,[bp-6]
        ror     dl,1
        ror     dl,1
        ror     dl,1
        ror     dl,1
        ror     dh,1
        ror     dh,1
        ror     dh,1
        ror     dh,1
        cmc
        adc     bx,0
        loop    y_major
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_2487A  ENDP

        END
