        .MODEL MEDIUM

        .DATA

; DS:5580 unsigned char[543]
        PUBLIC  _g_5580
_g_5580 DB      31h, 0CDh, 1Ch, 0CDh, 18h, 0CCh, 0, 18h, 0CCh, 1Ch, 0CBh
        DB      0, 2, 0CFh, 3, 0D0h, 4, 0D1h, 5, 0D2h, 6, 0D3h, 7, 0D4h
        DB      8, 0D5h, 9, 0D6h, 0Ah, 0D7h, 0Bh, 0D8h, 3Bh, 0D9h, 3Ch
        DB      0DAh, 3Dh, 0DBh, 3Eh, 0DCh, 3Fh, 0DDh, 21h, 0CAh, 1
        DB      0CAh, 26h, 0CBh, 1Ch, 0CBh, 0, 2Eh, 0CBh, 2, 0CFh, 3
        DB      0D0h, 4, 0D1h, 5, 0D2h, 6, 0D3h, 7, 0D4h, 8, 0D5h, 9
        DB      0D6h, 0Ah, 0D7h, 0Bh, 0D8h, 1Ch, 0CBh, 1, 0CAh, 21h
        DB      0CAh, 0, 2, 0CFh, 3, 0D0h, 4, 0D1h, 5, 0D2h, 6, 0D3h, 7
        DB      0D4h, 8, 0D5h, 9, 0D6h, 0Ah, 0D7h, 0Bh, 0D8h, 3Bh, 0D9h
        DB      3Ch, 0DAh, 3Dh, 0DBh, 3Eh, 0DCh, 3Fh, 0DDh, 1Fh, 0CBh
        DB      1Ch, 0CBh, 21h, 0CAh, 1, 0CAh, 0, 1Ch, 0BAh, 18h, 0BAh
        DB      0, 2Eh, 0CFh, 16h, 0CFh, 19h, 0D0h, 1Eh, 0D0h, 22h, 0D1h
        DB      1, 0D1h, 0, 19h, 0CDh, 31h, 0CCh, 1, 0CBh, 22h, 0CBh, 0
        DB      1Ch, 0BAh, 1, 0B9h, 1Fh, 0B5h, 15h, 0BAh, 21h, 0B9h, 0
        DB      31h, 0CFh, 20h, 0D0h, 2Dh, 0D1h, 10h, 0D2h, 26h, 0D3h
        DB      2Eh, 0D4h, 1Fh, 0DCh, 14h, 0D6h, 1Eh, 0D7h, 23h, 0D8h
        DB      13h, 0DBh, 19h, 0D5h, 22h, 0DDh, 17h, 0DEh, 30h, 0D9h
        DB      21h, 0DAh, 0, 2, 0CFh, 3, 0D0h, 4, 0D1h, 5, 0D2h, 6
        DB      0D3h, 7, 0D4h, 8, 0D5h, 9, 0D6h, 0Ah, 0D7h, 0Bh, 0D8h
        DB      1Ch, 0CDh, 1Fh, 0CDh, 26h, 0CCh, 1, 0CBh, 21h, 0CBh, 0
        DB      1Ch, 0B6h, 1, 0B8h, 18h, 0B6h, 31h, 0B8h, 0, 1Ch, 0BAh
        DB      18h, 0BAh, 31h, 0B9h, 1, 0B9h, 0, 19h, 0CFh, 31h, 0D0h
        DB      1Ch, 0D1h, 1, 0B7h, 21h, 0B7h, 0, 1Ch, 0BAh, 18h, 0BAh
        DB      0, 31h, 0B9h, 18h, 0BAh, 1Ch, 0BAh, 1, 0B9h, 0, 0, 2
        DB      0CFh, 3, 0D0h, 4, 0D1h, 1Ch, 0D1h, 1, 0B7h, 21h, 0B7h, 0
        DB      2, 0D5h, 3, 0D6h, 4, 0D7h, 1Ch, 0D7h, 1, 0B7h, 21h, 0B7h
        DB      0, 2, 0CFh, 3, 0D0h, 20h, 0CDh, 1, 0CDh, 1Ch, 0CDh, 1Fh
        DB      0CCh, 0, 2, 0CFh, 3, 0D0h, 4, 0D1h, 5, 0D2h, 6, 0D3h, 7
        DB      0D4h, 8, 0D5h, 9, 0D6h, 0Ah, 0D7h, 14h, 0CBh, 19h, 0CCh
        DB      20h, 0CDh, 11h, 0DBh, 1, 0CDh, 1Fh, 0CCh, 0, 20h, 0CFh
        DB      13h, 0D0h, 1Ch, 0D0h, 1, 0B7h, 21h, 0B7h, 0, 13h, 0CFh
        DB      19h, 0D0h, 14h, 0D2h, 0, 1Ch, 0B4h, 0, 1Ch, 0CFh, 21h
        DB      0CFh, 1, 0D0h, 24h, 0D0h, 0, 1Ch, 0CFh, 21h, 0CFh, 1
        DB      0D0h, 24h, 0D0h, 0, 31h, 0CDh, 18h, 0CCh, 1Fh, 0CBh, 1Ch
        DB      0CBh, 10h, 0BAh, 1, 0B8h, 21h, 0B8h, 0, 1, 0CDh, 20h
        DB      0CDh, 1Eh, 0C4h, 13h, 0C7h, 0, 18h, 0BAh, 1Ch, 0BAh, 0
        DB      1, 0B9h, 21h, 0B9h, 1Ch, 0BAh, 22h, 0BAh, 0, 18h, 0BAh
        DB      1Ch, 0BAh, 0, 18h, 0BAh, 1Ch, 0BAh, 0, 0, 0, 1Ch, 0BAh
        DB      1, 0B8h, 18h, 0BAh, 21h, 0B8h, 0, 2, 0AFh, 3, 0AEh, 4
        DB      0ADh, 5, 0ACh, 6, 0ABh, 7, 0AAh, 8, 0A9h, 9, 0A8h, 1
        DB      0B8h, 1Fh, 0B8h, 0, 2Eh, 0CBh, 2, 0CFh, 3, 0D0h, 4, 0D1h
        DB      5, 0D2h, 6, 0D3h, 7, 0D4h, 8, 0D5h, 9, 0D6h, 0Ah, 0D7h
        DB      0Bh, 0D8h, 1Ch, 0CBh, 1, 0CAh, 21h, 0CAh, 0, 22h, 0CFh
        DB      1Ch, 0D0h, 2Eh, 0D0h, 23h, 0D1h, 18h, 0D2h, 0, 1, 0CBh
        DB      1Ch, 0CBh, 0, 2, 0CFh, 3, 0D0h, 4, 0D1h, 5, 0D2h, 6
        DB      0D3h, 7, 0D4h, 8, 0D5h, 9, 0D6h, 0Ah, 0D7h, 0Bh, 0D8h
        DB      3Bh, 0D9h, 3Ch, 0DAh, 0, 31h, 0D1h, 26h, 0D0h, 10h, 0D2h
        DB      1Fh, 0CFh, 0, 31h, 0CDh, 26h, 0CCh, 10h, 0D2h, 0, 32h
        DB      0A6h, 0
        PUBLIC  _g_579f
_g_579f DW      42                       ; DS:579F struct KeyMap *[]
        DW      OFFSET DGROUP:_g_5580
        DW      OFFSET DGROUP:_g_5580+7
        DW      OFFSET DGROUP:_g_5580+12
        DW      OFFSET DGROUP:_g_5580+51
        DW      OFFSET DGROUP:_g_5580+80
        DW      OFFSET DGROUP:_g_5580+119
        DW      OFFSET DGROUP:_g_5580+124
        DW      OFFSET DGROUP:_g_5580+137
        DW      OFFSET DGROUP:_g_5580+146
        DW      OFFSET DGROUP:_g_5580+157
        DW      OFFSET DGROUP:_g_5580+190
        DW      OFFSET DGROUP:_g_5580+221
        DW      OFFSET DGROUP:_g_5580+230
        DW      OFFSET DGROUP:_g_5580+239
        DW      OFFSET DGROUP:_g_5580+250
        DW      OFFSET DGROUP:_g_5580+255
        DW      OFFSET DGROUP:_g_5580+264
        DW      OFFSET DGROUP:_g_5580+265
        DW      OFFSET DGROUP:_g_5580+278
        DW      OFFSET DGROUP:_g_5580+291
        DW      OFFSET DGROUP:_g_5580+304
        DW      OFFSET DGROUP:_g_5580+335
        DW      OFFSET DGROUP:_g_5580+346
        DW      OFFSET DGROUP:_g_5580+353
        DW      OFFSET DGROUP:_g_5580+356
        DW      OFFSET DGROUP:_g_5580+365
        DW      OFFSET DGROUP:_g_5580+374
        DW      OFFSET DGROUP:_g_5580+389
        DW      OFFSET DGROUP:_g_5580+398
        DW      OFFSET DGROUP:_g_5580+403
        DW      OFFSET DGROUP:_g_5580+412
        DW      OFFSET DGROUP:_g_5580+417
        DW      OFFSET DGROUP:_g_5580+422
        DW      OFFSET DGROUP:_g_5580+424
        DW      OFFSET DGROUP:_g_5580+433
        DW      OFFSET DGROUP:_g_5580+454
        DW      OFFSET DGROUP:_g_5580+483
        DW      OFFSET DGROUP:_g_5580+494
        DW      OFFSET DGROUP:_g_5580+499
        DW      OFFSET DGROUP:_g_5580+524
        DW      OFFSET DGROUP:_g_5580+533
        DW      OFFSET DGROUP:_g_5580+540
        EVEN

        .CODE S0000_TEXT

; F_0E56A @ 0000:E56A (linear 0x0E56A, 26 bytes)
; Is there RAM at es:3FFFh that holds a written value? Result in ZF
; (equal: the byte at 7FFFh read back the same after the increment).

        PUBLIC  _F_0E56A
_F_0E56A  PROC  FAR
        mov     bl,es:[7FFFh]
        mov     cl,es:[3FFFh]
        inc     BYTE PTR es:[3FFFh]
        cmp     bl,es:[3FFFh]
        mov     es:[3FFFh],cl
        ret
_F_0E56A  ENDP

; F_0E584 @ 0000:E584 (linear 0x0E584, 63 bytes)
; Hercules detection: does the status port 3BAh toggle bit 7, and is
; there memory at B000h that survives the config switch.

        PUBLIC  _F_0E584
_F_0E584  PROC  FAR
        mov     bx,100h
        mov     dx,3BAh
        in      al,dx
        mov     ah,al
        and     ah,80h
wait1:
        mov     cx,40h
wait2:
        in      al,dx
        and     al,80h
        cmp     al,ah
        loope   wait2
        jne     toggled
        dec     bx
        jne     wait1
        jmp     SHORT none
toggled:
        mov     ax,0B000h
        mov     es,ax
        call    _F_0E56A
        jne     found
        mov     al,1
        mov     dx,3BFh
        out     dx,al
        call    _F_0E56A
        je      none
found:
        mov     al,1
        jmp     SHORT done
none:
        xor     al,al
done:
        or      al,al
        ret
_F_0E584  ENDP

; F_0E5C3 @ 0000:E5C3 (linear 0x0E5C3, 14 bytes)
; BIOS equipment word: 0x30 in the video bits means monochrome.

        PUBLIC  _F_0E5C3
_F_0E5C3  PROC  FAR
        int     11h
        and     ax,30h
        cmp     ax,30h
        je      mono
        mov     ax,0
mono:
        ret
_F_0E5C3  ENDP

        END
