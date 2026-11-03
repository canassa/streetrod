; F_02C2B @ 0000:2C2B (linear 0x02C2B, 23 bytes)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_4774:BYTE
        EXTRN   _g_4799:BYTE
        EXTRN   _g_479a:BYTE
        EXTRN   _g_479f:BYTE

        .CODE

        PUBLIC  _F_02C2B
_F_02C2B  PROC  FAR
        cli
        mov     _g_4774,0
        mov     _g_4799,0
        mov     _g_479a,0
        mov     _g_479f,1Fh
        sti
        ret
_F_02C2B  ENDP

        END
