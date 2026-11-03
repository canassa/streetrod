; F_26458 @ 2645:0008 (linear 0x26458, 21 bytes)
; EGA/VGA: enable set/reset on all four planes (GC index 1 = 0Fh), then
; program GC index 3 (data rotate / function select) from the high byte of g_5c1e.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_vid:WORD
        EXTRN   _g_5c1e:WORD

        .CODE

        PUBLIC  _F_26458
_F_26458  PROC    FAR
        cmp     _g_vid+12,-2
        jne     done
        mov     dx,3CEh
        mov     ax,0F01h
        out     dx,ax
        mov     ax,_g_5c1e
        mov     al,3
        out     dx,ax
done:
        ret
_F_26458  ENDP

        END
