; sound-effect word instruction streams. Reconstructed as source records from the shipped data.
EFFECTS SEGMENT PARA PUBLIC 'FAR_DATA'
PUBLIC EFFECTS_BASE
EFFECTS_BASE LABEL BYTE
PUBLIC _g_2f93_0000
_g_2f93_0000 EQU EFFECTS_BASE+00h ; char far @[]
PUBLIC _g_2f93_002c
_g_2f93_002c EQU EFFECTS_BASE+02Ch ; char far @[]
PUBLIC _g_2f93_0050
_g_2f93_0050 EQU EFFECTS_BASE+050h ; char far @[]
PUBLIC _g_2f93_007c
_g_2f93_007c EQU EFFECTS_BASE+07Ch ; char far @[]
PUBLIC _g_2f93_00a0
_g_2f93_00a0 EQU EFFECTS_BASE+0A0h ; char far @[]
PUBLIC _g_2f93_00b0
_g_2f93_00b0 EQU EFFECTS_BASE+0B0h ; char far @[]
PUBLIC _g_2f93_00c0
_g_2f93_00c0 EQU EFFECTS_BASE+0C0h ; char far @[]
 DW 00h,00h,01h,023A1h,03h,00h,01h,01FBEh ; sound words +0000
 DW 03h,00h,01h,01DF6h,03h,00h,01h,01FBEh ; sound words +0010
 DW 03h,00h,00h,01h,00h,00h,00h,00h ; sound words +0020
 DW 02h,08E88h,02h,00h,02h,05F1Eh,02h,08E88h ; sound words +0030
 DW 03h,07120h,011h,00h,00h,01h,00h,00h ; sound words +0040
 DW 00h,00h,02h,0FFFFh,06h,00h,02h,0FDFFh ; sound words +0050
 DW 06h,00h,02h,0EFA2h,06h,00h,02h,0FDFFh ; sound words +0060
 DW 06h,00h,00h,01h,00h,00h,00h,00h ; sound words +0070
 DW 03h,0AAh,03h,09B5Ch,03h,0AFh,03h,033C9h ; sound words +0080
 DW 07h,0C0h,07h,07485h,00h,01h,00h,00h ; sound words +0090
 DW 00h,00h,024h,0F8h,00h,01h,00h,00h ; sound words +00A0
 DW 00h,00h,07h,02E9Bh,00h,01h,00h,00h ; sound words +00B0
 DW 00h,00h,01h,08E88h,06h,00h,02h,00h ; sound words +00C0
 DW 01h,07EF6h,06h,00h,02h,00h,01h,077D8h ; sound words +00D0
 DW 06h,00h,02h,00h,01h,07EF6h,06h,00h ; sound words +00E0
 DW 02h,00h,00h,01h,00h,00h,00h,00h ; sound words +00F0
EFFECTS ENDS
END
