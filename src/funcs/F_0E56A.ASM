; F_0E56A @ 0000:E56A (linear 0x0E56A, 26 bytes)
; Is there RAM at es:3FFFh that holds a written value? Result in ZF
; (equal: the byte at 7FFFh read back the same after the increment).

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_0E56A
_F_0E56A  PROC  FAR
        mov     bl,es:[7FFFh]
        mov     cl,es:[3FFFh]
        inc     BYTE PTR es:[3FFFh]
        cmp     bl,es:[3FFFh]
        mov     es:[3FFFh],cl
        ret
_F_0E56A  ENDP

        END
