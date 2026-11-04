        .MODEL MEDIUM

        .DATA
        EXTRN   _g_47d4:WORD
        EXTRN   _g_47d6:WORD

        .CODE S0000_TEXT

; F_02DE2 @ 0000:2DE2 (linear 0x02DE2, 18 bytes)

        PUBLIC  _F_02DE2
_F_02DE2  PROC  FAR
        push    ds
        mov     dx,_g_47d6
        mov     ax,_g_47d4
        mov     ds,ax
        mov     ah,25h
        mov     al,9
        int     21h
        pop     ds
        ret
_F_02DE2  ENDP

        END
