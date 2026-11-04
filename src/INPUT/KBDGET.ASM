        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _g_lastKey:BYTE
        EXTRN   _g_oldInt9Seg:WORD
        EXTRN   _g_oldInt9Off:WORD

        .CODE S0000_TEXT
        EXTRN   _kb_isr:FAR



        PUBLIC  _kg_key
_kg_key  PROC  FAR
        cli
        mov     al,_g_lastKey
        xor     ah,ah
        mov     _g_lastKey,0
        sti
        ret
_kg_key  ENDP



        PUBLIC  _kg_hook
_kg_hook  PROC  FAR
        push    ds
        mov     ah,DOS_GET_VECTOR
        mov     al,KEYBOARD_INT
        int     DOS_INT
        mov     _g_oldInt9Seg,es
        mov     _g_oldInt9Off,bx
        mov     ax,SEG _kb_isr
        mov     ds,ax
        mov     dx,OFFSET _kb_isr
        mov     ah,DOS_SET_VECTOR
        mov     al,KEYBOARD_INT
        int     DOS_INT
        pop     ds
        ret
_kg_hook  ENDP

        END
