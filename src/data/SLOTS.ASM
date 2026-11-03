; 277 Slot records and five 1120-word terrain work arrays. Reconstructed as source records from the shipped data.
SLOTS SEGMENT PARA PUBLIC 'FAR_DATA'
PUBLIC SLOTS_BASE
SLOTS_BASE LABEL BYTE
PUBLIC _g_slots
_g_slots EQU SLOTS_BASE+00h ; struct Slot far @[]
PUBLIC _g_ter33f0
_g_ter33f0 EQU SLOTS_BASE+033F0h ; unsigned far @[]
PUBLIC _g_ter3cb0
_g_ter3cb0 EQU SLOTS_BASE+03CB0h ; unsigned far @[]
PUBLIC _g_ter4570
_g_ter4570 EQU SLOTS_BASE+04570h ; int far @[]
PUBLIC _g_ter4e30
_g_ter4e30 EQU SLOTS_BASE+04E30h ; unsigned far @[]
PUBLIC _g_ter56f0
_g_ter56f0 EQU SLOTS_BASE+056F0h ; int far @[]
 DB 277 * 48 DUP (0) ; Slot[277]
 DW 1120 DUP (0) ; terrain work array 0
 DW 1120 DUP (0) ; terrain work array 1
 DW 1120 DUP (0) ; terrain work array 2
 DW 1120 DUP (0) ; terrain work array 3
 DW 1120 DUP (0) ; terrain work array 4
SLOTS ENDS
END
