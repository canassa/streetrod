        .MODEL MEDIUM
        INCLUDE PC.INC

        .DATA
        EXTRN   _g_joyX:WORD
        EXTRN   _g_joyButtons:BYTE
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
        EXTRN   _g_demoKey:BYTE
        EXTRN   _g_soundOn:BYTE
        EXTRN   _g_lastKey:BYTE
        EXTRN   _g_dirHeld:WORD
        EXTRN   _g_directionSteps:BYTE
        EXTRN   _g_joySavedSp:WORD

        .CODE S0000_TEXT

        PUBLIC  _jy_read
_jy_read  PROC  FAR
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
        mov     _g_joySavedSp,sp
        cli
        mov     dx,GAME_PORT
        mov     cx,400h
        mov     bx,GAME_JOY1_AXES
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
        mov     di,OFFSET _g_joyX
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
        mov     dx,GAME_PORT
        in      al,dx
        not     al
        mov     cl,4
        shr     al,cl
        and     al,3
        mov     _g_joyButtons,al
        je      nobtn
        cmp     _g_buttons,0
        jne     held
        mov     _g_click,al
        mov     ax,_g_cursorX
        mov     _g_clickX,ax
        mov     ax,_g_cursorY
        mov     _g_clickY,ax
held:
        or      _g_buttons,2
        jmp     SHORT done1
nobtn:
        and     _g_buttons,0FDh
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
        mov     sp,_g_joySavedSp
        jmp     done1
_jy_read  ENDP

; keyboard scan code handler, variant of the int 9 ISR kb_isr (retf, no port I/O)

        PUBLIC  _jy_demo_key
_jy_demo_key  PROC  FAR
        push    ax
        push    bx
        push    ds
        push    es
        mov     al,_g_demoKey
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
        cmp     BYTE PTR _g_dirHeld[bx+16],0
        jne     done2
        mov     BYTE PTR _g_dirHeld[bx+16],1
        mov     _g_inputLock,0FFFFh
        mov     al,_g_directionSteps[bx+16]
        add     _g_keyDx,al
        mov     al,_g_directionSteps[bx+17]
        add     _g_keyDy,al
        mov     ax,WORD PTR _g_directionSteps[bx+16]
        mov     WORD PTR _g_lastDx,ax     ; g_lastDx and g_lastDy
        mov     _g_inputLock,0
        jmp     SHORT done2
L6:
        cmp     BYTE PTR _g_dirHeld[bx+16],0
        je      done2
        mov     _g_inputLock,0FFFFh
        mov     BYTE PTR _g_dirHeld[bx+16],0
        mov     al,_g_directionSteps[bx+16]
        sub     _g_keyDx,al
        mov     al,_g_directionSteps[bx+17]
        sub     _g_keyDy,al
        mov     _g_inputLock,0
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
        xor     _g_soundOn,1
        jmp     SHORT done2
L12:
        mov     _g_lastKey,al
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
        jmp     SHORT done2
L15:
        mov     _g_inputLock,0FFFFh
        and     _g_buttons,0FEh
        mov     _g_inputLock,0
        jmp     SHORT done2
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
        jmp     SHORT done2
L17:
        jmp     SHORT done2
_jy_demo_key  ENDP

        END
