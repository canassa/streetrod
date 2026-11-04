        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _g_oldInt9Seg:WORD
        EXTRN   _g_oldInt9Off:WORD

        .CODE S0000_TEXT

        PUBLIC  _ko_unhook
_ko_unhook  PROC  FAR
        push    ds
        mov     dx,_g_oldInt9Off
        mov     ax,_g_oldInt9Seg
        mov     ds,ax
        mov     ah,DOS_SET_VECTOR
        mov     al,KEYBOARD_INT
        int     DOS_INT
        pop     ds
        ret
_ko_unhook  ENDP

        END
