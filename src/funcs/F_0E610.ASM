; F_0E610 @ 0000:E610 (linear 0x0E610, 31 bytes)
; Restore the equipment byte and the BIOS video mode saved by F_0E62F.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_5aee:BYTE
        EXTRN   _g_5af0:BYTE

        .CODE

        PUBLIC  _F_0E610
_F_0E610  PROC  FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        xor     ax,ax
        mov     es,ax
        mov     si,410h
        mov     al,_g_5af0
        mov     es:[si],al
        mov     ah,0
        mov     al,_g_5aee
        int     10h
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_0E610  ENDP

        END
