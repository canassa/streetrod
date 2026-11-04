        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _uc_cga:FAR
        EXTRN   _g_egaCard:WORD
        EXTRN   _g_forceEga:WORD

        PUBLIC  _g_savedVideoMode
_g_savedVideoMode DB      0
        PUBLIC  _g_egaMono
_g_egaMono DB      0
        PUBLIC  _g_savedEquip
_g_savedEquip DB      0
        PUBLIC  _g_egaMemory
_g_egaMemory DB      0
        PUBLIC  _g_egaSwitches
_g_egaSwitches DB      0
        EVEN
S_5af4  DB      'Tandy', 0
S_5afa  DB      'Tandy', 0
S_5b00  DB      'CGA', 0

        .CODE S0000_TEXT


; Restore the equipment byte and the BIOS video mode saved by vd_detect.

        PUBLIC  _vd_restore
_vd_restore  PROC  FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        xor     ax,ax
        mov     es,ax
        mov     si,BIOS_EQUIP
        mov     al,_g_savedEquip
        mov     es:[si],al
        mov     ah,VIDEO_SET_MODE
        mov     al,_g_savedVideoMode
        int     BIOS_VIDEO
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_vd_restore  ENDP


; Video adapter detection: saves the current mode and equipment byte,
; asks the EGA BIOS, falls back to probing the CRTC cursor register (VGA/CGA).

        PUBLIC  _vd_detect
_vd_detect  PROC  FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        mov     ax,VIDEO_GET_MODE SHL 8
        int     BIOS_VIDEO
        mov     _g_savedVideoMode,al
        xor     ax,ax
        mov     es,ax
        mov     si,BIOS_EQUIP
        mov     al,es:[si]
        mov     _g_savedEquip,al
        mov     ah,VIDEO_EGA_INFO
        mov     bl,EGA_GET_INFO
        int     BIOS_VIDEO
        mov     _g_egaMono,bh
        mov     _g_egaSwitches,cl
        cmp     bl,EGA_GET_INFO
        je      probe
        mov     _g_egaCard,1
        mov     _g_egaMemory,bl
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
        mov     _g_egaMemory,bl
        mov     _g_egaCard,1
        jmp     done
probe:
        mov     dx,CRTC_INDEX
        mov     al,CRTC_CURSOR_LO
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
        mov     _g_egaCard,2
        jmp     done
mono:
        mov     _g_forceEga,1
done:
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_vd_detect  ENDP

        END
