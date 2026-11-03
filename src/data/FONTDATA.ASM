; font glyphs: width and eight raster rows. Reconstructed as source records from the shipped data.
FONTDATA SEGMENT PARA PUBLIC 'CODE'
PUBLIC FONTDATA_BASE
FONTDATA_BASE LABEL BYTE
PUBLIC _g_font
_g_font EQU FONTDATA_BASE+00h ; struct Glyph far @[]
GLYPH MACRO w,r0,r1,r2,r3,r4,r5,r6,r7
 DB w,r0,r1,r2,r3,r4,r5,r6,r7
ENDM
 GLYPH 06h,00h,00h,00h,00h,00h,00h,00h,00h ; character 32
 GLYPH 03h,080h,080h,080h,080h,080h,00h,080h,00h ; character 33
 GLYPH 06h,090h,090h,090h,00h,00h,00h,00h,00h ; character 34
 GLYPH 06h,028h,050h,0F8h,050h,0F8h,050h,0A0h,00h ; character 35
 GLYPH 05h,020h,070h,080h,060h,010h,0E0h,020h,00h ; character 36
 GLYPH 06h,0C8h,0C8h,010h,020h,040h,098h,098h,00h ; character 37
 GLYPH 06h,030h,050h,020h,040h,0A8h,090h,068h,00h ; character 38
 GLYPH 04h,020h,020h,040h,00h,00h,00h,00h,00h ; character 39
 GLYPH 06h,020h,040h,080h,080h,080h,040h,020h,00h ; character 40
 GLYPH 06h,040h,020h,010h,010h,010h,020h,040h,00h ; character 41
 GLYPH 06h,00h,0A8h,070h,0F8h,070h,0A8h,00h,00h ; character 42
 GLYPH 06h,00h,020h,020h,0F8h,020h,020h,00h,00h ; character 43
 GLYPH 04h,00h,00h,00h,00h,00h,040h,040h,080h ; character 44
 GLYPH 06h,00h,00h,00h,078h,00h,00h,00h,00h ; character 45
 GLYPH 04h,00h,00h,00h,00h,00h,040h,040h,00h ; character 46
 GLYPH 06h,08h,08h,010h,020h,040h,080h,080h,00h ; character 47
 GLYPH 06h,00h,070h,088h,088h,088h,088h,070h,00h ; character 48
 GLYPH 05h,00h,020h,060h,020h,020h,020h,0F0h,00h ; character 49
 GLYPH 06h,00h,070h,088h,010h,040h,088h,0F8h,00h ; character 50
 GLYPH 06h,00h,070h,088h,030h,08h,088h,070h,00h ; character 51
 GLYPH 06h,00h,030h,050h,090h,0F8h,010h,010h,00h ; character 52
 GLYPH 06h,00h,0F8h,080h,0F0h,08h,088h,070h,00h ; character 53
 GLYPH 06h,00h,070h,080h,0F0h,088h,088h,070h,00h ; character 54
 GLYPH 06h,00h,0F8h,088h,010h,020h,020h,020h,00h ; character 55
 GLYPH 06h,00h,070h,088h,070h,088h,088h,070h,00h ; character 56
 GLYPH 06h,00h,070h,088h,078h,08h,08h,070h,00h ; character 57
 GLYPH 04h,00h,040h,040h,00h,00h,040h,040h,00h ; character 58
 GLYPH 04h,00h,040h,040h,00h,00h,040h,040h,080h ; character 59
 GLYPH 06h,08h,010h,020h,040h,020h,010h,08h,00h ; character 60
 GLYPH 06h,00h,00h,078h,00h,00h,078h,00h,00h ; character 61
 GLYPH 06h,040h,020h,010h,08h,010h,020h,040h,00h ; character 62
 GLYPH 06h,030h,048h,08h,010h,020h,00h,020h,00h ; character 63
 GLYPH 06h,070h,088h,0B8h,0A8h,0B8h,080h,078h,00h ; character 64
 GLYPH 05h,060h,090h,090h,0F0h,090h,090h,090h,00h ; character 65
 GLYPH 06h,0F0h,048h,048h,070h,048h,048h,0F0h,00h ; character 66
 GLYPH 05h,060h,090h,080h,080h,080h,090h,060h,00h ; character 67
 GLYPH 06h,0F0h,048h,048h,048h,048h,048h,0F0h,00h ; character 68
 GLYPH 06h,0F8h,048h,040h,070h,040h,048h,0F8h,00h ; character 69
 GLYPH 06h,0F8h,048h,040h,070h,040h,040h,0E0h,00h ; character 70
 GLYPH 06h,070h,088h,080h,080h,098h,088h,078h,00h ; character 71
 GLYPH 05h,090h,090h,090h,0F0h,090h,090h,090h,00h ; character 72
 GLYPH 04h,0E0h,040h,040h,040h,040h,040h,0E0h,00h ; character 73
 GLYPH 06h,038h,010h,010h,010h,010h,090h,060h,00h ; character 74
 GLYPH 06h,0C8h,048h,050h,060h,050h,048h,0C8h,00h ; character 75
 GLYPH 06h,0E0h,040h,040h,040h,040h,048h,0F8h,00h ; character 76
 GLYPH 06h,088h,0D8h,0A8h,0A8h,088h,088h,088h,00h ; character 77
 GLYPH 06h,088h,088h,0C8h,0A8h,098h,088h,088h,00h ; character 78
 GLYPH 05h,060h,090h,090h,090h,090h,090h,060h,00h ; character 79
 GLYPH 06h,0F0h,048h,048h,070h,040h,040h,0E0h,00h ; character 80
 GLYPH 06h,070h,088h,088h,088h,0A8h,0B8h,070h,08h ; character 81
 GLYPH 06h,0F0h,048h,048h,070h,050h,050h,0C8h,00h ; character 82
 GLYPH 06h,070h,088h,0C0h,030h,08h,088h,070h,00h ; character 83
 GLYPH 06h,0F8h,0A8h,020h,020h,020h,020h,070h,00h ; character 84
 GLYPH 05h,090h,090h,090h,090h,090h,090h,060h,00h ; character 85
 GLYPH 06h,088h,088h,088h,088h,088h,050h,020h,00h ; character 86
 GLYPH 06h,088h,088h,088h,0A8h,0A8h,0D8h,088h,00h ; character 87
 GLYPH 06h,088h,088h,050h,020h,050h,088h,088h,00h ; character 88
 GLYPH 06h,088h,088h,088h,070h,020h,020h,070h,00h ; character 89
 GLYPH 05h,0F0h,090h,020h,040h,080h,090h,0F0h,00h ; character 90
 GLYPH 07h,00h,00h,010h,038h,07Ch,0FEh,00h,00h ; character 91
 GLYPH 07h,010h,038h,07Ch,0FEh,010h,038h,07Ch,0FEh ; character 92
 GLYPH 07h,00h,00h,0FEh,07Ch,038h,010h,00h,00h ; character 93
 GLYPH 07h,0FEh,07Ch,038h,010h,0FEh,07Ch,038h,010h ; character 94
 GLYPH 06h,00h,00h,00h,00h,00h,00h,00h,0F8h ; character 95
 GLYPH 02h,00h,00h,00h,00h,00h,00h,00h,00h ; character 96
 GLYPH 06h,00h,00h,060h,010h,070h,090h,068h,00h ; character 97
 GLYPH 06h,040h,040h,070h,048h,048h,048h,0B0h,00h ; character 98
 GLYPH 05h,00h,00h,060h,090h,080h,090h,060h,00h ; character 99
 GLYPH 06h,010h,010h,070h,090h,090h,090h,068h,00h ; character 100
 GLYPH 05h,00h,00h,060h,090h,0F0h,080h,060h,00h ; character 101
 GLYPH 05h,020h,050h,040h,0E0h,040h,040h,0C0h,00h ; character 102
 GLYPH 06h,00h,00h,068h,090h,090h,070h,010h,070h ; character 103
 GLYPH 06h,0C0h,040h,070h,048h,048h,048h,048h,00h ; character 104
 GLYPH 04h,040h,00h,0C0h,040h,040h,040h,060h,00h ; character 105
 GLYPH 05h,010h,00h,030h,010h,010h,090h,090h,060h ; character 106
 GLYPH 06h,0C0h,040h,048h,050h,060h,050h,048h,00h ; character 107
 GLYPH 04h,0C0h,040h,040h,040h,040h,040h,060h,00h ; character 108
 GLYPH 06h,00h,00h,0D0h,0A8h,0A8h,0A8h,0A8h,00h ; character 109
 GLYPH 05h,00h,00h,0E0h,090h,090h,090h,090h,00h ; character 110
 GLYPH 05h,00h,00h,060h,090h,090h,090h,060h,00h ; character 111
 GLYPH 06h,00h,00h,0B0h,048h,048h,070h,040h,0C0h ; character 112
 GLYPH 06h,00h,00h,068h,090h,090h,070h,010h,018h ; character 113
 GLYPH 06h,00h,00h,0B0h,068h,040h,040h,040h,00h ; character 114
 GLYPH 05h,00h,00h,070h,080h,060h,010h,0E0h,00h ; character 115
 GLYPH 05h,040h,040h,0E0h,040h,040h,050h,020h,00h ; character 116
 GLYPH 06h,00h,00h,090h,090h,090h,090h,068h,00h ; character 117
 GLYPH 06h,00h,00h,088h,088h,088h,050h,020h,00h ; character 118
 GLYPH 06h,00h,00h,088h,088h,0A8h,0D8h,088h,00h ; character 119
 GLYPH 06h,00h,00h,088h,050h,020h,050h,088h,00h ; character 120
 GLYPH 05h,00h,00h,090h,090h,090h,070h,010h,0E0h ; character 121
 GLYPH 05h,00h,00h,0F0h,020h,040h,080h,0F0h,00h ; character 122
FONTDATA ENDS
END
