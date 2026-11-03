        .MODEL MEDIUM

; sr-member 0x02C04
; F_02C04 @ 0000:2C04 (linear 0x02C04, 39 bytes)

        .DATA
        EXTRN   _g_4766:BYTE
        EXTRN   _g_4732:BYTE

        .CODE

        PUBLIC  _F_02C04
_F_02C04  PROC  FAR
        push    bp
        mov     bp,sp
        cli
        test    WORD PTR [bp+6],0FFFFh
        jne     L02C04_L1
        mov     _g_4766+57,20h
L02C04_L1:     mov     _g_4766+14,8
        mov     _g_4766+51,2Ch
        mov     _g_4766+52,2Eh
        and     _g_4732,0FEh
        sti
        pop     bp
        ret
_F_02C04  ENDP
        END
