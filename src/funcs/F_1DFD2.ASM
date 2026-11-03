; F_1DFD2 @ 1C11:1EC2 (linear 0x1DFD2, 300 bytes)
; Draw a horizontal line with 2 bits per pixel into the interleaved
; (CGA/Hercules style) screen, clipped to the clip rectangle. On a Hercules
; segment (B000-B400 with g_0042 set) an odd line is also drawn 0x2000 bytes
; up, which is what the flag byte in the code segment records.
; F_1DFD2(x1, x2, y, colour)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_0042:WORD
        EXTRN   _g_787a:WORD
        EXTRN   _g_clip_xmin:WORD
        EXTRN   _g_clip_ymin:WORD
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_clip_xmax:WORD
        EXTRN   _g_5b6a:BYTE
        EXTRN   _g_5b7a:BYTE

        .CODE

        EXTRN   _F_1DFA9:NEAR

dbl     DB      0

        PUBLIC  _F_1DFD2
_F_1DFD2  PROC    FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     BYTE PTR cs:dbl,0
        cmp     _g_0042,0
        je      clip
        cmp     _g_787a,0B000h
        jb      clip
        cmp     _g_787a,0B400h
        ja      clip
        test    WORD PTR [bp+10],1
        je      clip
        mov     BYTE PTR cs:dbl,1
clip:
        mov     ax,[bp+10]
        cmp     _g_clip_ymin,ax
        jg      skip1
        cmp     _g_clip_ymax,ax
        jl      skip1
        mov     ax,[bp+6]
        mov     bx,[bp+8]
        mov     cx,_g_clip_xmin
        mov     dx,_g_clip_xmax
        cmp     cx,bx
        jg      skip1
        cmp     dx,ax
        jl      skip1
        jmp     SHORT inside
skip1:
        jmp     NEAR PTR done
inside:
        cmp     ax,cx
        jg      xok
        mov     [bp+6],cx
xok:
        cmp     bx,dx
        jl      x2ok
        mov     [bp+8],dx
x2ok:
        mov     ax,[bp+6]
        cmp     ax,[bp+8]
        jle     ordered
        xchg    ax,[bp+8]
        mov     [bp+6],ax
ordered:
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        call    _F_1DFA9
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+8]
        and     cl,3
        xor     cl,3
        shl     cl,1
        shl     dl,cl
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        mov     cl,2
        shr     ax,cl
        shr     bx,cl
        mov     cx,ax
        sub     cx,bx
        test    WORD PTR [bp+10],1
        je      evenline
        mov     bx,OFFSET _g_5b6a
        jmp     SHORT gotpat
evenline:
        mov     bx,OFFSET _g_5b7a
gotpat:
        mov     al,[bp+12]
        xlat
        or      dh,dh
        js      middle
        or      cx,cx
        jne     lead
        and     dl,dh
        jmp     SHORT last
lead:
        mov     ah,al
        and     ah,dh
        not     dh
        and     es:[di],dh
        or      es:[di],ah
        cmp     BYTE PTR cs:dbl,0
        je      lead1
        add     di,2000h
        and     es:[di],dh
        or      es:[di],ah
        sub     di,2000h
lead1:
        inc     di
        dec     cx
middle:
        cmp     BYTE PTR cs:dbl,0
        je      single
        push    cx
        push    di
        rep stosb
        pop     di
        pop     cx
        add     di,2000h
        rep stosb
        sub     di,2000h
        jmp     SHORT last
single:
        rep stosb
last:
        and     al,dl
        not     dl
        and     es:[di],dl
        or      es:[di],al
        cmp     BYTE PTR cs:dbl,0
        je      done
        add     di,2000h
        and     es:[di],dl
        or      es:[di],al
        sub     di,2000h
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_1DFD2  ENDP

        END
