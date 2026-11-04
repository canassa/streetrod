        .MODEL MEDIUM

        .DATA

; DS:5ABC int[10]
        PUBLIC  _g_5abc
_g_5abc DW      3500h, 2801h, 2C02h, 803h, 0FC04h, 705h, 6406h, 7207h
        DW      208h, 209h
        PUBLIC  _g_5ad0
_g_5ad0 DB      7, 'P', 0                ; DS:5AD0 char[30]
        DB      0, 80h, 21 DUP (0), 0B4h, 3, 2, 0

        .CODE S0000_TEXT

; hercgfx_set_mode @ 0000:E5D2 (linear 0x0E5D2, 61 bytes)
; Hercules graphics mode set up: BIOS video state, CRTC registers.

        PUBLIC  _hercgfx_set_mode
_hercgfx_set_mode  PROC    NEAR
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
        mov     al,_g_5ad0+28
        out     dx,al
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_hercgfx_set_mode  ENDP

        END
