; screen Bitmap descriptor. Reconstructed as source records from the shipped data.
SCREEN2 SEGMENT PARA PUBLIC 'FAR_DATA'
PUBLIC SCREEN2_BASE
SCREEN2_BASE LABEL BYTE
PUBLIC _g_2fa3_0000
_g_2fa3_0000 EQU SCREEN2_BASE+00h ; struct Bitmap far @
        DW 0140h ; +0000 bitmap[0].width
        DW 0C8h ; +0002 bitmap[0].height
        DW 01F40h ; +0004 bitmap[0].planesize
        DB 00h ; +0006 bitmap[0].pad[0]
        DB 00h ; +0007 bitmap[0].pad[1]
        DB 00h ; +0008 bitmap[0].pad[2]
        DB 00h ; +0009 bitmap[0].pad[3]
        DB 00h ; +000A bitmap[0].pad[4]
        DB 00h ; +000B bitmap[0].pad[5]
        DB 00h ; +000C bitmap[0].pad[6]
        DB 00h ; +000D bitmap[0].pad[7]
        DB 00h ; +000E bitmap[0].pad[8]
        DB 00h ; +000F bitmap[0].pad[9]
        DB 00h ; +0010 bitmap[0].pad[10]
        DB 00h ; +0011 bitmap[0].pad[11]
        DB 00h ; +0012 bitmap[0].pad[12]
        DB 00h ; +0013 bitmap[0].pad[13]
        DB 00h ; +0014 bitmap[0].pad[14]
        DB 00h ; +0015 bitmap[0].pad[15]
        DB 00h ; +0016 bitmap[0].pad[16]
        DB 00h ; +0017 bitmap[0].pad[17]
        DB 00h ; +0018 bitmap[0].pad[18]
        DB 00h ; +0019 bitmap[0].pad[19]
        DB 00h ; +001A bitmap[0].pad[20]
        DB 00h ; +001B bitmap[0].pad[21]
        DB 00h ; +001C bitmap[0].pad[22]
        DB 00h ; +001D bitmap[0].pad[23]
        DB 0FFh ; +001E bitmap[0].pad[24]
        DB 00h ; +001F bitmap[0].pad[25]
        DB 00h ; +0020 bitmap[0].pad[26]
        DB 00h ; +0021 bitmap[0].pad[27]
        DB 00h ; +0022 bitmap[0].pad[28]
        DB 00h ; +0023 bitmap[0].pad[29]
        DD 0A0000000h ; +0024 bitmap[0].bits.p
        DD 00h ; +0028 bitmap[0].alt.p
        DW 028h ; +002C bitmap[0].pitch
        DB 0FEh ; +002E bitmap[0].flags
        DB 1 DUP (0) ; +002F zero-initialized storage
SCREEN2 ENDS
END
