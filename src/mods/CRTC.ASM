        .MODEL MEDIUM

        .DATA
        EXTRN   _g_0254:WORD

        .CODE S21A0_TEXT

; crtc_wait_vsync @ 21A0:0006 (linear 0x21A06, 14 bytes)
; Wait for the start of vertical retrace on the CRT status port 3DAh:
; first until the display is not in the middle of a retrace, then until bit 3 sets.

        PUBLIC  _crtc_wait_vsync
_crtc_wait_vsync  PROC    FAR
        mov     dx,3DAh
wait1a:
        in      al,dx
        test    al,1
        jne     wait1a
wait2a:
        in      al,dx
        test    al,8
        je      wait2a
        ret
_crtc_wait_vsync  ENDP

; crtc_set_split @ 21A0:0014 (linear 0x21A14, 151 bytes)
; Set the CRTC line compare register (split screen row) during vertical
; retrace. The CRTC base port comes from the BIOS data area (0040:0063).
; A line value of -1 means the bottom of the screen (1FFh, or 3FFh when
; g_0254 is set). With g_0254 clear only the plain VGA bits are used; otherwise
; the extra overflow bits live in registers 7 and 9 and the value is biased.
; crtc_set_split(line)

        PUBLIC  _crtc_set_split
_crtc_set_split  PROC    FAR
        push    bp
        mov     bp,sp
        mov     ax,40h
        mov     es,ax
        mov     dx,es:[63h]
        add     dl,6
wait1b:
        in      al,dx
        test    al,8
        jne     wait1b
wait2b:
        in      al,dx
        test    al,8
        je      wait2b
        sub     dl,6
        mov     ax,[bp+6]
        cmp     ax,0FFFFh
        jne     have
        cmp     _g_0254,0
        je      short1ff
        mov     ax,3FFh
        jmp     SHORT have
short1ff:
        mov     ax,1FFh
have:
        nop
        cmp     _g_0254,0
        jne     wide
        mov     bh,ah
        and     bh,1
        mov     cl,4
        shl     bh,cl
        cli
        mov     ah,al
        mov     al,18h
        out     dx,ax
        mov     ah,11h
        and     ah,0EFh
        or      ah,bh
        mov     al,7
        out     dx,ax
        sti
        pop     bp
        ret
wide:
        cmp     ax,0C8h
        jg      biased
        add     ax,64h
biased:
        mov     bh,ah
        mov     bl,bh
        and     bx,201h
        mov     cl,4
        shl     bx,cl
        shl     bh,1
        cli
        mov     ah,al
        mov     al,18h
        out     dx,ax
        mov     al,7
        out     dx,al
        inc     dx
        in      al,dx
        dec     dx
        mov     ah,al
        and     ah,0EFh
        or      ah,bl
        mov     al,7
        out     dx,ax
        mov     al,9
        out     dx,al
        inc     dx
        in      al,dx
        dec     dx
        mov     ah,al
        and     ah,0BFh
        or      ah,bh
        mov     al,9
        out     dx,ax
        sti
        pop     bp
        ret
_crtc_set_split  ENDP

        END
