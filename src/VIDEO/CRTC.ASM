        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _g_vga:WORD

        .CODE S21A0_TEXT


; Wait for the start of vertical retrace on the CRT status port 3DAh:
; first until the display is not in the middle of a retrace, then until bit 3 sets.

        PUBLIC  _ct_wait_vsync
_ct_wait_vsync  PROC    FAR
        mov     dx,INPUT_STATUS
wait1a:
        in      al,dx
        test    al,STATUS_DISPLAY_DISABLED
        jne     wait1a
wait2a:
        in      al,dx
        test    al,STATUS_VRETRACE
        je      wait2a
        ret
_ct_wait_vsync  ENDP


; Set the CRTC line compare register (split screen row) during vertical
; retrace. The CRTC base port comes from the BIOS data area (0040:0063).
; A line value of -1 means the bottom of the screen (1FFh, or 3FFh when
; g_vga is set). With g_vga clear only the plain VGA bits are used; otherwise
; the extra overflow bits live in registers 7 and 9 and the value is biased.
; ct_set_split(line)

        PUBLIC  _ct_set_split
_ct_set_split  PROC    FAR
        push    bp
        mov     bp,sp
        mov     ax,BIOS_DATA_SEGMENT
        mov     es,ax
        mov     dx,es:[BIOS_CRTC_BASE]
        add     dl,CRTC_STATUS_DELTA
wait1b:
        in      al,dx
        test    al,STATUS_VRETRACE
        jne     wait1b
wait2b:
        in      al,dx
        test    al,STATUS_VRETRACE
        je      wait2b
        sub     dl,CRTC_STATUS_DELTA
        mov     ax,[bp+6]
        cmp     ax,0FFFFh
        jne     have
        cmp     _g_vga,0
        je      short1ff
        mov     ax,VGA_LINE_COMPARE_MAX
        jmp     SHORT have
short1ff:
        mov     ax,EGA_LINE_COMPARE_MAX
have:
        nop
        cmp     _g_vga,0
        jne     wide
        mov     bh,ah
        and     bh,1
        mov     cl,4
        shl     bh,cl
        cli
        mov     ah,al
        mov     al,CRTC_LINE_COMPARE
        out     dx,ax
        mov     ah,11h
        and     ah,(0FFh XOR CRTC_LINE_COMPARE_BIT8)
        or      ah,bh
        mov     al,CRTC_OVERFLOW
        out     dx,ax
        sti
        pop     bp
        ret
wide:
        cmp     ax,SCREEN_HEIGHT
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
        mov     al,CRTC_LINE_COMPARE
        out     dx,ax
        mov     al,CRTC_OVERFLOW
        out     dx,al
        inc     dx
        in      al,dx
        dec     dx
        mov     ah,al
        and     ah,(0FFh XOR CRTC_LINE_COMPARE_BIT8)
        or      ah,bl
        mov     al,CRTC_OVERFLOW
        out     dx,ax
        mov     al,CRTC_MAX_SCAN_LINE
        out     dx,al
        inc     dx
        in      al,dx
        dec     dx
        mov     ah,al
        and     ah,(0FFh XOR CRTC_LINE_COMPARE_BIT9)
        or      ah,bh
        mov     al,CRTC_MAX_SCAN_LINE
        out     dx,ax
        sti
        pop     bp
        ret
_ct_set_split  ENDP

        END
