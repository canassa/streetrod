; F_0E62F @ 0000:E62F (linear 0x0E62F, 146 bytes)
; Video adapter detection: saves the current mode and equipment byte,
; asks the EGA BIOS, falls back to probing the CRTC cursor register (VGA/CGA).

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_0250:WORD
        EXTRN   _g_0252:WORD
        EXTRN   _g_5aee:BYTE
        EXTRN   _g_5aef:BYTE
        EXTRN   _g_5af0:BYTE
        EXTRN   _g_5af1:BYTE
        EXTRN   _g_5af2:BYTE

        .CODE

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
