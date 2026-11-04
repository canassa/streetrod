        .MODEL MEDIUM

        .DATA

; DS:5580: the key maps of g_keyMaps, one per screen: (scan code, key value) pairs, each list
; ended by a 0 byte.
        PUBLIC  _g_5580
_g_5580 DB      31h, 0CDh,  1Ch, 0CDh,  18h, 0CCh,  0  ; DS:5580
K_5587  DB      18h, 0CCh,  1Ch, 0CBh,  0
K_558c  DB      2, 0CFh,  3, 0D0h,  4, 0D1h,  5, 0D2h,  6, 0D3h,  7, 0D4h
        DB      8, 0D5h,  9, 0D6h,  0Ah, 0D7h,  0Bh, 0D8h,  3Bh, 0D9h,  3Ch, 0DAh
        DB      3Dh, 0DBh,  3Eh, 0DCh,  3Fh, 0DDh,  21h, 0CAh,  1, 0CAh,  26h, 0CBh
        DB      1Ch, 0CBh,  0
K_55b3  DB      2Eh, 0CBh,  2, 0CFh,  3, 0D0h,  4, 0D1h,  5, 0D2h,  6, 0D3h
        DB      7, 0D4h,  8, 0D5h,  9, 0D6h,  0Ah, 0D7h,  0Bh, 0D8h,  1Ch, 0CBh
        DB      1, 0CAh,  21h, 0CAh,  0
K_55d0  DB      2, 0CFh,  3, 0D0h,  4, 0D1h,  5, 0D2h,  6, 0D3h,  7, 0D4h
        DB      8, 0D5h,  9, 0D6h,  0Ah, 0D7h,  0Bh, 0D8h,  3Bh, 0D9h,  3Ch, 0DAh
        DB      3Dh, 0DBh,  3Eh, 0DCh,  3Fh, 0DDh,  1Fh, 0CBh,  1Ch, 0CBh,  21h, 0CAh
        DB      1, 0CAh,  0
K_55f7  DB      1Ch, 0BAh,  18h, 0BAh,  0
K_55fc  DB      2Eh, 0CFh,  16h, 0CFh,  19h, 0D0h,  1Eh, 0D0h,  22h, 0D1h,  1, 0D1h
        DB      0
K_5609  DB      19h, 0CDh,  31h, 0CCh,  1, 0CBh,  22h, 0CBh,  0
K_5612  DB      1Ch, 0BAh,  1, 0B9h,  1Fh, 0B5h,  15h, 0BAh,  21h, 0B9h,  0
K_561d  DB      31h, 0CFh,  20h, 0D0h,  2Dh, 0D1h,  10h, 0D2h,  26h, 0D3h,  2Eh, 0D4h
        DB      1Fh, 0DCh,  14h, 0D6h,  1Eh, 0D7h,  23h, 0D8h,  13h, 0DBh,  19h, 0D5h
        DB      22h, 0DDh,  17h, 0DEh,  30h, 0D9h,  21h, 0DAh,  0
K_563e  DB      2, 0CFh,  3, 0D0h,  4, 0D1h,  5, 0D2h,  6, 0D3h,  7, 0D4h
        DB      8, 0D5h,  9, 0D6h,  0Ah, 0D7h,  0Bh, 0D8h,  1Ch, 0CDh,  1Fh, 0CDh
        DB      26h, 0CCh,  1, 0CBh,  21h, 0CBh,  0
K_565d  DB      1Ch, 0B6h,  1, 0B8h,  18h, 0B6h,  31h, 0B8h,  0
K_5666  DB      1Ch, 0BAh,  18h, 0BAh,  31h, 0B9h,  1, 0B9h,  0
K_566f  DB      19h, 0CFh,  31h, 0D0h,  1Ch, 0D1h,  1, 0B7h,  21h, 0B7h,  0
K_567a  DB      1Ch, 0BAh,  18h, 0BAh,  0
K_567f  DB      31h, 0B9h,  18h, 0BAh,  1Ch, 0BAh,  1, 0B9h,  0
K_5688  DB      0
K_5689  DB      2, 0CFh,  3, 0D0h,  4, 0D1h,  1Ch, 0D1h,  1, 0B7h,  21h, 0B7h
        DB      0
K_5696  DB      2, 0D5h,  3, 0D6h,  4, 0D7h,  1Ch, 0D7h,  1, 0B7h,  21h, 0B7h
        DB      0
K_56a3  DB      2, 0CFh,  3, 0D0h,  20h, 0CDh,  1, 0CDh,  1Ch, 0CDh,  1Fh, 0CCh
        DB      0
K_56b0  DB      2, 0CFh,  3, 0D0h,  4, 0D1h,  5, 0D2h,  6, 0D3h,  7, 0D4h
        DB      8, 0D5h,  9, 0D6h,  0Ah, 0D7h,  14h, 0CBh,  19h, 0CCh,  20h, 0CDh
        DB      11h, 0DBh,  1, 0CDh,  1Fh, 0CCh,  0
K_56cf  DB      20h, 0CFh,  13h, 0D0h,  1Ch, 0D0h,  1, 0B7h,  21h, 0B7h,  0
K_56da  DB      13h, 0CFh,  19h, 0D0h,  14h, 0D2h,  0
K_56e1  DB      1Ch, 0B4h,  0
K_56e4  DB      1Ch, 0CFh,  21h, 0CFh,  1, 0D0h,  24h, 0D0h,  0
K_56ed  DB      1Ch, 0CFh,  21h, 0CFh,  1, 0D0h,  24h, 0D0h,  0
K_56f6  DB      31h, 0CDh,  18h, 0CCh,  1Fh, 0CBh,  1Ch, 0CBh,  10h, 0BAh,  1, 0B8h
        DB      21h, 0B8h,  0
K_5705  DB      1, 0CDh,  20h, 0CDh,  1Eh, 0C4h,  13h, 0C7h,  0
K_570e  DB      18h, 0BAh,  1Ch, 0BAh,  0
K_5713  DB      1, 0B9h,  21h, 0B9h,  1Ch, 0BAh,  22h, 0BAh,  0
K_571c  DB      18h, 0BAh,  1Ch, 0BAh,  0
K_5721  DB      18h, 0BAh,  1Ch, 0BAh,  0
K_5726  DB      0
        DB      0                        ; unreferenced empty list
K_5728  DB      1Ch, 0BAh,  1, 0B8h,  18h, 0BAh,  21h, 0B8h,  0
K_5731  DB      2, 0AFh,  3, 0AEh,  4, 0ADh,  5, 0ACh,  6, 0ABh,  7, 0AAh
        DB      8, 0A9h,  9, 0A8h,  1, 0B8h,  1Fh, 0B8h,  0
K_5746  DB      2Eh, 0CBh,  2, 0CFh,  3, 0D0h,  4, 0D1h,  5, 0D2h,  6, 0D3h
        DB      7, 0D4h,  8, 0D5h,  9, 0D6h,  0Ah, 0D7h,  0Bh, 0D8h,  1Ch, 0CBh
        DB      1, 0CAh,  21h, 0CAh,  0
K_5763  DB      22h, 0CFh,  1Ch, 0D0h,  2Eh, 0D0h,  23h, 0D1h,  18h, 0D2h,  0
K_576e  DB      1, 0CBh,  1Ch, 0CBh,  0
K_5773  DB      2, 0CFh,  3, 0D0h,  4, 0D1h,  5, 0D2h,  6, 0D3h,  7, 0D4h
        DB      8, 0D5h,  9, 0D6h,  0Ah, 0D7h,  0Bh, 0D8h,  3Bh, 0D9h,  3Ch, 0DAh
        DB      0
K_578c  DB      31h, 0D1h,  26h, 0D0h,  10h, 0D2h,  1Fh, 0CFh,  0
K_5795  DB      31h, 0CDh,  26h, 0CCh,  10h, 0D2h,  0
K_579c  DB      32h, 0A6h,  0
        PUBLIC  _g_keyMaps
_g_keyMaps DW      42                       ; DS:579F struct KeyMap *[]
        DW      OFFSET DGROUP:_g_5580
        DW      OFFSET DGROUP:K_5587
        DW      OFFSET DGROUP:K_558c
        DW      OFFSET DGROUP:K_55b3
        DW      OFFSET DGROUP:K_55d0
        DW      OFFSET DGROUP:K_55f7
        DW      OFFSET DGROUP:K_55fc
        DW      OFFSET DGROUP:K_5609
        DW      OFFSET DGROUP:K_5612
        DW      OFFSET DGROUP:K_561d
        DW      OFFSET DGROUP:K_563e
        DW      OFFSET DGROUP:K_565d
        DW      OFFSET DGROUP:K_5666
        DW      OFFSET DGROUP:K_566f
        DW      OFFSET DGROUP:K_567a
        DW      OFFSET DGROUP:K_567f
        DW      OFFSET DGROUP:K_5688
        DW      OFFSET DGROUP:K_5689
        DW      OFFSET DGROUP:K_5696
        DW      OFFSET DGROUP:K_56a3
        DW      OFFSET DGROUP:K_56b0
        DW      OFFSET DGROUP:K_56cf
        DW      OFFSET DGROUP:K_56da
        DW      OFFSET DGROUP:K_56e1
        DW      OFFSET DGROUP:K_56e4
        DW      OFFSET DGROUP:K_56ed
        DW      OFFSET DGROUP:K_56f6
        DW      OFFSET DGROUP:K_5705
        DW      OFFSET DGROUP:K_570e
        DW      OFFSET DGROUP:K_5713
        DW      OFFSET DGROUP:K_571c
        DW      OFFSET DGROUP:K_5721
        DW      OFFSET DGROUP:K_5726
        DW      OFFSET DGROUP:K_5728
        DW      OFFSET DGROUP:K_5731
        DW      OFFSET DGROUP:K_5746
        DW      OFFSET DGROUP:K_5763
        DW      OFFSET DGROUP:K_576e
        DW      OFFSET DGROUP:K_5773
        DW      OFFSET DGROUP:K_578c
        DW      OFFSET DGROUP:K_5795
        DW      OFFSET DGROUP:K_579c
        EVEN

        .CODE S0000_TEXT

; hercdet_ram_test @ 0000:E56A (linear 0x0E56A, 26 bytes)
; Is there RAM at es:3FFFh that holds a written value? Result in ZF
; (equal: the byte at 7FFFh read back the same after the increment).

        PUBLIC  _hercdet_ram_test
_hercdet_ram_test  PROC  FAR
        mov     bl,es:[7FFFh]
        mov     cl,es:[3FFFh]
        inc     BYTE PTR es:[3FFFh]
        cmp     bl,es:[3FFFh]
        mov     es:[3FFFh],cl
        ret
_hercdet_ram_test  ENDP

; hercdet_detect @ 0000:E584 (linear 0x0E584, 63 bytes)
; Hercules detection: does the status port 3BAh toggle bit 7, and is
; there memory at B000h that survives the config switch.

        PUBLIC  _hercdet_detect
_hercdet_detect  PROC  FAR
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
        call    _hercdet_ram_test
        jne     found
        mov     al,1
        mov     dx,3BFh
        out     dx,al
        call    _hercdet_ram_test
        je      none
found:
        mov     al,1
        jmp     SHORT done
none:
        xor     al,al
done:
        or      al,al
        ret
_hercdet_detect  ENDP

; hercdet_bios_mono @ 0000:E5C3 (linear 0x0E5C3, 14 bytes)
; BIOS equipment word: 0x30 in the video bits means monochrome.

        PUBLIC  _hercdet_bios_mono
_hercdet_bios_mono  PROC  FAR
        int     11h
        and     ax,30h
        cmp     ax,30h
        je      mono
        mov     ax,0
mono:
        ret
_hercdet_bios_mono  ENDP

        END
