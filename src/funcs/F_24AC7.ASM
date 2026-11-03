; F_24AC7 @ 2487:0257 (linear 0x24AC7, 178 bytes)
; Clipped horizontal span fill in the 16-colour planar-less (nibble packed)
; framebuffer: fill_span(x1, x2, y, color), clipped to the clip window.

        .MODEL  MEDIUM
        .DATA
        EXTRN   _g_clip_xmin:WORD
        EXTRN   _g_clip_ymin:WORD
        EXTRN   _g_clip_ymax:WORD
        EXTRN   _g_clip_xmax:WORD

        .CODE
        EXTRN   F_24A9B:NEAR

        PUBLIC  _fill_span_clipped
_fill_span_clipped  PROC  FAR
        push    bp
        mov     bp,sp
        push    si
        push    di
        mov     ax,[bp+10]              ; y
        cmp     _g_clip_ymin,ax
        jg      done_jmp
        cmp     _g_clip_ymax,ax
        jl      done_jmp
        mov     ax,[bp+6]               ; x1
        mov     bx,[bp+8]               ; x2
        mov     cx,_g_clip_xmin
        mov     dx,_g_clip_xmax
        cmp     cx,bx
        jg      done_jmp
        cmp     dx,ax
        jl      done_jmp
        jmp     short clip
done_jmp:
        jmp     short done
clip:
        cmp     ax,cx
        jg      x1_ok
        mov     [bp+6],cx
x1_ok:
        cmp     bx,dx
        jl      x2_ok
        mov     [bp+8],dx
x2_ok:
        mov     ax,[bp+6]
        cmp     ax,[bp+8]
        jle     ordered
        xchg    ax,[bp+8]
        mov     [bp+6],ax
ordered:
        mov     ax,[bp+10]
        mov     bx,[bp+6]
        call    F_24A9B
        mov     di,bx
        mov     dh,ah
        not     dh
        mov     dl,0FFh
        shl     dh,cl
        not     dh
        mov     cx,[bp+8]
        and     cl,1
        jne     odd_end
        mov     dl,0F0h
odd_end:
        mov     ax,[bp+8]
        mov     bx,[bp+6]
        shr     ax,1
        shr     bx,1
        mov     cx,ax
        sub     cx,bx
        mov     al,[bp+12]
        mov     ah,al
        shl     al,1
        shl     al,1
        shl     al,1
        shl     al,1
        or      al,ah
        or      dh,dh
        js      edge_mask
        or      cx,cx
        jne     first_byte
        and     dl,dh
        jmp     short last_byte
first_byte:
        mov     ah,al
        and     ah,dh
        not     dh
        and     es:[di],dh
        or      es:[di],ah
        inc     di
        dec     cx
edge_mask:
        rep stosb
last_byte:
        and     al,dl
        not     dl
        and     es:[di],dl
        or      es:[di],al
done:
        pop     di
        pop     si
        mov     sp,bp
        pop     bp
        ret
_fill_span_clipped  ENDP

        END
