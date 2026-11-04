/* The game reads three MSC runtime variables by their own names, declared by MSC's
   headers: errno (DS:630D) and _doserrno (DS:6318), written by DOSRET, and _amblksiz
   (DS:64A6, initially 0x2000) from AMALLOC. They are library storage, not game state. */
