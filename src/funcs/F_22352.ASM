; F_22352 @ 21A0:0952 (linear 0x22352, 18 bytes)
; Install an alternate set of blit hooks: copy the 15-entry far-pointer table
; at g_5b8a over the hook table starting at g_78a2.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_78a2:DWORD
        EXTRN   _g_5b8a:DWORD

        .CODE

        PUBLIC  _F_22352
_F_22352  PROC    FAR
        push    di
        push    si
        mov     di,OFFSET _g_78a2
        mov     si,OFFSET _g_5b8a
        push    ds
        pop     es
        mov     cx,1Eh
        repne   movsw
        pop     si
        pop     di
        ret
_F_22352  ENDP

        END
