; F_24BE2 @ 24BE:0002 (linear 0x24BE2, 201 bytes)
; F_24BE2(width, text, dest, fg, bg): draw a string with the 8x8 font in
; segment 0E9Fh into the 4-bit packed screen buffer (160 bytes per row).  The
; font has 9 bytes per glyph: a width byte, then eight row bits.  Returns the
; x position reached (the caller passes the field width and the start parity).

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_24BE2
_F_24BE2  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    si
        push    di
        mov     ax,0E9Fh
        mov     es,ax
        mov     ax,320
        sub     ax,[bp+6]
        mov     [bp-2],ax
        mov     ax,[bp+6]
        and     ax,1
        mov     [bp-4],ax
nextch:
        mov     bx,[bp+8]
        mov     al,[bx]
        or      al,al
        jne     havech
        jmp     done
havech:
        inc     WORD PTR [bp+8]
        cbw
        sub     ax,20h
        jl      badch
        cmp     ax,5Ah
        jle     goodch
badch:
        xor     ax,ax
goodch:
        mov     bx,ax
        shl     ax,1
        shl     ax,1
        shl     ax,1
        add     bx,ax
        mov     ax,0
        add     bx,ax
        mov     ax,[bp-4]
        and     ax,1
        mov     [bp-6],al
        mov     al,es:[bx]
        xor     ah,ah
        add     [bp-4],ax
        mov     cx,[bp-2]
        cmp     [bp-4],cx
        jl      fits
        sub     [bp-4],ax
        jmp     SHORT done
fits:
        mov     si,[bp+0Ah]
        mov     ch,al
        mov     WORD PTR [bp-8],8
nextrow:
        mov     di,si
        inc     bx
        mov     dh,es:[bx]
        mov     al,[di]
        mov     ah,[bp-6]
        mov     cl,ch
        mov     dl,80h
nextpix:
        test    dh,dl
        je      isbg
        or      al,[bp+0Ch]
        jmp     SHORT havepix
isbg:
        or      al,[bp+0Eh]
havepix:
        xor     ah,1
        jne     lownib
        mov     [di],al
        inc     di
        xor     al,al
        jmp     SHORT pixdone
lownib:
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
pixdone:
        dec     cl
        je      rowdone
        shr     dl,1
        jmp     SHORT nextpix
rowdone:
        mov     [di],al
        dec     WORD PTR [bp-8]
        je      glyphdone
        add     si,0A0h
        jmp     SHORT nextrow
glyphdone:
        mov     ax,di
        sub     ax,si
        add     [bp+0Ah],ax
        jmp     nextch
done:
        mov     ax,[bp-4]
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_24BE2  ENDP

        END
