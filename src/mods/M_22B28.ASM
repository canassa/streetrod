        .MODEL MEDIUM

; F_22B28 @ 21A0:1128 (linear 0x22B28, 18 bytes)
; Install an alternate set of blit hooks: copy the 15-entry far-pointer table
; at g_5bd0 over the hook table starting at b_78a2.

        .DATA
        EXTRN   _b_78a2:DWORD
        EXTRN   _g_5bd0:DWORD

        .CODE S21A0_TEXT

        PUBLIC  _F_22B28
_F_22B28  PROC    FAR
        push    di
        push    si
        mov     di,OFFSET _b_78a2
        mov     si,OFFSET _g_5bd0
        push    ds
        pop     es
        mov     cx,1Eh
        repne   movsw
        pop     si
        pop     di
        ret
_F_22B28  ENDP

        END
