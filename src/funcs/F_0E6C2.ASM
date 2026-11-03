; F_0E6C2 @ 0e6c:0002 (linear 0x0E6C2, 191 bytes)
; CGA 320x200 4-colour masked blit with background save: for each row, saves the
; screen words to one buffer, then ANDs with a mask and ORs the sprite data.
; Args: sprite, mask, save buffer (far), screen (far), x, y, width in words, rows.
; Screen rows interleave in four banks of 2000h.

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_0E6C2
_F_0E6C2  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    ds
        push    di
        push    si
        les     di,DWORD PTR [bp+12h]
        mov     ax,0A0h
        mov     bx,[bp+18h]
        mov     bh,bl
        shr     bl,1
        shr     bl,1
        mul     bl
        add     di,ax
        add     di,[bp+16h]
        test    bh,1
        je      bank1
        add     di,2000h
bank1:
        test    bh,2
        je      bank2
        add     di,4000h
bank2:
        mov     ax,18h
        sub     ax,[bp+1Ah]
        mov     [bp-8],ax
        mov     ax,0Ch
        sub     ax,[bp+1Ah]
        mov     [bp-6],ax
        mov     ax,2000h
        sub     ax,[bp+1Ah]
        mov     [bp-2],ax
        mov     ax,0A0h
        sub     ax,6000h
        sub     ax,[bp+1Ah]
        mov     [bp-4],ax
        lds     si,DWORD PTR [bp+0Eh]
        mov     dx,si
        lds     si,DWORD PTR [bp+0Ah]
        mov     bx,si
        lds     si,DWORD PTR [bp+6]
row:
        mov     cx,[bp+1Ah]
        shr     cx,1
        jae     words
        xchg    dx,bx
        mov     al,es:[di]
        mov     [bx],al
        xchg    dx,bx
        inc     dx
        and     al,[bx]
        inc     bx
        or      al,[si]
        inc     si
        stosb
        jcxz    rowend
words:
        xchg    dx,bx
        mov     ax,es:[di]
        mov     [bx],ax
        xchg    dx,bx
        inc     dx
        inc     dx
        and     ax,[bx]
        inc     bx
        inc     bx
        or      ax,[si]
        inc     si
        inc     si
        stosw
        loop    words
rowend:
        dec     WORD PTR [bp+1Ch]
        je      done
        mov     ax,[bp-8]
        add     si,ax
        add     bx,ax
        add     dx,[bp-6]
        inc     WORD PTR [bp+18h]
        test    WORD PTR [bp+18h],3
        je      nextbank
        add     di,[bp-2]
        jmp     row
nextbank:
        add     di,[bp-4]
        jmp     row
done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_0E6C2  ENDP

        END
