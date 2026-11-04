        .MODEL MEDIUM

; F_0E6C2 @ 0e6c:0002 (linear 0x0E6C2, 191 bytes)
; CGA 320x200 4-colour masked blit with background save: for each L0E6C2_row, saves the
; screen L0E6C2_words to one buffer, then ANDs with a mask and ORs the sprite data.
; Args: sprite, mask, save buffer (far), screen (far), x, y, width in L0E6C2_words, rows.
; Screen rows interleave in four banks of 2000h.

        .CODE S0E6C_TEXT

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
        je      L0E6C2_bank1
        add     di,2000h
L0E6C2_bank1:
        test    bh,2
        je      L0E6C2_bank2
        add     di,4000h
L0E6C2_bank2:
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
L0E6C2_row:
        mov     cx,[bp+1Ah]
        shr     cx,1
        jae     L0E6C2_words
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
        jcxz    L0E6C2_rowend
L0E6C2_words:
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
        loop    L0E6C2_words
L0E6C2_rowend:
        dec     WORD PTR [bp+1Ch]
        je      L0E6C2_done
        mov     ax,[bp-8]
        add     si,ax
        add     bx,ax
        add     dx,[bp-6]
        inc     WORD PTR [bp+18h]
        test    WORD PTR [bp+18h],3
        je      L0E6C2_nextbank
        add     di,[bp-2]
        jmp     L0E6C2_row
L0E6C2_nextbank:
        add     di,[bp-4]
        jmp     L0E6C2_row
L0E6C2_done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_0E6C2  ENDP

; F_0E781 @ 0e6c:00c1 (linear 0x0E781, 180 bytes)
; Same blit as F_0E6C2 with two interleaved banks and 80 byte rows.

        .CODE S0E6C_TEXT

        PUBLIC  _F_0E781
_F_0E781  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,8
        push    ds
        push    di
        push    si
        les     di,DWORD PTR [bp+12h]
        mov     ax,50h
        mov     bx,[bp+18h]
        mov     bh,bl
        shr     bl,1
        mul     bl
        add     di,ax
        add     di,[bp+16h]
        test    bh,1
        je      L0E781_bank1
        add     di,2000h
L0E781_bank1:
        mov     ax,18h
        sub     ax,[bp+1Ah]
        mov     [bp-8],ax
        mov     ax,6
        sub     ax,[bp+1Ah]
        mov     [bp-6],ax
        mov     ax,2000h
        sub     ax,[bp+1Ah]
        mov     [bp-2],ax
        mov     ax,50h
        sub     ax,2000h
        sub     ax,[bp+1Ah]
        mov     [bp-4],ax
        lds     si,DWORD PTR [bp+0Eh]
        mov     dx,si
        lds     si,DWORD PTR [bp+0Ah]
        mov     bx,si
        lds     si,DWORD PTR [bp+6]
L0E781_row:
        mov     cx,[bp+1Ah]
        shr     cx,1
        jae     L0E781_words
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
        jcxz    L0E781_rowend
L0E781_words:
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
        loop    L0E781_words
L0E781_rowend:
        dec     WORD PTR [bp+1Ch]
        je      L0E781_done
        mov     ax,[bp-8]
        add     si,ax
        add     bx,ax
        add     dx,[bp-6]
        inc     WORD PTR [bp+18h]
        test    WORD PTR [bp+18h],1
        je      L0E781_nextbank
        add     di,[bp-2]
        jmp     L0E781_row
L0E781_nextbank:
        add     di,[bp-4]
        jmp     L0E781_row
L0E781_done:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_0E781  ENDP

; F_0E835 @ 0e6c:0175 (linear 0x0E835, 240 bytes)
; EGA 320x200 16-colour masked blit with background save, planar: the mask is
; applied through the graphics controller (write mode 0, bit mask register 8)
; and the four sprite planes are 600h bytes apart.

        .CODE S0E6C_TEXT

        PUBLIC  _F_0E835
_F_0E835  PROC  FAR
        push    bp
        mov     bp,sp
        sub     sp,0Ch
        push    ds
        push    di
        push    si
        les     di,DWORD PTR [bp+12h]
        mov     ax,28h
        mov     bx,[bp+18h]
        mul     bl
        add     di,ax
        add     di,[bp+16h]
        mov     ax,18h
        sub     ax,[bp+1Ah]
        mov     [bp-6],ax
        mov     ax,3
        sub     ax,[bp+1Ah]
        mov     [bp-4],ax
        mov     ax,28h
        sub     ax,[bp+1Ah]
        mov     [bp-2],ax
        lds     si,DWORD PTR [bp+0Eh]
        mov     [bp-0Ah],si
        mov     ax,ds
        mov     [bp-8],ax
        lds     si,DWORD PTR [bp+0Ah]
        mov     bx,si
        lds     si,DWORD PTR [bp+6]
        mov     ax,ds
        mov     [bp-0Ch],ax
        mov     ax,5
        mov     dx,3CEh
        out     dx,al
        sub     ax,ax
        inc     dx
        out     dx,al
        mov     ax,8
        dec     dx
        out     dx,al
L0E835_row:
        mov     cx,[bp+1Ah]
L0E835_pixel:
        sub     ax,ax
        mov     dx,3CFh
        out     dx,al
        mov     ax,0Fh
        mov     dx,3C5h
        out     dx,al
        xchg    bx,[bp-0Ah]
        mov     ax,[bp-8]
        mov     ds,ax
        mov     al,es:[di]
        mov     [bx],al
        xchg    bx,[bp-0Ah]
        inc     WORD PTR [bp-0Ah]
        mov     ax,[bp-0Ch]
        mov     ds,ax
        mov     al,[bx]
        inc     bx
        sub     ah,ah
        mov     dx,3CFh
        out     dx,al
        mov     ax,1
        mov     dx,3C5h
        out     dx,al
        mov     al,[si]
        mov     es:[di],al
        mov     ax,2
        mov     dx,3C5h
        out     dx,al
        mov     al,[si+600h]
        mov     es:[di],al
        mov     ax,4
        mov     dx,3C5h
        out     dx,al
        mov     al,[si+0C00h]
        mov     es:[di],al
        mov     ax,8
        mov     dx,3C5h
        out     dx,al
        mov     al,[si+1200h]
        stosb
        inc     si
        loop    L0E835_pixel
        dec     WORD PTR [bp+1Ch]
        je      L0E835_done
        mov     ax,[bp-6]
        add     si,ax
        add     bx,ax
        mov     ax,[bp-4]
        add     [bp-0Ah],ax
        add     di,[bp-2]
        jmp     L0E835_row
L0E835_done:
        mov     ax,0Fh
        mov     dx,3C5h
        out     dx,al
        mov     ax,0FFh
        mov     dx,3CFh
        out     dx,al
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_F_0E835  ENDP

        END
