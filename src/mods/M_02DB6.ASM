        .MODEL MEDIUM

        .DATA
        EXTRN   _g_47d0:BYTE
        EXTRN   _g_47d4:WORD
        EXTRN   _g_47d6:WORD

        .CODE S0000_TEXT
        EXTRN   _F_02C42:FAR

; F_02DB6 @ 0000:2DB6 (linear 0x02DB6, 13 bytes)

        PUBLIC  _F_02DB6
_F_02DB6  PROC  FAR
        cli
        mov     al,_g_47d0
        xor     ah,ah
        mov     _g_47d0,0
        sti
        ret
_F_02DB6  ENDP

; F_02DC3 @ 0000:2DC3 (linear 0x02DC3, 31 bytes)

        PUBLIC  _F_02DC3
_F_02DC3  PROC  FAR
        push    ds
        mov     ah,35h
        mov     al,9
        int     21h
        mov     _g_47d4,es
        mov     _g_47d6,bx
        mov     ax,SEG _F_02C42
        mov     ds,ax
        mov     dx,OFFSET _F_02C42
        mov     ah,25h
        mov     al,9
        int     21h
        pop     ds
        ret
_F_02DC3  ENDP

        END
