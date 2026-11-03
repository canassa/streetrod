; F_21A06 @ 21A0:0006 (linear 0x21A06, 14 bytes)
; Wait for the start of vertical retrace on the CRT status port 3DAh:
; first until the display is not in the middle of a retrace, then until bit 3 sets.

        .MODEL  MEDIUM
        .CODE

        PUBLIC  _F_21A06
_F_21A06  PROC    FAR
        mov     dx,3DAh
wait1:
        in      al,dx
        test    al,1
        jne     wait1
wait2:
        in      al,dx
        test    al,8
        je      wait2
        ret
_F_21A06  ENDP

        END
