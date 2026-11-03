; F_0E5D2 @ 0000:E5D2 (linear 0x0E5D2, 61 bytes)
; Hercules graphics mode set up: BIOS video state, CRTC registers.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_5ad0:BYTE
        EXTRN   _g_5abc:WORD
        EXTRN   _g_5aec:BYTE

        .CODE

        PUBLIC  _F_0E5D2
_F_0E5D2  PROC    NEAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,40h
        mov     es,ax
        mov     di,49h
        mov     si,OFFSET _g_5ad0
        mov     cx,1Eh
        even
        rep     movsb
        mov     dx,3BFh
        mov     al,1
        out     dx,al
        mov     dx,3B8h
        xor     al,al
        out     dx,al
        sub     dl,4
        mov     si,OFFSET _g_5abc
        mov     cx,0Ah
        even
crtc:
        lodsw
        out     dx,ax
        loop    crtc
        add     dl,4
        mov     al,_g_5aec
        out     dx,al
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_0E5D2  ENDP

        END
