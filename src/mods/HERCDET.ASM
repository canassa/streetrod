        .MODEL MEDIUM
        INCLUDE PC.INC

        .CODE S0000_TEXT

; hercdet_ram_test @ 0000:E56A (linear 0x0E56A, 26 bytes)
; Is there RAM at es:3FFFh that holds a written value? Result in ZF
; (equal: the byte at 7FFFh read back the same after the increment).

        PUBLIC  _hercdet_ram_test
_hercdet_ram_test  PROC  FAR
        mov     bl,es:[7FFFh]
        mov     cl,es:[3FFFh]
        inc     BYTE PTR es:[3FFFh]
        cmp     bl,es:[3FFFh]
        mov     es:[3FFFh],cl
        ret
_hercdet_ram_test  ENDP

; hercdet_detect @ 0000:E584 (linear 0x0E584, 63 bytes)
; Hercules detection: does the status port 3BAh toggle bit 7, and is
; there memory at B000h that survives the config switch.

        PUBLIC  _hercdet_detect
_hercdet_detect  PROC  FAR
        mov     bx,100h
        mov     dx,MONO_STATUS
        in      al,dx
        mov     ah,al
        and     ah,HERC_STATUS_VSYNC
wait1:
        mov     cx,40h
wait2:
        in      al,dx
        and     al,HERC_STATUS_VSYNC
        cmp     al,ah
        loope   wait2
        jne     toggled
        dec     bx
        jne     wait1
        jmp     SHORT none
toggled:
        mov     ax,MONO_SEGMENT
        mov     es,ax
        call    _hercdet_ram_test
        jne     found
        mov     al,HERC_ENABLE_GRAPHICS
        mov     dx,HERC_CONFIG
        out     dx,al
        call    _hercdet_ram_test
        je      none
found:
        mov     al,1
        jmp     SHORT done
none:
        xor     al,al
done:
        or      al,al
        ret
_hercdet_detect  ENDP

; hercdet_bios_mono @ 0000:E5C3 (linear 0x0E5C3, 14 bytes)
; BIOS equipment word: 0x30 in the video bits means monochrome.

        PUBLIC  _hercdet_bios_mono
_hercdet_bios_mono  PROC  FAR
        int     EQUIP_INT
        and     ax,EQUIP_VIDEO_MASK
        cmp     ax,EQUIP_VIDEO_MONO
        je      mono
        mov     ax,0
mono:
        ret
_hercdet_bios_mono  ENDP

        END
