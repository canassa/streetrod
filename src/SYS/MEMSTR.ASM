        .MODEL MEDIUM

        .CODE S2460_TEXT

; ms_find @ 2460:0002 (linear 0x24602, 43 bytes)
; Search len bytes of a far buffer for a NUL-terminated near string:
; ms_find(needle, haystack_far, len). Returns the number of bytes left
; (cx) when the match ended, 0 if not found.

        PUBLIC  _ms_find
_ms_find  PROC  FAR
        push    bp
        mov     bp,sp
        push    di
        push    si
        mov     cx,[bp+12]
        mov     bx,[bp+6]
        mov     dx,bx
        inc     dx
next:
        mov     al,[bx]
        les     di,DWORD PTR [bp+8]
        repne   scasb
        jcxz    done
        mov     si,dx
        repe    cmpsb
        jcxz    done
        dec     si
        cmp     BYTE PTR [si],0
        jne     next
done:
        mov     ax,cx
        pop     si
        pop     di
        mov     sp,bp
        pop     bp
        ret
_ms_find  ENDP

        END
