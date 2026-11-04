        .MODEL MEDIUM

        .DATA
        EXTRN   _g_0610:WORD
        EXTRN   _g_472a:WORD
        EXTRN   _g_472c:WORD
        EXTRN   _g_472e:BYTE
        EXTRN   _g_472f:BYTE
        EXTRN   _g_4730:BYTE
        EXTRN   _g_4732:BYTE
        EXTRN   _g_4736:BYTE
        EXTRN   _g_4737:BYTE
        EXTRN   _g_4738:WORD
        EXTRN   _g_473a:WORD
        EXTRN   _g_473c:WORD
        EXTRN   _g_473e:WORD
        EXTRN   _g_4740:WORD
        EXTRN   _g_4766:BYTE
        EXTRN   _g_47cd:BYTE
        EXTRN   _g_47ce:BYTE
        EXTRN   _g_47d0:BYTE
        EXTRN   _g_47e8:WORD
        EXTRN   _g_47ee:WORD

        .CODE S0000_TEXT

; F_02C04 @ 0000:2C04 (linear 0x02C04, 39 bytes)

        PUBLIC  _F_02C04
_F_02C04  PROC  FAR
        push    bp
        mov     bp,sp
        cli
        test    WORD PTR [bp+6],0FFFFh
        jne     skip57
        mov     _g_4766+57,20h
skip57:
        mov     _g_4766+14,8
        mov     _g_4766+51,2Ch
        mov     _g_4766+52,2Eh
        and     _g_4732,0FEh
        sti
        pop     bp
        ret
_F_02C04  ENDP

; F_02C2B @ 0000:2C2B (linear 0x02C2B, 23 bytes)

        PUBLIC  _F_02C2B
_F_02C2B  PROC  FAR
        cli
        mov     _g_4766+14,0
        mov     _g_4766+51,0
        mov     _g_4766+52,0
        mov     _g_4766+57,1Fh
        sti
        ret
_F_02C2B  ENDP

; F_02C42 @ 0000:2C42 (linear 0x02C42, 366 bytes)
; int 9 keyboard ISR (installed by F_02DC3)

        PUBLIC  _F_02C42
_F_02C42  PROC  FAR
        push    ax
        push    bx
        push    ds
        push    es
        in      al,60h
        xor     ah,ah
        push    ax
        in      al,61h
        mov     ah,al
        or      al,80h
        out     61h,al
        mov     al,ah
        out     61h,al
        mov     ax,DGROUP
        mov     ds,ax
        mov     es,ax
        pop     ax
        xor     ah,ah
        cmp     _g_0610,0
        je      L1
        cmp     al,1Dh
        je      L1
        cmp     al,9Dh
        je      L1
        cmp     al,32h
        je      L1
        cmp     al,1
        jne     L0
        mov     _g_0610,63h
L0:
        jmp     done
L1:
        cmp     al,0FFh
        jne     L2
        jmp     done
L2:
        cmp     al,0E0h
        jb      L3
        jmp     done
L3:
        test    al,80h
        pushf
        and     al,7Fh
        cmp     al,55h
        jbe     L4
        pop     ax
        xor     ax,ax
        pushf
        mov     ax,56h
L4:
        mov     bx,ax
        mov     al,_g_4766[bx]
        cmp     al,0
        je      L9
        jg      L10
        cmp     al,0E2h
        jl      L10
        cbw
        mov     bx,ax
        popf
        jne     L6
        cmp     BYTE PTR _g_47e8[bx],0
        jne     done
        mov     BYTE PTR _g_47e8[bx],1
        mov     _g_4740,0FFFFh
        mov     al,_g_4766[bx]
        add     _g_472e,al
        mov     al,_g_4766[bx+1]
        add     _g_472f,al
        mov     ax,WORD PTR _g_4766[bx]
        mov     WORD PTR _g_4730,ax     ; g_4730 and g_4731
        mov     _g_4740,0
        jmp     SHORT done
L6:
        cmp     BYTE PTR _g_47e8[bx],0
        je      done
        mov     _g_4740,0FFFFh
        mov     BYTE PTR _g_47e8[bx],0
        mov     al,_g_4766[bx]
        sub     _g_472e,al
        mov     al,_g_4766[bx+1]
        sub     _g_472f,al
        mov     _g_4740,0
        jmp     SHORT done
L9:
        popf
        jmp     SHORT done
L10:
        popf
        jne     L11
        cmp     al,1Fh
        je      L14
        cmp     al,1Eh
        je      L16
        cmp     al,1Dh
        jne     L12
        xor     _g_47ce,1
        jmp     SHORT done
L12:
        mov     _g_47d0,al
        jmp     SHORT done
L11:
        cmp     al,1Fh
        je      L15
        cmp     al,1Eh
        je      L17
done:
        mov     al,20h
        out     20h,al
        pop     es
        pop     ds
        pop     bx
        pop     ax
        iret
L14:
        cmp     _g_4732,0
        jne     L14b
        mov     _g_4740,0FFFFh
        mov     _g_4736,1Fh
        mov     ax,_g_472a
        mov     _g_4738,ax
        mov     ax,_g_472c
        mov     _g_473a,ax
        mov     _g_4740,0
L14b:
        or      _g_4732,1
        mov     _g_47cd,1
        mov     _g_47ee,1
        jmp     SHORT done
L15:
        mov     _g_4740,0FFFFh
        and     _g_4732,0FEh
        mov     _g_4740,0
        jmp     SHORT done
L16:
        cmp     _g_4732,0
        jne     L16b
        mov     _g_4740,0FFFFh
        mov     _g_4737,1Eh
        mov     ax,_g_472a
        mov     _g_473c,ax
        mov     ax,_g_472c
        mov     _g_473e,ax
        mov     _g_4740,0
L16b:
        jmp     SHORT done
L17:
        jmp     SHORT done
_F_02C42  ENDP

        END
