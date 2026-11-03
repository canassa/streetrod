; F_0E584 @ 0000:E584 (linear 0x0E584, 63 bytes)
; Hercules detection: does the status port 3BAh toggle bit 7, and is
; there memory at B000h that survives the config switch.

        .MODEL  MEDIUM
        .CODE

        EXTRN   _F_0E56A:FAR

        PUBLIC  _F_0E584
_F_0E584  PROC  FAR
        mov     bx,100h
        mov     dx,3BAh
        in      al,dx
        mov     ah,al
        and     ah,80h
wait1:
        mov     cx,40h
wait2:
        in      al,dx
        and     al,80h
        cmp     al,ah
        loope   wait2
        jne     toggled
        dec     bx
        jne     wait1
        jmp     SHORT none
toggled:
        mov     ax,0B000h
        mov     es,ax
        call    _F_0E56A
        jne     found
        mov     al,1
        mov     dx,3BFh
        out     dx,al
        call    _F_0E56A
        je      none
found:
        mov     al,1
        jmp     SHORT done
none:
        xor     al,al
done:
        or      al,al
        ret
_F_0E584  ENDP

        END
