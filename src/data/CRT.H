/* The original game accesses these MSC globals directly. Library code and game
   code must share their storage; they are not separate game-state variables.
   DOSRET writes _doserrno at old DS:6318 and errno at DS:630D.
   AMALLOC exports _amblksiz (initially 0x2000) at old DS:64A6. */
#define g_630d errno
#define g_6318 _doserrno
#define g_64a6 _amblksiz
