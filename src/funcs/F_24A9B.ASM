; F_24A9B @ 2487:022B (linear 0x24A9B, 44 bytes)
; Pixel address helper for the nibble packed 320x200 screen, interlaced in
; four banks of 80 bytes per line.  In: ax = y, bx = x.
; Out: bx = offset, es = video segment, cl = 0 or 4 (shift of the pixel in
; its byte), ah = 0Fh (pixel mask).

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_767c:WORD
        EXTRN   _g_787a:WORD

        .CODE

        PUBLIC  _F_24A9B
_F_24A9B  PROC  NEAR
        mov     cl,bl
        xchg    ah,al
        shr     ax,1
        shr     ax,1
        add     bh,al
        xor     al,al
        add     bx,ax
        shr     ax,1
        shr     ax,1
        add     bx,ax
        shr     bx,1
        add     bx,_g_767c
        mov     ax,_g_787a
        mov     es,ax
        mov     ah,1
        and     cl,ah
        xor     cl,ah
        je      even
        mov     cl,4
even:
        mov     ah,0Fh
        ret
_F_24A9B  ENDP

        END
