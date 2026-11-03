; F_2462E @ 2462:000E (linear 0x2462E, 76 bytes)
; Near helper shared by F_2467A and F_24740, which keep its tables in their own
; frame: [bp-110h] 16 bytes (copy of g_4eee), [bp-120h] 16 bytes (copy of
; g_4efe), [bp-100h] 256 bytes mapping a colour byte of the palette passed at
; [bp+10h] to its index+1.  Then clears the destination buffer ([bp+0Ah] far,
; [bp+0Eh] bytes).

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_4eee:BYTE
        EXTRN   _g_4efe:BYTE

        .CODE

        PUBLIC  _F_2462E
_F_2462E  PROC  NEAR
        push    ss
        pop     es
        mov     cx,8
        lea     di,[bp-110h]
        mov     si,OFFSET _g_4eee
        rep     movsw
        mov     cx,8
        lea     di,[bp-120h]
        mov     si,OFFSET _g_4efe
        rep     movsw
        sub     ax,ax
        mov     cx,80h
        lea     di,[bp-100h]
        rep     stosw
        mov     si,[bp+10h]
        mov     dx,16
        xor     ax,ax
index_loop:
        mov     bx,ax
        mov     di,[bx+si]
        and     di,0FFh
        inc     al
        mov     [bp+di-100h],al
        cmp     ax,dx
        jl      index_loop
        les     di,DWORD PTR [bp+0ah]
        mov     cx,[bp+0eh]
        shr     cx,1
        xor     ax,ax
        rep     stosw
        ret
_F_2462E  ENDP

        END
