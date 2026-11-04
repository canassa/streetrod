        .MODEL MEDIUM

        .DATA
        EXTRN   _b_78a2:DWORD
        EXTRN   _g_5b8a:DWORD

        .CODE S21A0_TEXT

; tandydrv_install @ 21A0:0952 (linear 0x22352, 18 bytes)
; Install an alternate set of blit hooks: copy the 15-entry far-pointer table
; at g_5b8a over the hook table starting at b_78a2.

        PUBLIC  _tandydrv_install
_tandydrv_install  PROC    FAR
        push    di
        push    si
        mov     di,OFFSET _b_78a2
        mov     si,OFFSET _g_5b8a
        push    ds
        pop     es
        mov     cx,1Eh
        repne   movsw
        pop     si
        pop     di
        ret
_tandydrv_install  ENDP

        END
