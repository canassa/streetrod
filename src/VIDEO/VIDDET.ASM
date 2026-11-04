        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _uc_cga:FAR
        EXTRN   _g_egaCard:WORD
        EXTRN   _g_forceEga:WORD

        PUBLIC  _g_savedVideoMode
_g_savedVideoMode DB      0                        ; DS:5AEE char
        PUBLIC  _g_egaMono
_g_egaMono DB      0                        ; DS:5AEF char
        PUBLIC  _g_savedEquip
_g_savedEquip DB      0                        ; DS:5AF0 char
        PUBLIC  _g_egaMemory
_g_egaMemory DB      0                        ; DS:5AF1 char
        PUBLIC  _g_egaSwitches
_g_egaSwitches DB      0                        ; DS:5AF2 char
        PUBLIC  _g_5af3
_g_5af3 DB      0                        ; DS:5AF3 char
S_5af4  DB      'Tandy', 0               ; DS:5AF4 string
S_5afa  DB      'Tandy', 0               ; DS:5AFA string
S_5b00  DB      'CGA', 0                 ; DS:5B00 string

        .CODE S0000_TEXT

; vd_restore @ 0000:E610 (linear 0x0E610, 31 bytes)
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

; vd_detect @ 0000:E62F (linear 0x0E62F, 146 bytes)
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
