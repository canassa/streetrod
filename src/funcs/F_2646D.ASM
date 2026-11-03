; F_2646D @ 2645:001D (linear 0x2646D, 23 bytes)
; Restore the graphics controller defaults: set/reset 0, enable set/reset 0,
; function select 0, bit mask FFh.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_vid:WORD

        .CODE

        PUBLIC  _F_2646D
_F_2646D  PROC    FAR
        cmp     _g_vid+12,-2
        jne     done
        mov     dx,3CEh
        xor     ax,ax
        out     dx,ax
        inc     ax
        out     dx,ax
        mov     al,3
        out     dx,ax
        mov     ax,0FF08h
        out     dx,ax
done:
        ret
_F_2646D  ENDP

        END
