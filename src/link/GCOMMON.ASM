; Communals whose C views overlap the next symbol (a typed array or struct that the
; code indexes past its own storage). C cannot declare their exact size, so they are
; declared here; LINK still allocates them in hash-bucket order with the C communals.
        .MODEL MEDIUM
        .DATA
        COMM    NEAR _ai70e4t:BYTE:4            ; DS:70E4 struct Panel @[]
        COMM    NEAR _at70e8:BYTE:516           ; DS:70E8 struct Zone @[]
        COMM    NEAR _ae7312y:BYTE:2            ; DS:7312 struct Bitmap far *@[]
        COMM    NEAR _g77bal:BYTE:6             ; DS:77BA struct Frame @[]
        COMM    NEAR _e_78e0:BYTE:10            ; DS:78E0 struct Eng @
        COMM    NEAR _g79e2g:BYTE:188           ; DS:79E2 struct Eng @
        COMM    NEAR _g7d86n:BYTE:2             ; DS:7D86 struct Spec @[]
        COMM    NEAR _w_7d88:BYTE:6             ; DS:7D88 struct Rec10 @[]
        COMM    NEAR _g7d8ew:BYTE:252           ; DS:7D8E struct Ent10 @[]
        COMM    NEAR _av7ff8:BYTE:8             ; DS:7FF8 struct Racer @[]
        COMM    NEAR _p_8000:BYTE:374           ; DS:8000 struct Rec18 @[]
        COMM    NEAR _r_vid:BYTE:22             ; DS:822A struct Video @
        COMM    NEAR _x_colors:BYTE:2           ; DS:8242 struct Colors @
        COMM    NEAR _w_8df0:BYTE:262           ; DS:8DF0 struct Model @[]
        END
