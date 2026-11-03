; F_247CB @ 2462:01AB (linear 0x247CB, 73 bytes)
; Near helper of F_2467A: expand the 8 bits of al into two words of 4-bit
; pixels, masking the colour word kept in the caller's frame ([bp-122h]), and
; OR them into the destination (es:di).

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_247CB
_F_247CB  PROC  NEAR
        mov     bx,[bp-122h]
        mov     cx,bx
        rol     al,1
        jc      b1
        and     bl,0Fh
b1:
        rol     al,1
        jc      b2
        and     bl,0F0h
b2:
        rol     al,1
        jc      b3
        and     bh,0Fh
b3:
        rol     al,1
        jc      b4
        and     bh,0F0h
b4:
        or      es:[di],bx
        inc     di
        inc     di
        rol     al,1
        jc      b5
        and     cl,0Fh
b5:
        rol     al,1
        jc      b6
        and     cl,0F0h
b6:
        rol     al,1
        jc      b7
        and     ch,0Fh
b7:
        rol     al,1
        jc      b8
        and     ch,0F0h
b8:
        or      es:[di],cx
        inc     di
        inc     di
        ret
_F_247CB  ENDP

        END
