; F_1DD22 @ 1C11:1C12 (linear 0x1DD22, 647 bytes)
; Draw a line with 2 bits per pixel into the interleaved (CGA/Hercules
; style) screen, Bresenham style. A horizontal line is a byte-fill with edge
; masks, a vertical one a per-line loop, anything else steps along its major
; axis (xmajor / ymajor loops, picked through the pointer kept at [bp-8]).
; Local words: [bp-2] = alternate row step, [bp-4] = 2*minor, [bp-6] = error
; decrement, [bp-8] = loop to enter, [bp-0Ah] = Hercules double-write state
; (0 = off, 2/3 = on with the parity of the current row).
; F_1DD22(x1, y1, x2, y2, colour)

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_0042:WORD
        EXTRN   _g_787a:WORD
        EXTRN   _g_5b56:BYTE

        .CODE

        EXTRN   _F_1DFA9:NEAR
        EXTRN   _F_1E15E:NEAR

        PUBLIC  _F_1DD22
_F_1DD22  PROC    FAR
        push    bp
        mov     bp,sp
        sub     sp,0Ah
        push    si
        push    di
        mov     al,[bp+14]
        call    _F_1E15E
        mov     [bp+14],al
        mov     si,2000h
        mov     di,0E050h
        mov     WORD PTR [bp-0Ah],0
        cmp     _g_0042,0
        je      gotdx
        cmp     _g_787a,0B000h
        jb      gotdx
        cmp     _g_787a,0B400h
        ja      gotdx
        mov     WORD PTR [bp-0Ah],2
        mov     ax,[bp+8]
        and     ax,1
        add     [bp-0Ah],ax
gotdx:
        mov     cx,[bp+10]
        sub     cx,[bp+6]
        je      vert
        jns     xordered
        neg     cx
        mov     bx,[bp+10]
        xchg    bx,[bp+6]
        mov     [bp+10],bx
        mov     bx,[bp+12]
        xchg    bx,[bp+8]
        mov     [bp+12],bx
        cmp     WORD PTR [bp-0Ah],0
        je      xordered
        mov     ax,[bp+8]
        and     ax,1
        or      ax,2
        mov     [bp-0Ah],ax
xordered:
        mov     bx,[bp+12]
        sub     bx,[bp+8]
        jne     notflat
        jmp     horiz
notflat:
        jns     ydown
        neg     bx
        neg     si
        neg     di
        xchg    si,di
ydown:
        mov     [bp-2],di
        mov     WORD PTR [bp-8],OFFSET xmajor
        cmp     bx,cx
        jle     minor
        mov     WORD PTR [bp-8],OFFSET ymajor
        xchg    bx,cx
minor:
        shl     bx,1
        mov     [bp-4],bx
        sub     bx,cx
        mov     di,bx
        sub     bx,cx
        mov     [bp-6],bx
        push    cx
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _F_1DFA9
        mov     al,[bp+14]
        shl     ax,cl
        mov     dx,ax
        not     dh
        pop     cx
        inc     cx
        test    bx,2000h
        je      enter
        xchg    si,[bp-2]
enter:
        jmp     WORD PTR [bp-8]
vert:
        mov     ax,[bp+8]
        mov     bx,[bp+12]
        mov     cx,bx
        sub     cx,ax
        jge     vlen
        neg     cx
        mov     ax,bx
vlen:
        inc     cx
        mov     bx,[bp+6]
        push    cx
        call    _F_1DFA9
        mov     al,[bp+14]
        shl     ax,cl
        not     ah
        pop     cx
        test    bx,si
        je      vloop
        xchg    si,di
vloop:
        and     es:[bx],ah
        or      es:[bx],al
        cmp     WORD PTR [bp-0Ah],0
        je      vnext
        test    WORD PTR [bp-0Ah],1
        je      vflip
        add     bx,2000h
        and     es:[bx],ah
        or      es:[bx],al
        sub     bx,2000h
vflip:
        xor     WORD PTR [bp-0Ah],1
vnext:
        add     bx,si
        xchg    si,di
        loop    vloop
        jmp     done
horiz:
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        call    _F_1DFA9
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+10]
        and     cl,3
        xor     cl,3
        shl     cl,1
        shl     dl,cl
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        mov     cl,2
        shr     ax,cl
        shr     bx,cl
        mov     cx,ax
        sub     cx,bx
        mov     bx,OFFSET _g_5b56
        mov     al,[bp+14]
        xlat
        or      dh,dh
        js      hmiddle
        or      cx,cx
        jne     hlead
        and     dl,dh
        jmp     SHORT hlast
hlead:
        mov     ah,al
        and     ah,dh
        not     dh
        and     es:[di],dh
        or      es:[di],ah
        cmp     WORD PTR [bp-0Ah],0
        je      hlead1
        test    WORD PTR [bp-0Ah],1
        je      hlead1
        add     di,2000h
        and     es:[di],dh
        or      es:[di],ah
        sub     di,2000h
hlead1:
        inc     di
        dec     cx
hmiddle:
        cmp     WORD PTR [bp-0Ah],0
        je      hsingle
        test    WORD PTR [bp-0Ah],1
        je      hsingle
        push    cx
        push    di
        rep stosb
        pop     di
        pop     cx
        add     di,2000h
        rep stosb
        sub     di,2000h
        jmp     SHORT hlast
hsingle:
        rep stosb
hlast:
        and     al,dl
        not     dl
        and     es:[di],dl
        or      es:[di],al
        cmp     WORD PTR [bp-0Ah],0
        je      hend
        test    WORD PTR [bp-0Ah],1
        je      hend
        add     di,2000h
        and     es:[di],dl
        or      es:[di],al
        sub     di,2000h
hend:
        jmp     done
xmajor:
        mov     ah,es:[bx]
xand:
        and     ah,dh
        or      ah,dl
        ror     dl,1
        ror     dl,1
        ror     dh,1
        ror     dh,1
        jae     xnext
        or      di,di
        jns     xdiag
        add     di,[bp-4]
        loop    xand
        mov     es:[bx],ah
        jmp     done
xdiag:
        add     di,[bp-6]
        mov     es:[bx],ah
        cmp     WORD PTR [bp-0Ah],0
        je      xrow
        test    WORD PTR [bp-0Ah],1
        je      xflip
        add     bx,2000h
        mov     es:[bx],ah
        sub     bx,2000h
xflip:
        xor     WORD PTR [bp-0Ah],1
xrow:
        add     bx,si
        xchg    si,[bp-2]
        loop    xmajor
        jmp     SHORT done
xnext:
        mov     es:[bx],ah
        inc     bx
        or      di,di
        jns     xnext2
        add     di,[bp-4]
        dec     cx
        je      done
        jmp     xmajor
xnext2:
        add     di,[bp-6]
        add     bx,si
        xchg    si,[bp-2]
        dec     cx
        je      done
        jmp     xmajor
ymajor:
        and     es:[bx],dh
        or      es:[bx],dl
        cmp     WORD PTR [bp-0Ah],0
        je      yrow
        test    WORD PTR [bp-0Ah],1
        je      yflip
        add     bx,2000h
        and     es:[bx],dh
        or      es:[bx],dl
        sub     bx,2000h
yflip:
        xor     WORD PTR [bp-0Ah],1
yrow:
        add     bx,si
        xchg    si,[bp-2]
        or      di,di
        jns     ydiag
        add     di,[bp-4]
        loop    ymajor
        jmp     SHORT done
ydiag:
        add     di,[bp-6]
        ror     dl,1
        ror     dl,1
        ror     dh,1
        ror     dh,1
        cmc
        adc     bx,0
        loop    ymajor
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_F_1DD22  ENDP

        END
