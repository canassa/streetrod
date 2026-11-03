; F_1E15E @ 1C11:204E (linear 0x1E15E, 16 bytes)
; Map a colour in al to a CGA 2-bit pixel value: 0-3 stay as they are,
; 4-15 go through the table g_5b5a, anything above becomes 3.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_5b5a:BYTE

        .CODE

        PUBLIC  _F_1E15E
_F_1E15E  PROC    NEAR
        cmp     al,3
        jbe     done
        cmp     al,0Fh
        jbe     mapped
        mov     al,3
done:
        ret
mapped:
        mov     bx,OFFSET _g_5b5a
        xlat
        ret
_F_1E15E  ENDP

        END
