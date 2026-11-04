        .MODEL MEDIUM
        INCLUDE PC.INC

        .CODE S0E6C_TEXT

; cursblt_tandy @ 0E6C:0002 (linear 0x0E6C2, 191 bytes)
; CGA 320x200 4-colour masked blit with background save: for each row, saves the
; screen words to one buffer, then ANDs with a mask and ORs the sprite data.
; Args: sprite, mask, save buffer (far), screen (far), x, y, width in words, rows.
; Screen rows interleave in four banks of 2000h.

        PUBLIC  _cursblt_tandy
_cursblt_tandy  PROC  FAR
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
        je      bank1a
        add     di,2000h
bank1a:
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
row1:
        mov     cx,[bp+1Ah]
        shr     cx,1
        jae     words1
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
        jcxz    rowend1
words1:
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
        loop    words1
rowend1:
        dec     WORD PTR [bp+1Ch]
        je      done1
        mov     ax,[bp-8]
        add     si,ax
        add     bx,ax
        add     dx,[bp-6]
        inc     WORD PTR [bp+18h]
        test    WORD PTR [bp+18h],3
        je      nextbank1
        add     di,[bp-2]
        jmp     row1
nextbank1:
        add     di,[bp-4]
        jmp     row1
done1:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_cursblt_tandy  ENDP

; cursblt_cga @ 0E6C:00C1 (linear 0x0E781, 180 bytes)
; Same blit as cursblt_tandy with two interleaved banks and 80 byte rows.

        PUBLIC  _cursblt_cga
_cursblt_cga  PROC  FAR
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
        je      bank1b
        add     di,2000h
bank1b:
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
row2:
        mov     cx,[bp+1Ah]
        shr     cx,1
        jae     words2
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
        jcxz    rowend2
words2:
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
        loop    words2
rowend2:
        dec     WORD PTR [bp+1Ch]
        je      done2
        mov     ax,[bp-8]
        add     si,ax
        add     bx,ax
        add     dx,[bp-6]
        inc     WORD PTR [bp+18h]
        test    WORD PTR [bp+18h],1
        je      nextbank2
        add     di,[bp-2]
        jmp     row2
nextbank2:
        add     di,[bp-4]
        jmp     row2
done2:
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_cursblt_cga  ENDP

; cursblt_ega @ 0E6C:0175 (linear 0x0E835, 240 bytes)
; EGA 320x200 16-colour masked blit with background save, planar: the mask is
; applied through the graphics controller (write mode 0, bit mask register 8)
; and the four sprite planes are 600h bytes apart.

        PUBLIC  _cursblt_ega
_cursblt_ega  PROC  FAR
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
        mov     ax,GC_MODE
        mov     dx,GC_INDEX
        out     dx,al
        sub     ax,ax
        inc     dx
        out     dx,al
        mov     ax,GC_BIT_MASK
        dec     dx
        out     dx,al
row3:
        mov     cx,[bp+1Ah]
pixel:
        sub     ax,ax
        mov     dx,GC_DATA
        out     dx,al
        mov     ax,SEQ_ALL_PLANES
        mov     dx,SEQ_DATA
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
        mov     dx,GC_DATA
        out     dx,al
        mov     ax,SEQ_PLANE0
        mov     dx,SEQ_DATA
        out     dx,al
        mov     al,[si]
        mov     es:[di],al
        mov     ax,SEQ_PLANE1
        mov     dx,SEQ_DATA
        out     dx,al
        mov     al,[si+600h]
        mov     es:[di],al
        mov     ax,SEQ_PLANE2
        mov     dx,SEQ_DATA
        out     dx,al
        mov     al,[si+0C00h]
        mov     es:[di],al
        mov     ax,SEQ_PLANE3
        mov     dx,SEQ_DATA
        out     dx,al
        mov     al,[si+1200h]
        stosb
        inc     si
        loop    pixel
        dec     WORD PTR [bp+1Ch]
        je      done3
        mov     ax,[bp-6]
        add     si,ax
        add     bx,ax
        mov     ax,[bp-4]
        add     [bp-0Ah],ax
        add     di,[bp-2]
        jmp     row3
done3:
        mov     ax,SEQ_ALL_PLANES
        mov     dx,SEQ_DATA
        out     dx,al
        mov     ax,GC_MASK_ALL
        mov     dx,GC_DATA
        out     dx,al
        pop     si
        pop     di
        pop     ds
        mov     sp,bp
        pop     bp
        ret
_cursblt_ega  ENDP

        END
