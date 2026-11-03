        .MODEL MEDIUM

; sr-member 0x02E93
; F_02E93 @ 0000:2E93 (linear 0x02E93, 309 bytes)
; keyboard scan code handler, variant of the int 9 ISR F_02C42 (retf, no port I/O)

        .DATA
        EXTRN   _g_472a:WORD
        EXTRN   _g_472c:WORD
        EXTRN   _g_472e:BYTE
        EXTRN   _g_472f:BYTE
        EXTRN   _g_4730:WORD
        EXTRN   _g_4732:BYTE
        EXTRN   _g_4736:BYTE
        EXTRN   _g_4737:BYTE
        EXTRN   _g_4738:WORD
        EXTRN   _g_473a:WORD
        EXTRN   _g_473c:WORD
        EXTRN   _g_473e:WORD
        EXTRN   _g_4740:WORD
        EXTRN   _g_4766:BYTE
        EXTRN   _g_47cc:BYTE
        EXTRN   _g_47ce:BYTE
        EXTRN   _g_47d0:BYTE
        EXTRN   _g_47e8:WORD

        .CODE

        PUBLIC  _F_02E93
_F_02E93  PROC  FAR
        push    ax
        push    bx
        push    ds
        push    es
        mov     al,_g_47cc
        xor     ah,ah
        push    ax
        mov     ah,al
        or      al,80h
        mov     al,ah
        mov     ax,DGROUP
        mov     ds,ax
        mov     es,ax
        pop     ax
        xor     ah,ah
        cmp     al,0E0h
        jb      L02E93_L3
        jmp     L02E93_done
L02E93_L3:     test    al,80h
        pushf
        and     al,7Fh
        cmp     al,55h
        jbe     L02E93_L4
        pop     ax
        xor     ax,ax
        pushf
        mov     ax,56h
L02E93_L4:     mov     bx,ax
        mov     al,_g_4766[bx]
        cmp     al,0
        je      L02E93_L9
        jg      L02E93_L10
        cmp     al,0E2h
        jl      L02E93_L10
        cbw
        mov     bx,ax
        popf
        jne     L02E93_L6
        cmp     BYTE PTR _g_47e8[bx],0
        jne     L02E93_done
        mov     BYTE PTR _g_47e8[bx],1
        mov     _g_4740,0FFFFh
        mov     al,_g_4766[bx]
        add     _g_472e,al
        mov     al,_g_4766[bx+1]
        add     _g_472f,al
        mov     ax,WORD PTR _g_4766[bx]
        mov     _g_4730,ax
        mov     _g_4740,0
        jmp     SHORT L02E93_done
L02E93_L6:     cmp     BYTE PTR _g_47e8[bx],0
        je      L02E93_done
        mov     _g_4740,0FFFFh
        mov     BYTE PTR _g_47e8[bx],0
        mov     al,_g_4766[bx]
        sub     _g_472e,al
        mov     al,_g_4766[bx+1]
        sub     _g_472f,al
        mov     _g_4740,0
        jmp     SHORT L02E93_done
L02E93_L9:     popf
        jmp     SHORT L02E93_done
L02E93_L10:    popf
        jne     L02E93_L11
        cmp     al,1Fh
        je      L02E93_L14
        cmp     al,1Eh
        je      L02E93_L16
        cmp     al,1Dh
        jne     L02E93_L12
        xor     _g_47ce,1
        jmp     SHORT L02E93_done
L02E93_L12:    mov     _g_47d0,al
        jmp     SHORT L02E93_done
L02E93_L11:    cmp     al,1Fh
        je      L02E93_L15
        cmp     al,1Eh
        je      L02E93_L17
L02E93_done:   mov     al,20h
        pop     es
        pop     ds
        pop     bx
        pop     ax
        ret
L02E93_L14:    cmp     _g_4732,0
        jne     L02E93_L14b
        mov     _g_4740,0FFFFh
        mov     _g_4736,1Fh
        mov     ax,_g_472a
        mov     _g_4738,ax
        mov     ax,_g_472c
        mov     _g_473a,ax
        mov     _g_4740,0
L02E93_L14b:   or      _g_4732,1
        jmp     SHORT L02E93_done
L02E93_L15:    mov     _g_4740,0FFFFh
        and     _g_4732,0FEh
        mov     _g_4740,0
        jmp     SHORT L02E93_done
L02E93_L16:    cmp     _g_4732,0
        jne     L02E93_L16b
        mov     _g_4740,0FFFFh
        mov     _g_4737,1Eh
        mov     ax,_g_472a
        mov     _g_473c,ax
        mov     ax,_g_472c
        mov     _g_473e,ax
        mov     _g_4740,0
L02E93_L16b:   jmp     SHORT L02E93_done
L02E93_L17:    jmp     SHORT L02E93_done
_F_02E93  ENDP
        END
