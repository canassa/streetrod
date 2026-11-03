        .MODEL MEDIUM

; sr-member 0x0E56A
; F_0E56A @ 0000:E56A (linear 0x0E56A, 26 bytes)
; Is there RAM at es:3FFFh that holds a written value? Result in ZF
; (equal: the byte at 7FFFh read back the same after the increment).

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

; sr-member 0x0E584
; F_0E584 @ 0000:E584 (linear 0x0E584, 63 bytes)
; Hercules detection: does the status port 3BAh toggle bit 7, and is
; there memory at B000h that survives the config switch.

        .CODE


        PUBLIC  _F_0E584
_F_0E584  PROC  FAR
        mov     bx,100h
        mov     dx,3BAh
        in      al,dx
        mov     ah,al
        and     ah,80h
L0E584_wait1:
        mov     cx,40h
L0E584_wait2:
        in      al,dx
        and     al,80h
        cmp     al,ah
        loope   L0E584_wait2
        jne     L0E584_toggled
        dec     bx
        jne     L0E584_wait1
        jmp     SHORT L0E584_none
L0E584_toggled:
        mov     ax,0B000h
        mov     es,ax
        call    _F_0E56A
        jne     L0E584_found
        mov     al,1
        mov     dx,3BFh
        out     dx,al
        call    _F_0E56A
        je      L0E584_none
L0E584_found:
        mov     al,1
        jmp     SHORT L0E584_done
L0E584_none:
        xor     al,al
L0E584_done:
        or      al,al
        ret
_F_0E584  ENDP

; sr-member 0x0E5C3
; F_0E5C3 @ 0000:E5C3 (linear 0x0E5C3, 14 bytes)
; BIOS equipment word: 0x30 in the video bits means monochrome.

        .CODE

        PUBLIC  _F_0E5C3
_F_0E5C3  PROC  FAR
        int     11h
        and     ax,30h
        cmp     ax,30h
        je      L0E5C3_mono
        mov     ax,0
L0E5C3_mono:
        ret
_F_0E5C3  ENDP
        END
