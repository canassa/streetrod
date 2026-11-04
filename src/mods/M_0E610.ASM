        .MODEL MEDIUM

        .DATA
        EXTRN   _F_0E781:FAR
        EXTRN   _g_0250:WORD
        EXTRN   _g_0252:WORD

        ; data (recovered DGROUP layout)
        PUBLIC _g_5aee
_g_5aee LABEL BYTE ; +5AEE char @
        DB 00h
        PUBLIC _g_5aef
_g_5aef LABEL BYTE ; +5AEF char @
        DB 00h
        PUBLIC _g_5af0
_g_5af0 LABEL BYTE ; +5AF0 char @
        DB 00h
        PUBLIC _g_5af1
_g_5af1 LABEL BYTE ; +5AF1 char @
        DB 00h
        PUBLIC _g_5af2
_g_5af2 LABEL BYTE ; +5AF2 char @
        DB 00h
        PUBLIC _g_5af3
_g_5af3 LABEL BYTE ; +5AF3 char @
        DB 00h
S_5af4 LABEL BYTE
        DB 'Tandy', 0 ; +5AF4
S_5afa LABEL BYTE
        DB 'Tandy', 0 ; +5AFA
S_5b00 LABEL BYTE
        DB 'CGA', 0 ; +5B00

        .CODE S0000_TEXT

; F_0E610 @ 0000:E610 (linear 0x0E610, 31 bytes)
; Restore the equipment byte and the BIOS video mode saved by F_0E62F.

        PUBLIC  _F_0E610
_F_0E610  PROC  FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        xor     ax,ax
        mov     es,ax
        mov     si,410h
        mov     al,_g_5af0
        mov     es:[si],al
        mov     ah,0
        mov     al,_g_5aee
        int     10h
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_0E610  ENDP

; F_0E62F @ 0000:E62F (linear 0x0E62F, 146 bytes)
; Video adapter detection: saves the current mode and equipment byte,
; asks the EGA BIOS, falls back to probing the CRTC cursor register (VGA/CGA).

        PUBLIC  _F_0E62F
_F_0E62F  PROC  FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        mov     ax,0F00h
        int     10h
        mov     _g_5aee,al
        xor     ax,ax
        mov     es,ax
        mov     si,410h
        mov     al,es:[si]
        mov     _g_5af0,al
        mov     ah,12h
        mov     bl,10h
        int     10h
        mov     _g_5aef,bh
        mov     _g_5af2,cl
        cmp     bl,10h
        je      probe
        mov     _g_0250,1
        mov     _g_5af1,bl
        cmp     cl,0Fh
        jge     probe
        cmp     bh,1
        jg      probe
        cmp     bl,3
        jg      probe
        cmp     cl,1
        je      mono
        cmp     cl,0Eh
        je      mono
        cmp     cl,8
        jle     probe
        mov     _g_5af1,bl
        mov     _g_0250,1
        jmp     done
probe:
        mov     dx,3D4h
        mov     al,0Fh
        out     dx,al
        inc     dx
        in      al,dx
        mov     bh,al
        mov     al,5Ah
        out     dx,al
        jmp     SHORT $+2
        jmp     SHORT $+2
        jmp     SHORT $+2
        in      al,dx
        cmp     al,5Ah
        mov     al,bh
        out     dx,al
        je      done
        mov     _g_0250,2
        jmp     done
mono:
        mov     _g_0252,1
done:
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_F_0E62F  ENDP

        END
