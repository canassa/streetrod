; Font glyphs: width and eight raster rows.
FONTDATA SEGMENT PARA PUBLIC 'CODE'
        PUBLIC  _g_font

GLYPH   MACRO   w, r0, r1, r2, r3, r4, r5, r6, r7
        DB      w, r0, r1, r2, r3, r4, r5, r6, r7
        ENDM

_g_font LABEL   BYTE                     ; struct Glyph far[]
        GLYPH   6, 0, 0, 0, 0, 0, 0, 0, 0  ; character 32
        GLYPH   3, 80h, 80h, 80h, 80h, 80h, 0, 80h, 0  ; character 33
        GLYPH   6, 90h, 90h, 90h, 0, 0, 0, 0, 0  ; character 34
        GLYPH   6, 28h, 50h, 0F8h, 50h, 0F8h, 50h, 0A0h, 0  ; character 35
        GLYPH   5, 20h, 70h, 80h, 60h, 10h, 0E0h, 20h, 0  ; character 36
        GLYPH   6, 0C8h, 0C8h, 10h, 20h, 40h, 98h, 98h, 0  ; character 37
        GLYPH   6, 30h, 50h, 20h, 40h, 0A8h, 90h, 68h, 0  ; character 38
        GLYPH   4, 20h, 20h, 40h, 0, 0, 0, 0, 0  ; character 39
        GLYPH   6, 20h, 40h, 80h, 80h, 80h, 40h, 20h, 0  ; character 40
        GLYPH   6, 40h, 20h, 10h, 10h, 10h, 20h, 40h, 0  ; character 41
        GLYPH   6, 0, 0A8h, 70h, 0F8h, 70h, 0A8h, 0, 0  ; character 42
        GLYPH   6, 0, 20h, 20h, 0F8h, 20h, 20h, 0, 0  ; character 43
        GLYPH   4, 0, 0, 0, 0, 0, 40h, 40h, 80h  ; character 44
        GLYPH   6, 0, 0, 0, 78h, 0, 0, 0, 0  ; character 45
        GLYPH   4, 0, 0, 0, 0, 0, 40h, 40h, 0  ; character 46
        GLYPH   6, 8, 8, 10h, 20h, 40h, 80h, 80h, 0  ; character 47
        GLYPH   6, 0, 70h, 88h, 88h, 88h, 88h, 70h, 0  ; character 48
        GLYPH   5, 0, 20h, 60h, 20h, 20h, 20h, 0F0h, 0  ; character 49
        GLYPH   6, 0, 70h, 88h, 10h, 40h, 88h, 0F8h, 0  ; character 50
        GLYPH   6, 0, 70h, 88h, 30h, 8, 88h, 70h, 0  ; character 51
        GLYPH   6, 0, 30h, 50h, 90h, 0F8h, 10h, 10h, 0  ; character 52
        GLYPH   6, 0, 0F8h, 80h, 0F0h, 8, 88h, 70h, 0  ; character 53
        GLYPH   6, 0, 70h, 80h, 0F0h, 88h, 88h, 70h, 0  ; character 54
        GLYPH   6, 0, 0F8h, 88h, 10h, 20h, 20h, 20h, 0  ; character 55
        GLYPH   6, 0, 70h, 88h, 70h, 88h, 88h, 70h, 0  ; character 56
        GLYPH   6, 0, 70h, 88h, 78h, 8, 8, 70h, 0  ; character 57
        GLYPH   4, 0, 40h, 40h, 0, 0, 40h, 40h, 0  ; character 58
        GLYPH   4, 0, 40h, 40h, 0, 0, 40h, 40h, 80h  ; character 59
        GLYPH   6, 8, 10h, 20h, 40h, 20h, 10h, 8, 0  ; character 60
        GLYPH   6, 0, 0, 78h, 0, 0, 78h, 0, 0  ; character 61
        GLYPH   6, 40h, 20h, 10h, 8, 10h, 20h, 40h, 0  ; character 62
        GLYPH   6, 30h, 48h, 8, 10h, 20h, 0, 20h, 0  ; character 63
        GLYPH   6, 70h, 88h, 0B8h, 0A8h, 0B8h, 80h, 78h, 0  ; character 64
        GLYPH   5, 60h, 90h, 90h, 0F0h, 90h, 90h, 90h, 0  ; character 65
        GLYPH   6, 0F0h, 48h, 48h, 70h, 48h, 48h, 0F0h, 0  ; character 66
        GLYPH   5, 60h, 90h, 80h, 80h, 80h, 90h, 60h, 0  ; character 67
        GLYPH   6, 0F0h, 48h, 48h, 48h, 48h, 48h, 0F0h, 0  ; character 68
        GLYPH   6, 0F8h, 48h, 40h, 70h, 40h, 48h, 0F8h, 0  ; character 69
        GLYPH   6, 0F8h, 48h, 40h, 70h, 40h, 40h, 0E0h, 0  ; character 70
        GLYPH   6, 70h, 88h, 80h, 80h, 98h, 88h, 78h, 0  ; character 71
        GLYPH   5, 90h, 90h, 90h, 0F0h, 90h, 90h, 90h, 0  ; character 72
        GLYPH   4, 0E0h, 40h, 40h, 40h, 40h, 40h, 0E0h, 0  ; character 73
        GLYPH   6, 38h, 10h, 10h, 10h, 10h, 90h, 60h, 0  ; character 74
        GLYPH   6, 0C8h, 48h, 50h, 60h, 50h, 48h, 0C8h, 0  ; character 75
        GLYPH   6, 0E0h, 40h, 40h, 40h, 40h, 48h, 0F8h, 0  ; character 76
        GLYPH   6, 88h, 0D8h, 0A8h, 0A8h, 88h, 88h, 88h, 0  ; character 77
        GLYPH   6, 88h, 88h, 0C8h, 0A8h, 98h, 88h, 88h, 0  ; character 78
        GLYPH   5, 60h, 90h, 90h, 90h, 90h, 90h, 60h, 0  ; character 79
        GLYPH   6, 0F0h, 48h, 48h, 70h, 40h, 40h, 0E0h, 0  ; character 80
        GLYPH   6, 70h, 88h, 88h, 88h, 0A8h, 0B8h, 70h, 8  ; character 81
        GLYPH   6, 0F0h, 48h, 48h, 70h, 50h, 50h, 0C8h, 0  ; character 82
        GLYPH   6, 70h, 88h, 0C0h, 30h, 8, 88h, 70h, 0  ; character 83
        GLYPH   6, 0F8h, 0A8h, 20h, 20h, 20h, 20h, 70h, 0  ; character 84
        GLYPH   5, 90h, 90h, 90h, 90h, 90h, 90h, 60h, 0  ; character 85
        GLYPH   6, 88h, 88h, 88h, 88h, 88h, 50h, 20h, 0  ; character 86
        GLYPH   6, 88h, 88h, 88h, 0A8h, 0A8h, 0D8h, 88h, 0  ; character 87
        GLYPH   6, 88h, 88h, 50h, 20h, 50h, 88h, 88h, 0  ; character 88
        GLYPH   6, 88h, 88h, 88h, 70h, 20h, 20h, 70h, 0  ; character 89
        GLYPH   5, 0F0h, 90h, 20h, 40h, 80h, 90h, 0F0h, 0  ; character 90
        GLYPH   7, 0, 0, 10h, 38h, 7Ch, 0FEh, 0, 0  ; character 91
        GLYPH   7, 10h, 38h, 7Ch, 0FEh, 10h, 38h, 7Ch, 0FEh  ; character 92
        GLYPH   7, 0, 0, 0FEh, 7Ch, 38h, 10h, 0, 0  ; character 93
        GLYPH   7, 0FEh, 7Ch, 38h, 10h, 0FEh, 7Ch, 38h, 10h  ; character 94
        GLYPH   6, 0, 0, 0, 0, 0, 0, 0, 0F8h  ; character 95
        GLYPH   2, 0, 0, 0, 0, 0, 0, 0, 0  ; character 96
        GLYPH   6, 0, 0, 60h, 10h, 70h, 90h, 68h, 0  ; character 97
        GLYPH   6, 40h, 40h, 70h, 48h, 48h, 48h, 0B0h, 0  ; character 98
        GLYPH   5, 0, 0, 60h, 90h, 80h, 90h, 60h, 0  ; character 99
        GLYPH   6, 10h, 10h, 70h, 90h, 90h, 90h, 68h, 0  ; character 100
        GLYPH   5, 0, 0, 60h, 90h, 0F0h, 80h, 60h, 0  ; character 101
        GLYPH   5, 20h, 50h, 40h, 0E0h, 40h, 40h, 0C0h, 0  ; character 102
        GLYPH   6, 0, 0, 68h, 90h, 90h, 70h, 10h, 70h  ; character 103
        GLYPH   6, 0C0h, 40h, 70h, 48h, 48h, 48h, 48h, 0  ; character 104
        GLYPH   4, 40h, 0, 0C0h, 40h, 40h, 40h, 60h, 0  ; character 105
        GLYPH   5, 10h, 0, 30h, 10h, 10h, 90h, 90h, 60h  ; character 106
        GLYPH   6, 0C0h, 40h, 48h, 50h, 60h, 50h, 48h, 0  ; character 107
        GLYPH   4, 0C0h, 40h, 40h, 40h, 40h, 40h, 60h, 0  ; character 108
        GLYPH   6, 0, 0, 0D0h, 0A8h, 0A8h, 0A8h, 0A8h, 0  ; character 109
        GLYPH   5, 0, 0, 0E0h, 90h, 90h, 90h, 90h, 0  ; character 110
        GLYPH   5, 0, 0, 60h, 90h, 90h, 90h, 60h, 0  ; character 111
        GLYPH   6, 0, 0, 0B0h, 48h, 48h, 70h, 40h, 0C0h  ; character 112
        GLYPH   6, 0, 0, 68h, 90h, 90h, 70h, 10h, 18h  ; character 113
        GLYPH   6, 0, 0, 0B0h, 68h, 40h, 40h, 40h, 0  ; character 114
        GLYPH   5, 0, 0, 70h, 80h, 60h, 10h, 0E0h, 0  ; character 115
        GLYPH   5, 40h, 40h, 0E0h, 40h, 40h, 50h, 20h, 0  ; character 116
        GLYPH   6, 0, 0, 90h, 90h, 90h, 90h, 68h, 0  ; character 117
        GLYPH   6, 0, 0, 88h, 88h, 88h, 50h, 20h, 0  ; character 118
        GLYPH   6, 0, 0, 88h, 88h, 0A8h, 0D8h, 88h, 0  ; character 119
        GLYPH   6, 0, 0, 88h, 50h, 20h, 50h, 88h, 0  ; character 120
        GLYPH   5, 0, 0, 90h, 90h, 90h, 70h, 10h, 0E0h  ; character 121
        GLYPH   5, 0, 0, 0F0h, 20h, 40h, 80h, 0F0h, 0  ; character 122
FONTDATA ENDS

        END
