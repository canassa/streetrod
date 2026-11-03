; F_1C2B8 @ 1C11:01A8 (linear 0x1C2B8, 18 bytes)
; Install the default blit hooks: copy the 15-entry far-pointer table
; at g_5b04 over the hook table starting at g_78a2.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_78a2:DWORD
        EXTRN   _g_5b04:DWORD

        .CODE

        PUBLIC  _F_1C2B8
_F_1C2B8  PROC    FAR
        push    di
        push    si
        mov     di,OFFSET _g_78a2
        mov     si,OFFSET _g_5b04
        push    ds
        pop     es
        mov     cx,1Eh
        repne   movsw
        pop     si
        pop     di
        ret
_F_1C2B8  ENDP

        END
