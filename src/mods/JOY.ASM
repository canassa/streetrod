        .MODEL MEDIUM

        .DATA
        EXTRN   _g_4718:WORD
        EXTRN   _g_471c:BYTE
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
        EXTRN   _g_47cc:BYTE
        EXTRN   _g_47ce:BYTE
        EXTRN   _g_47d0:BYTE
        EXTRN   _g_47e8:WORD

        .CODE S0000_TEXT

; F_02DF4 @ 0000:2DF4 (linear 0x02DF4, 159 bytes)

        PUBLIC  _F_02DF4
_F_02DF4  PROC  FAR
        push    ax
        push    bx
        push    cx
        push    dx
        push    di
        push    bp
        push    ds
        push    es
        mov     ax,DGROUP
        mov     ds,ax
        mov     es,ax
        mov     _g_47e8,sp
        cli
        mov     dx,201h
        mov     cx,400h
        mov     bx,3
        out     dx,al
        out     dx,al
poll:
        in      al,dx
        and     al,bl
        cmp     al,bl
        loope   poll
        jcxz    timeout
        xor     al,bl
        push    ax
        push    cx
        dec     cx
        xor     bl,al
        jne     poll
        sti
        mov     dl,2
        mov     di,OFFSET _g_4718
next:
        pop     bx
        sub     bx,400h
        neg     bx
        mov     bp,bx
        pop     ax
        mov     cx,2
axis:
        shr     al,1
        jae     skip
        mov     bx,bp
        sub     bx,[di+6]
        mov     [di],bx
        dec     dl
skip:
        inc     di
        inc     di
        loop    axis
        sub     di,4
        or      dl,dl
        jne     next
        mov     dx,201h
        in      al,dx
        not     al
        mov     cl,4
        shr     al,cl
        and     al,3
        mov     _g_471c,al
        je      nobtn
        cmp     _g_4732,0
        jne     held
        mov     _g_4736,al
        mov     ax,_g_472a
        mov     _g_4738,ax
        mov     ax,_g_472c
        mov     _g_473a,ax
held:
        or      _g_4732,2
        jmp     SHORT done1
nobtn:
        and     _g_4732,0FDh
done1:
        pop     es
        pop     ds
        pop     bp
        pop     di
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret
timeout:
        sti
        mov     sp,_g_47e8
        jmp     done1
_F_02DF4  ENDP

; F_02E93 @ 0000:2E93 (linear 0x02E93, 309 bytes)
; keyboard scan code handler, variant of the int 9 ISR F_02C42 (retf, no port I/O)

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
        jb      L3
        jmp     done2
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
        jne     done2
        mov     BYTE PTR _g_47e8[bx],1
        mov     _g_4740,0FFFFh
        mov     al,_g_4766[bx]
        add     _g_472e,al
        mov     al,_g_4766[bx+1]
        add     _g_472f,al
        mov     ax,WORD PTR _g_4766[bx]
        mov     WORD PTR _g_4730,ax     ; g_4730 and g_4731
        mov     _g_4740,0
        jmp     SHORT done2
L6:
        cmp     BYTE PTR _g_47e8[bx],0
        je      done2
        mov     _g_4740,0FFFFh
        mov     BYTE PTR _g_47e8[bx],0
        mov     al,_g_4766[bx]
        sub     _g_472e,al
        mov     al,_g_4766[bx+1]
        sub     _g_472f,al
        mov     _g_4740,0
        jmp     SHORT done2
L9:
        popf
        jmp     SHORT done2
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
        jmp     SHORT done2
L12:
        mov     _g_47d0,al
        jmp     SHORT done2
L11:
        cmp     al,1Fh
        je      L15
        cmp     al,1Eh
        je      L17
done2:
        mov     al,20h
        pop     es
        pop     ds
        pop     bx
        pop     ax
        ret
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
        jmp     SHORT done2
L15:
        mov     _g_4740,0FFFFh
        and     _g_4732,0FEh
        mov     _g_4740,0
        jmp     SHORT done2
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
        jmp     SHORT done2
L17:
        jmp     SHORT done2
_F_02E93  ENDP

        END
