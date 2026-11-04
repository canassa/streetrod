; CGA glyphs: width and eight word rasters; segment offset four is linker alignment.
CGAFONT SEGMENT WORD PUBLIC 'CODE'
        PUBLIC  _g_cgaFont

GLYPH   MACRO   w, r0, r1, r2, r3, r4, r5, r6, r7
        DW      w, r0, r1, r2, r3, r4, r5, r6, r7
        ENDM

_g_cgaFont LABEL   BYTE                  ; struct CgaGlyph far[]
        GLYPH   6, 0, 0, 0, 0, 0, 0, 0, 0  ; character 32
        GLYPH   3, 0C000h, 0C000h, 0C000h, 0C000h, 0C000h, 0, 0C000h, 0  ; character 33
        GLYPH   6, 0C300h, 0C300h, 0C300h, 0, 0, 0, 0, 0  ; character 34
        GLYPH   6, 0CC0h, 3300h, 0FFC0h, 3300h, 0FFC0h, 3300h, 0CC00h, 0  ; character 35
        GLYPH   5, 0C00h, 3F00h, 0C000h, 3C00h, 300h, 0FC00h, 0C00h, 0  ; character 36
        GLYPH   6, 0F0C0h, 0F0C0h, 300h, 0C00h, 3000h, 0C3C0h, 0C3C0h, 0  ; character 37
        GLYPH   6, 0F00h, 3300h, 0C00h, 3000h, 0CCC0h, 0C300h, 3CC0h, 0  ; character 38
        GLYPH   4, 0C00h, 0C00h, 3000h, 0, 0, 0, 0, 0  ; character 39
        GLYPH   6, 0C00h, 3000h, 0C000h, 0C000h, 0C000h, 3000h, 0C00h, 0  ; character 40
        GLYPH   6, 3000h, 0C00h, 300h, 300h, 300h, 0C00h, 3000h, 0  ; character 41
        GLYPH   6, 0, 0CCC0h, 3F00h, 0FFC0h, 3F00h, 0CCC0h, 0, 0  ; character 42
        GLYPH   6, 0, 0C00h, 0C00h, 0FFC0h, 0C00h, 0C00h, 0, 0  ; character 43
        GLYPH   4, 0, 0, 0, 0, 0, 3000h, 3000h, 0C000h  ; character 44
        GLYPH   6, 0, 0, 0, 3FC0h, 0, 0, 0, 0  ; character 45
        GLYPH   4, 0, 0, 0, 0, 0, 3000h, 3000h, 0  ; character 46
        GLYPH   6, 0C0h, 0C0h, 300h, 0C00h, 3000h, 0C000h, 0C000h, 0  ; character 47
        GLYPH   6, 0, 3F00h, 0C0C0h, 0C0C0h, 0C0C0h, 0C0C0h, 3F00h, 0  ; character 48
        GLYPH   5, 0, 0C00h, 3C00h, 0C00h, 0C00h, 0C00h, 0FF00h, 0  ; character 49
        GLYPH   6, 0, 3F00h, 0C0C0h, 300h, 3000h, 0C0C0h, 0FFC0h, 0  ; character 50
        GLYPH   6, 0, 3F00h, 0C0C0h, 0F00h, 0C0h, 0C0C0h, 3F00h, 0  ; character 51
        GLYPH   6, 0, 0F00h, 3300h, 0C300h, 0FFC0h, 300h, 300h, 0  ; character 52
        GLYPH   6, 0, 0FFC0h, 0C000h, 0FF00h, 0C0h, 0C0C0h, 3F00h, 0  ; character 53
        GLYPH   6, 0, 3F00h, 0C000h, 0FF00h, 0C0C0h, 0C0C0h, 3F00h, 0  ; character 54
        GLYPH   6, 0, 0FFC0h, 0C0C0h, 300h, 0C00h, 0C00h, 0C00h, 0  ; character 55
        GLYPH   6, 0, 3F00h, 0C0C0h, 3F00h, 0C0C0h, 0C0C0h, 3F00h, 0  ; character 56
        GLYPH   6, 0, 3F00h, 0C0C0h, 3FC0h, 0C0h, 0C0h, 3F00h, 0  ; character 57
        GLYPH   4, 0, 3000h, 3000h, 0, 0, 3000h, 3000h, 0  ; character 58
        GLYPH   4, 0, 3000h, 3000h, 0, 0, 3000h, 3000h, 0C000h  ; character 59
        GLYPH   6, 0C0h, 300h, 0C00h, 3000h, 0C00h, 300h, 0C0h, 0  ; character 60
        GLYPH   6, 0, 0, 3FC0h, 0, 0, 3FC0h, 0, 0  ; character 61
        GLYPH   6, 3000h, 0C00h, 300h, 0C0h, 300h, 0C00h, 3000h, 0  ; character 62
        GLYPH   6, 0F00h, 30C0h, 0C0h, 300h, 0C00h, 0, 0C00h, 0  ; character 63
        GLYPH   6, 3F00h, 0C0C0h, 0CFC0h, 0CCC0h, 0CFC0h, 0C000h, 3FC0h, 0  ; character 64
        GLYPH   5, 3C00h, 0C300h, 0C300h, 0FF00h, 0C300h, 0C300h, 0C300h, 0  ; character 65
        GLYPH   6, 0FF00h, 30C0h, 30C0h, 3F00h, 30C0h, 30C0h, 0FF00h, 0  ; character 66
        GLYPH   5, 3C00h, 0C300h, 0C000h, 0C000h, 0C000h, 0C300h, 3C00h, 0  ; character 67
        GLYPH   6, 0FF00h, 30C0h, 30C0h, 30C0h, 30C0h, 30C0h, 0FF00h, 0  ; character 68
        GLYPH   6, 0FFC0h, 30C0h, 3000h, 3F00h, 3000h, 30C0h, 0FFC0h, 0  ; character 69
        GLYPH   6, 0FFC0h, 30C0h, 3000h, 3F00h, 3000h, 3000h, 0FC00h, 0  ; character 70
        GLYPH   6, 3F00h, 0C0C0h, 0C000h, 0C000h, 0C3C0h, 0C0C0h, 3FC0h, 0  ; character 71
        GLYPH   5, 0C300h, 0C300h, 0C300h, 0FF00h, 0C300h, 0C300h, 0C300h, 0  ; character 72
        GLYPH   4, 0FC00h, 3000h, 3000h, 3000h, 3000h, 3000h, 0FC00h, 0  ; character 73
        GLYPH   6, 0FC0h, 300h, 300h, 300h, 300h, 0C300h, 3C00h, 0  ; character 74
        GLYPH   6, 0F0C0h, 30C0h, 3300h, 3C00h, 3300h, 30C0h, 0F0C0h, 0  ; character 75
        GLYPH   6, 0FC00h, 3000h, 3000h, 3000h, 3000h, 30C0h, 0FFC0h, 0  ; character 76
        GLYPH   6, 0C0C0h, 0F3C0h, 0CCC0h, 0CCC0h, 0C0C0h, 0C0C0h, 0C0C0h, 0  ; character 77
        GLYPH   6, 0C0C0h, 0C0C0h, 0F0C0h, 0CCC0h, 0C3C0h, 0C0C0h, 0C0C0h, 0  ; character 78
        GLYPH   5, 3C00h, 0C300h, 0C300h, 0C300h, 0C300h, 0C300h, 3C00h, 0  ; character 79
        GLYPH   6, 0FF00h, 30C0h, 30C0h, 3F00h, 3000h, 3000h, 0FC00h, 0  ; character 80
        GLYPH   6, 3F00h, 0C0C0h, 0C0C0h, 0C0C0h, 0CCC0h, 0CFC0h, 3F00h, 0C0h  ; character 81
        GLYPH   6, 0FF00h, 30C0h, 30C0h, 3F00h, 3300h, 3300h, 0F0C0h, 0  ; character 82
        GLYPH   6, 3F00h, 0C0C0h, 0F000h, 0F00h, 0C0h, 0C0C0h, 3F00h, 0  ; character 83
        GLYPH   6, 0FFC0h, 0CCC0h, 0C00h, 0C00h, 0C00h, 0C00h, 3F00h, 0  ; character 84
        GLYPH   5, 0C300h, 0C300h, 0C300h, 0C300h, 0C300h, 0C300h, 3C00h, 0  ; character 85
        GLYPH   6, 0C0C0h, 0C0C0h, 0C0C0h, 0C0C0h, 0C0C0h, 3300h, 0C00h, 0  ; character 86
        GLYPH   6, 0C0C0h, 0C0C0h, 0C0C0h, 0CCC0h, 0CCC0h, 0F3C0h, 0C0C0h, 0  ; character 87
        GLYPH   6, 0C0C0h, 0C0C0h, 3300h, 0C00h, 3300h, 0C0C0h, 0C0C0h, 0  ; character 88
        GLYPH   6, 0C0C0h, 0C0C0h, 0C0C0h, 3F00h, 0C00h, 0C00h, 3F00h, 0  ; character 89
        GLYPH   5, 0FF00h, 0C300h, 0C00h, 3000h, 0C000h, 0C300h, 0FF00h, 0  ; character 90
        GLYPH   7, 0, 0, 300h, 0FC0h, 3FF0h, 0FFFCh, 0, 0  ; character 91
        GLYPH   7, 300h, 0FC0h, 3FF0h, 0FFFCh, 300h, 0FC0h, 3FF0h, 0FFFCh  ; character 92
        GLYPH   7, 0, 0, 0FFFCh, 3FF0h, 0FC0h, 300h, 0, 0  ; character 93
        GLYPH   7, 0FFFCh, 3FF0h, 0FC0h, 300h, 0FFFCh, 3FF0h, 0FC0h, 300h  ; character 94
        GLYPH   6, 0, 0, 0, 0, 0, 0, 0, 0FFC0h  ; character 95
        GLYPH   2, 0, 0, 0, 0, 0, 0, 0, 0  ; character 96
        GLYPH   6, 0, 0, 3C00h, 300h, 3F00h, 0C300h, 3CC0h, 0  ; character 97
        GLYPH   6, 3000h, 3000h, 3F00h, 30C0h, 30C0h, 30C0h, 0CF00h, 0  ; character 98
        GLYPH   5, 0, 0, 3C00h, 0C300h, 0C000h, 0C300h, 3C00h, 0  ; character 99
        GLYPH   6, 300h, 300h, 3F00h, 0C300h, 0C300h, 0C300h, 3CC0h, 0  ; character 100
        GLYPH   5, 0, 0, 3C00h, 0C300h, 0FF00h, 0C000h, 3C00h, 0  ; character 101
        GLYPH   5, 0C00h, 3300h, 3000h, 0FC00h, 3000h, 3000h, 0F000h, 0  ; character 102
        GLYPH   6, 0, 0, 3CC0h, 0C300h, 0C300h, 3F00h, 300h, 3F00h  ; character 103
        GLYPH   6, 0F000h, 3000h, 3F00h, 30C0h, 30C0h, 30C0h, 30C0h, 0  ; character 104
        GLYPH   4, 3000h, 0, 0F000h, 3000h, 3000h, 3000h, 3C00h, 0  ; character 105
        GLYPH   5, 300h, 0, 0F00h, 300h, 300h, 0C300h, 0C300h, 3C00h  ; character 106
        GLYPH   6, 0F000h, 3000h, 30C0h, 3300h, 3C00h, 3300h, 30C0h, 0  ; character 107
        GLYPH   4, 0F000h, 3000h, 3000h, 3000h, 3000h, 3000h, 3C00h, 0  ; character 108
        GLYPH   6, 0, 0, 0F300h, 0CCC0h, 0CCC0h, 0CCC0h, 0CCC0h, 0  ; character 109
        GLYPH   5, 0, 0, 0FC00h, 0C300h, 0C300h, 0C300h, 0C300h, 0  ; character 110
        GLYPH   5, 0, 0, 3C00h, 0C300h, 0C300h, 0C300h, 3C00h, 0  ; character 111
        GLYPH   6, 0, 0, 0CF00h, 30C0h, 30C0h, 3F00h, 3000h, 0F000h  ; character 112
        GLYPH   6, 0, 0, 3CC0h, 0C300h, 0C300h, 3F00h, 300h, 3C0h  ; character 113
        GLYPH   6, 0, 0, 0CF00h, 3CC0h, 3000h, 3000h, 3000h, 0  ; character 114
        GLYPH   5, 0, 0, 3F00h, 0C000h, 3C00h, 300h, 0FC00h, 0  ; character 115
        GLYPH   5, 3000h, 3000h, 0FC00h, 3000h, 3000h, 3300h, 0C00h, 0  ; character 116
        GLYPH   6, 0, 0, 0C300h, 0C300h, 0C300h, 0C300h, 3CC0h, 0  ; character 117
        GLYPH   6, 0, 0, 0C0C0h, 0C0C0h, 0C0C0h, 3300h, 0C00h, 0  ; character 118
        GLYPH   6, 0, 0, 0C0C0h, 0C0C0h, 0CCC0h, 0F3C0h, 0C0C0h, 0  ; character 119
        GLYPH   6, 0, 0, 0C0C0h, 3300h, 0C00h, 3300h, 0C0C0h, 0  ; character 120
        GLYPH   5, 0, 0, 0C300h, 0C300h, 0C300h, 3F00h, 300h, 0FC00h  ; character 121
        GLYPH   5, 0, 0, 0FF00h, 0C00h, 3000h, 0C000h, 0FF00h, 0  ; character 122
CGAFONT ENDS

        END
