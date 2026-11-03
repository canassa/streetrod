; F_02C04 @ 0000:2C04 (linear 0x02C04, 39 bytes)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_4732:BYTE
        EXTRN   _g_4774:BYTE
        EXTRN   _g_4799:BYTE
        EXTRN   _g_479a:BYTE
        EXTRN   _g_479f:BYTE

        .CODE

        PUBLIC  _F_02C04
_F_02C04  PROC  FAR
        push    bp
        mov     bp,sp
        cli
        test    WORD PTR [bp+6],0FFFFh
        jne     L1
        mov     _g_479f,20h
L1:     mov     _g_4774,8
        mov     _g_4799,2Ch
        mov     _g_479a,2Eh
        and     _g_4732,0FEh
        sti
        pop     bp
        ret
_F_02C04  ENDP

        END
