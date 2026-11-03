; three screen Bitmap descriptors. Reconstructed as source records from the shipped data.
SCREENS SEGMENT PARA PUBLIC 'FAR_DATA'
PUBLIC SCREENS_BASE
SCREENS_BASE LABEL BYTE
PUBLIC _g_screens
_g_screens EQU SCREENS_BASE+00h ; struct Bitmap far @[]
PUBLIC _g_screen1
_g_screen1 EQU SCREENS_BASE+030h ; struct Bitmap far @
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
        DB 0FFh ; +001C bitmap[0].pad[22]
        DB 0FFh ; +001D bitmap[0].pad[23]
        DB 00h ; +001E bitmap[0].pad[24]
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
        DW 0140h ; +0030 bitmap[1].width
        DW 0C8h ; +0032 bitmap[1].height
        DW 01F40h ; +0034 bitmap[1].planesize
        DB 00h ; +0036 bitmap[1].pad[0]
        DB 00h ; +0037 bitmap[1].pad[1]
        DB 00h ; +0038 bitmap[1].pad[2]
        DB 00h ; +0039 bitmap[1].pad[3]
        DB 00h ; +003A bitmap[1].pad[4]
        DB 00h ; +003B bitmap[1].pad[5]
        DB 00h ; +003C bitmap[1].pad[6]
        DB 00h ; +003D bitmap[1].pad[7]
        DB 00h ; +003E bitmap[1].pad[8]
        DB 00h ; +003F bitmap[1].pad[9]
        DB 00h ; +0040 bitmap[1].pad[10]
        DB 00h ; +0041 bitmap[1].pad[11]
        DB 00h ; +0042 bitmap[1].pad[12]
        DB 00h ; +0043 bitmap[1].pad[13]
        DB 00h ; +0044 bitmap[1].pad[14]
        DB 00h ; +0045 bitmap[1].pad[15]
        DB 00h ; +0046 bitmap[1].pad[16]
        DB 00h ; +0047 bitmap[1].pad[17]
        DB 00h ; +0048 bitmap[1].pad[18]
        DB 00h ; +0049 bitmap[1].pad[19]
        DB 00h ; +004A bitmap[1].pad[20]
        DB 00h ; +004B bitmap[1].pad[21]
        DB 00h ; +004C bitmap[1].pad[22]
        DB 00h ; +004D bitmap[1].pad[23]
        DB 0FFh ; +004E bitmap[1].pad[24]
        DB 00h ; +004F bitmap[1].pad[25]
        DB 00h ; +0050 bitmap[1].pad[26]
        DB 00h ; +0051 bitmap[1].pad[27]
        DB 00h ; +0052 bitmap[1].pad[28]
        DB 00h ; +0053 bitmap[1].pad[29]
        DD 0A2000000h ; +0054 bitmap[1].bits.p
        DD 00h ; +0058 bitmap[1].alt.p
        DW 028h ; +005C bitmap[1].pitch
        DB 0FEh ; +005E bitmap[1].flags
        DB 1 DUP (0) ; +005F zero-initialized storage
        DW 0140h ; +0060 bitmap[2].width
        DW 0C8h ; +0062 bitmap[2].height
        DW 04000h ; +0064 bitmap[2].planesize
        DB 00h ; +0066 bitmap[2].pad[0]
        DB 00h ; +0067 bitmap[2].pad[1]
        DB 00h ; +0068 bitmap[2].pad[2]
        DB 00h ; +0069 bitmap[2].pad[3]
        DB 00h ; +006A bitmap[2].pad[4]
        DB 00h ; +006B bitmap[2].pad[5]
        DB 00h ; +006C bitmap[2].pad[6]
        DB 00h ; +006D bitmap[2].pad[7]
        DB 00h ; +006E bitmap[2].pad[8]
        DB 00h ; +006F bitmap[2].pad[9]
        DB 00h ; +0070 bitmap[2].pad[10]
        DB 00h ; +0071 bitmap[2].pad[11]
        DB 00h ; +0072 bitmap[2].pad[12]
        DB 00h ; +0073 bitmap[2].pad[13]
        DB 00h ; +0074 bitmap[2].pad[14]
        DB 00h ; +0075 bitmap[2].pad[15]
        DB 00h ; +0076 bitmap[2].pad[16]
        DB 00h ; +0077 bitmap[2].pad[17]
        DB 00h ; +0078 bitmap[2].pad[18]
        DB 00h ; +0079 bitmap[2].pad[19]
        DB 00h ; +007A bitmap[2].pad[20]
        DB 00h ; +007B bitmap[2].pad[21]
        DB 00h ; +007C bitmap[2].pad[22]
        DB 00h ; +007D bitmap[2].pad[23]
        DB 0FFh ; +007E bitmap[2].pad[24]
        DB 00h ; +007F bitmap[2].pad[25]
        DB 00h ; +0080 bitmap[2].pad[26]
        DB 00h ; +0081 bitmap[2].pad[27]
        DB 00h ; +0082 bitmap[2].pad[28]
        DB 00h ; +0083 bitmap[2].pad[29]
        DD 0A0002000h ; +0084 bitmap[2].bits.p
        DD 00h ; +0088 bitmap[2].alt.p
        DW 050h ; +008C bitmap[2].pitch
        DB 0FDh ; +008E bitmap[2].flags
        DB 1 DUP (0) ; +008F zero-initialized storage
SCREENS ENDS
END
