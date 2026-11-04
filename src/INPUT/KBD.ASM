        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _g_demo:WORD
        EXTRN   _g_cursorX:WORD
        EXTRN   _g_cursorY:WORD
        EXTRN   _g_keyDx:BYTE
        EXTRN   _g_keyDy:BYTE
        EXTRN   _g_lastDx:BYTE
        EXTRN   _g_buttons:BYTE
        EXTRN   _g_click:BYTE
        EXTRN   _g_click2:BYTE
        EXTRN   _g_clickX:WORD
        EXTRN   _g_clickY:WORD
        EXTRN   _g_click2X:WORD
        EXTRN   _g_click2Y:WORD
        EXTRN   _g_inputLock:WORD
        EXTRN   _g_scanMap:BYTE
        EXTRN   _g_fireLatch:BYTE
        EXTRN   _g_soundOn:BYTE
        EXTRN   _g_lastKey:BYTE
        EXTRN   _g_joySavedSp:WORD
        EXTRN   _g_firePressed:WORD

        .CODE S0000_TEXT

; kb_text_on @ 0000:2C04 (linear 0x02C04, 39 bytes)

        PUBLIC  _kb_text_on
_kb_text_on  PROC  FAR
        push    bp
        mov     bp,sp
        cli
        test    WORD PTR [bp+6],0FFFFh
        jne     skip57
        mov     _g_scanMap+SC_SPACE,20h
skip57:
        mov     _g_scanMap+SC_BACKSPACE,8
        mov     _g_scanMap+SC_COMMA,2Ch
        mov     _g_scanMap+SC_PERIOD,2Eh
        and     _g_buttons,0FEh
        sti
        pop     bp
        ret
_kb_text_on  ENDP

; kb_text_off @ 0000:2C2B (linear 0x02C2B, 23 bytes)

        PUBLIC  _kb_text_off
_kb_text_off  PROC  FAR
        cli
        mov     _g_scanMap+SC_BACKSPACE,0
        mov     _g_scanMap+SC_COMMA,0
        mov     _g_scanMap+SC_PERIOD,0
        mov     _g_scanMap+SC_SPACE,1Fh
        sti
        ret
_kb_text_off  ENDP

; kb_isr @ 0000:2C42 (linear 0x02C42, 366 bytes)
; int 9 keyboard ISR (installed by kg_hook)

        PUBLIC  _kb_isr
_kb_isr  PROC  FAR
        push    ax
        push    bx
        push    ds
        push    es
        in      al,PPI_A
        xor     ah,ah
        push    ax
        in      al,PPI_B
        mov     ah,al
        or      al,PPI_KEYBOARD_ACK
        out     PPI_B,al
        mov     al,ah
        out     PPI_B,al
        mov     ax,DGROUP
        mov     ds,ax
        mov     es,ax
        pop     ax
        xor     ah,ah
        cmp     _g_demo,0
        je      L1
        cmp     al,SC_CTRL
        je      L1
        cmp     al,SC_CTRL + 80h
        je      L1
        cmp     al,SC_M
        je      L1
        cmp     al,SC_ESC
        jne     L0
        mov     _g_demo,63h
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
        mov     al,_g_scanMap[bx]
        cmp     al,0
        je      L9
        jg      L10
        cmp     al,0E2h
        jl      L10
        cbw
        mov     bx,ax
        popf
        jne     L6
        cmp     BYTE PTR _g_joySavedSp[bx],0
        jne     done
        mov     BYTE PTR _g_joySavedSp[bx],1
        mov     _g_inputLock,0FFFFh
        mov     al,_g_scanMap[bx]
        add     _g_keyDx,al
        mov     al,_g_scanMap[bx+1]
        add     _g_keyDy,al
        mov     ax,WORD PTR _g_scanMap[bx]
        mov     WORD PTR _g_lastDx,ax     ; g_lastDx and g_lastDy
        mov     _g_inputLock,0
        jmp     SHORT done
L6:
        cmp     BYTE PTR _g_joySavedSp[bx],0
        je      done
        mov     _g_inputLock,0FFFFh
        mov     BYTE PTR _g_joySavedSp[bx],0
        mov     al,_g_scanMap[bx]
        sub     _g_keyDx,al
        mov     al,_g_scanMap[bx+1]
        sub     _g_keyDy,al
        mov     _g_inputLock,0
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
        xor     _g_soundOn,1
        jmp     SHORT done
L12:
        mov     _g_lastKey,al
        jmp     SHORT done
L11:
        cmp     al,1Fh
        je      L15
        cmp     al,1Eh
        je      L17
done:
        mov     al,PIC_EOI
        out     PIC_CMD,al
        pop     es
        pop     ds
        pop     bx
        pop     ax
        iret
L14:
        cmp     _g_buttons,0
        jne     L14b
        mov     _g_inputLock,0FFFFh
        mov     _g_click,1Fh
        mov     ax,_g_cursorX
        mov     _g_clickX,ax
        mov     ax,_g_cursorY
        mov     _g_clickY,ax
        mov     _g_inputLock,0
L14b:
        or      _g_buttons,1
        mov     _g_fireLatch,1
        mov     _g_firePressed,1
        jmp     SHORT done
L15:
        mov     _g_inputLock,0FFFFh
        and     _g_buttons,0FEh
        mov     _g_inputLock,0
        jmp     SHORT done
L16:
        cmp     _g_buttons,0
        jne     L16b
        mov     _g_inputLock,0FFFFh
        mov     _g_click2,1Eh
        mov     ax,_g_cursorX
        mov     _g_click2X,ax
        mov     ax,_g_cursorY
        mov     _g_click2Y,ax
        mov     _g_inputLock,0
L16b:
        jmp     SHORT done
L17:
        jmp     SHORT done
_kb_isr  ENDP

        END
