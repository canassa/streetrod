; F_0E835 @ 0e6c:0175 (linear 0x0E835, 240 bytes)
; EGA 320x200 16-colour masked blit with background save, planar: the mask is
; applied through the graphics controller (write mode 0, bit mask register 8)
; and the four sprite planes are 600h bytes apart.

        .MODEL  MEDIUM
        .CODE

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
row:
        mov     cx,[bp+1Ah]
pixel:
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
        loop    pixel
        dec     WORD PTR [bp+1Ch]
        je      done
        mov     ax,[bp-6]
        add     si,ax
        add     bx,ax
        mov     ax,[bp-4]
        add     [bp-0Ah],ax
        add     di,[bp-2]
        jmp     row
done:
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
