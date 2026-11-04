        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _g_lastKey:BYTE
        EXTRN   _g_oldInt9Seg:WORD
        EXTRN   _g_oldInt9Off:WORD

        .CODE S0000_TEXT
        EXTRN   _kbd_isr:FAR

; kbdget_key @ 0000:2DB6 (linear 0x02DB6, 13 bytes)

        PUBLIC  _kbdget_key
_kbdget_key  PROC  FAR
        cli
        mov     al,_g_lastKey
        xor     ah,ah
        mov     _g_lastKey,0
        sti
        ret
_kbdget_key  ENDP

; kbdget_hook @ 0000:2DC3 (linear 0x02DC3, 31 bytes)

        PUBLIC  _kbdget_hook
_kbdget_hook  PROC  FAR
        push    ds
        mov     ah,DOS_GET_VECTOR
        mov     al,KEYBOARD_INT
        int     DOS_INT
        mov     _g_oldInt9Seg,es
        mov     _g_oldInt9Off,bx
        mov     ax,SEG _kbd_isr
        mov     ds,ax
        mov     dx,OFFSET _kbd_isr
        mov     ah,DOS_SET_VECTOR
        mov     al,KEYBOARD_INT
        int     DOS_INT
        pop     ds
        ret
_kbdget_hook  ENDP

        END
