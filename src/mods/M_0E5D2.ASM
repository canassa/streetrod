        .MODEL MEDIUM

; sr-member 0x0E5D2
; F_0E5D2 @ 0000:E5D2 (linear 0x0E5D2, 61 bytes)
; Hercules graphics mode set up: BIOS video state, CRTC registers.

        .DATA













































        ; data (recovered DGROUP layout)
        PUBLIC _g_5abc
_g_5abc LABEL BYTE ; +5ABC int @[10]
        DW 03500h
        DW 02801h
        DW 02C02h
        DW 0803h
        DW 0FC04h
        DW 0705h
        DW 06406h
        DW 07207h
        DW 0208h
        DW 0209h
        PUBLIC _g_5ad0
_g_5ad0 LABEL BYTE ; +5AD0 char @[30]
        DB 07h
        DB 050h
        DB 00h
        DB 00h
        DB 080h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 00h
        DB 0B4h
        DB 03h
        DB 02h
        DB 00h

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
L0E5D2_crtc:
        lodsw
        out     dx,ax
        loop    L0E5D2_crtc
        add     dl,4
        mov     al,_g_5ad0+28
        out     dx,al
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_0E5D2  ENDP
        END
